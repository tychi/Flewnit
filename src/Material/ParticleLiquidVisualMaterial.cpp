/*
 * ParticleLiquidVisualMaterial.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#include "ParticleLiquidVisualMaterial.h"
#include "Util/Log/Log.h"

#include "URE.h"
#include "MPP/Shader/ParticleLiquidShader.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "Buffer/BufferSharedDefinitions.h"
#include "Buffer/Texture.h"
#include "Buffer/PingPongBuffer.h"
#include "Util/Loader/Loader.h"

namespace Flewnit {

ParticleLiquidVisualMaterial::ParticleLiquidVisualMaterial(
		String name,
		unsigned int numCurvatureFlowRelaxationSteps,
		float curvatureDisplacementFactorPerStep,
		//depthvalue to decide if there is a "different fluid layer", which must not be smoothed
		float silhouetteThreshold,
		//to save performance in the repeated curvature flow steps and in the
		//thickness&noise generation step, intermediate results should be computed in
		//a lower resolution and upsampled later
		float fluidTextureScaleFactor,

		float particleDrawRadius,

		//color to be interpolated with the background scene texture,
		//based on thickness
		const Vector4D & liquidColor,
		//value to scale influence actually computed thickness to yield
		//a customizable transparency effect: as thickness can be in [0..infinity],
		//we need to adjust this value for for different domain sizes;
		float thicknessAttenuationFactor,
		//when looking up the background image for the refracted color,
		//this factor influences how much "pseudo-refraction" is caused by the fluid;
		//it's a fake parameter as we have no efficient way to compute exact refraction (yet);
		//corresponds to gamma value in [Green08];
		float refractionStrengthBias,

		//acceleration value to start with adding foam
		float foamGenerationAccelerationThreshold,
		//usually grey-ish color, but feel free to customize as you wish
		const Vector4D & foamColor,

		//used as constant factor for specular highlight computation
		float reflectivity,
		//classical phong-exponent for specular highlight
		float shininess,

		//"brechungsindices", for Fresnel effect;
		float refractiveIndexNonFluid, //usually 1.0
		float refractiveIndexFluid, //usually >1.0

		//to have at least a fast, dirty hack for the CV-Tag, I'll implement a
		//quick n' dirty visualization first
		bool sophisticatedRendering
		)

:
	VisualMaterial(	name,
			VISUAL_MATERIAL_TYPE_LIQUID_RENDERING,
			ShadingFeatures(SHADING_FEATURE_CUBE_MAPPING|SHADING_FEATURE_DIRECT_LIGHTING),
			std::map<BufferSemantics, Texture*>(),
			//no shadow casting, time runs out
			VisualMaterialFlags(false,true,true,false,false,true)
	),

	numCurvatureFlowRelaxationSteps(numCurvatureFlowRelaxationSteps),
	curvatureDisplacementFactorPerStep(curvatureDisplacementFactorPerStep),
	silhouetteThreshold(silhouetteThreshold),
	fluidTextureScaleFactor(fluidTextureScaleFactor),
	particleDrawRadius(particleDrawRadius),
	liquidColor(liquidColor),
	thicknessAttenuationFactor(thicknessAttenuationFactor),
	refractionStrengthBias(refractionStrengthBias),
	foamGenerationAccelerationThreshold(foamGenerationAccelerationThreshold),
	foamColor(foamColor),
	reflectivity(reflectivity),
	shininess(shininess),
	refractiveIndexNonFluid(refractiveIndexNonFluid),
	refractiveIndexFluid(refractiveIndexFluid),
	sophisticatedRendering(sophisticatedRendering),

	mDepthAndAccelGenerationShader(
		sophisticatedRendering ? new ParticleLiquidShader(PARTLIQU_DEPTH_ACCEL_GENERATION_TYPE) :0),
	mCurvatureFlowRelaxationShader(
		sophisticatedRendering ? new ParticleLiquidShader(PARTLIQU_CURVATURE_FLOW_TYPE) :0),

	mThicknessAndNoiseGenerationShader(
		sophisticatedRendering ? new ParticleLiquidShader(PARTLIQU_THICKNESS_NOISE_GENERATION_TYPE) :0),

	mCompositionShader(
		sophisticatedRendering
			? new ParticleLiquidShader(PARTLIQU_SOPHISTICATED_RENDERING_TYPE)
			: new ParticleLiquidShader(PARTLIQU_DIRECT_RENDERING_TYPE)
	),

	//init is complicated
	mDepthPingPongTexture(0),
	mThicknessTexture(0),
	mNoiseRenderedTexture(0),
	mNoiseSourceTexture(0)

{
	mCompositionShader->build();

	BufferElementInfo texeli(1,GPU_DATA_TYPE_FLOAT,32,false);

	mNoiseSourceTexture =
		dynamic_cast<Texture2D*>(
			URE_INSTANCE->getLoader()->loadTexture(
			"partLiquidNoiseSourceTex",NOISE_SEMANTICS,
			Path("./assets/textures/perlinNoiseTex512x512gray.jpg"),
			texeli,true,false,false
		)
	);
	assert("noise texture must be a texture 2D" && mNoiseSourceTexture);
	//TODO handle noise texture resolution to yield a good lookup pattern

	if(sophisticatedRendering)
	{
		mDepthAndAccelGenerationShader->build();
		mCurvatureFlowRelaxationShader->build();
		mThicknessAndNoiseGenerationShader->build();


		Vector2D textureSizes =
				Vector2D(WindowManager::getInstance().getWindowResolution().x,
						WindowManager::getInstance().getWindowResolution().y)
				* fluidTextureScaleFactor;

		Texture2D* depthPing = new Texture2D(
				"partLiquidDepthTexPing", DEPTH_BUFFER_SEMANTICS,
				textureSizes.x,textureSizes.y,
				texeli,true,false,false,0,false
				);
		Texture2D* depthPong = new Texture2D(
				"partLiquidDepthTexPong", DEPTH_BUFFER_SEMANTICS,
				textureSizes.x,textureSizes.y,
				texeli,true,false,false,0,false
				);

		mDepthPingPongTexture = new PingPongBuffer("partLiquidDepthTexPingPong",depthPing,depthPong);

		mThicknessTexture= new Texture2D(
				//TODO include thickness semantics
				"partLiquidThicknessTex", CUSTOM_SEMANTICS,
				textureSizes.x,textureSizes.y,
				texeli,true,false,false,0,false
				);
		mNoiseRenderedTexture= new Texture2D(
				"partLiquidNoiseRenderedTex", NOISE_SEMANTICS,
				textureSizes.x,textureSizes.y,
				texeli,true,false,false,0,false
				);
	}

}


ParticleLiquidVisualMaterial::~ParticleLiquidVisualMaterial()
{

}


//check for equality in order to check if a material with the desired properties
//(shader feature set and textures) already exists in the ResourceManager;
bool ParticleLiquidVisualMaterial::operator==(const Material& rhs) const
{
	return VisualMaterial::operator ==(rhs);
	//TODO refine ;/ no time anymore
}


//void ParticleLiquidVisualMaterial::activate(
//		SimulationPipelineStage* currentStage,
//		SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//	assert(0 && "TODO implement");
//}
//
//void ParticleLiquidVisualMaterial::deactivate(SimulationPipelineStage* currentStage,
//		SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//	assert(0 && "TODO implement");
//}


}
