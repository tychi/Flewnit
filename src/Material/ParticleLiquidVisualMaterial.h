/*
 * ParticleLiquidVisualMaterial.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once


#include "Material/VisualMaterial.h"


namespace Flewnit
{

class ParticleLiquidVisualMaterial
	: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	ParticleLiquidVisualMaterial(
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
			float refractiveIndexFluid,	//usually >1.0

			//to have at least a fast, dirty hack for the CV-Tag, I'll implement a
			//quick n' dirty visualization first
			bool sophisticatedRendering

			//Texture* noiseTexture, <-- perlin noise is procedurally generated.. isn't it?
			//							 anyway, for time reasons, gonna load image from hardcoded
			//							 location ;( (14.7.2011)
			//Texture2DCube* envmap <-- grab globally managed one
			);

	virtual ~ParticleLiquidVisualMaterial();


	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
				SimulationPipelineStage* currentStage,
				SubObject* currentUsingSuboject) throw(SimulatorException)
	{
		assert(0&&"this is a multipass rendering material; implement the several passes directly"
				"without the classical approach");
	}
	virtual void deactivate(SimulationPipelineStage* currentStage,
				SubObject* currentUsingSuboject) throw(SimulatorException)
	{
		assert(0&&"this is a multipass rendering material; implement the several passes directly"
				"without the classical approach");
	}

private:
	friend class ParticleLiquidDrawStage;
	friend class ParticleLiquidShader;




	unsigned int numCurvatureFlowRelaxationSteps;
	float curvatureDisplacementFactorPerStep;
	float silhouetteThreshold;
	float fluidTextureScaleFactor;
	float particleDrawRadius;
	Vector4D liquidColor;
	float thicknessAttenuationFactor;
	float refractionStrengthBias;
	float foamGenerationAccelerationThreshold;
	Vector4D foamColor;
	float reflectivity;
	float shininess;
	float refractiveIndexNonFluid;
	float refractiveIndexFluid;
	bool sophisticatedRendering;


	Shader* mDepthAndAccelGenerationShader;
	Shader* mCurvatureFlowRelaxationShader;
	Shader* mThicknessAndNoiseGenerationShader;
	Shader* mCompositionShader;

	PingPongBuffer* mDepthPingPongTexture;
	Texture2D* mThicknessTexture;
	Texture2D* mNoiseRenderedTexture;
	Texture2D* mNoiseSourceTexture;

};

}


