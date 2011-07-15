/*
 * ParticleLiquidShader.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */

#include "ParticleLiquidShader.h"

#include "MPP/Shader/ShaderManager.h"
#include "Simulator/ParallelComputeManager.h"
#include "Buffer/Texture.h"
#include "MPP/MPP.h"
#include "Buffer/BufferHelperUtils.h"

#include <grantlee/engine.h>
#include "UserInterface/WindowManager/WindowManager.h"
#include "Material/ParticleLiquidVisualMaterial.h"
#include "URE.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "WorldObject/SubObject.h"
#include "Simulator/SimulationPipelineStage.h"
#include "Simulator/SimulatorInterface.h"
#include "Simulator/SimulationResourceManager.h"
#include "WorldObject/SkyDome.h"
#include "Buffer/PingPongBuffer.h"



namespace Flewnit
{

	ParticleLiquidShader::ParticleLiquidShader(ParticleLiquidShaderType type)
	:
	Shader(
		ShaderManager::getInstance().getShaderCodeDirectory()/Path("Fluid/Liquid/Particlebased/"),
		Path(""),
		//ShaderManager::getInstance().getShaderCodeDirectory()/Path("/Fluid/Liquid"),
		//Path("/Particlebased"),

		ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,TEXTURE_TYPE_2D,
					VISUAL_MATERIAL_TYPE_LIQUID_RENDERING,
					ShadingFeatures( SHADING_FEATURE_DIRECT_LIGHTING | SHADING_FEATURE_CUBE_MAPPING),
					false),
		String("ParticleLiquidShader_")+particleLiquidShaderTypeStrings[type]
	  ),
	  mParticleLiquidShaderType(type)
	{

	}

	ParticleLiquidShader::~ParticleLiquidShader()
	{

	}




	void ParticleLiquidShader::build()
	{
		initBuild();

		//build vert and frag shader
		generateShaderStage(VERTEX_SHADER_STAGE,mTemplateEngine,*mTemplateContextMap);
		generateShaderStage(FRAGMENT_SHADER_STAGE,mTemplateEngine,*mTemplateContextMap);

		finishBuild();

	}

	void ParticleLiquidShader::setupTemplateContext(TemplateContextMap& contextMap)
	{
		contextMap.insert("GL_MAYOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().x);
		contextMap.insert("GL_MINOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().y);

		for(unsigned int i = 0; i < __NUM_TOTAL_SEMANTICS__;i++)
		{
			//add the numeric value of the semantics enum in order to define the layout qualifier for the vertex shader
			contextMap.insert(
					BufferHelper::BufferSemanticsToString(BufferSemantics(i)).c_str(),
					i
			);
		}


		//TODO make shader source and context dependent from texture type to show

		for(unsigned int i = 0; i < __NUM_PARTICLE_LIQU_SHADER_TYPES__;i++)
		{
			contextMap.insert(
					(particleLiquidShaderTypeStrings[i]).c_str(),
					(bool) ( (unsigned int)(mParticleLiquidShaderType) == i)
			);
		}


	}

	void ParticleLiquidShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
	{

	}


	void ParticleLiquidShader::use(SubObject* so)throw(SimulatorException)
	{
		//-------------------------------------
		GUARD(glUseProgram(mGLProgramHandle));

		ParticleLiquidVisualMaterial* liquidMat =
				dynamic_cast<ParticleLiquidVisualMaterial*>(so->getMaterial());
		assert("ParticleLiquidShader::use(): type of passed Texture and type of initially defined must fit"
				&& ( liquidMat != 0) );

		Camera *cam = URE_INSTANCE->getCurrentlyActiveCamera();
		//-------------------------------------

		Vector2D viewPortSizes = Vector2D(
			static_cast<float>(WindowManager::getInstance().getWindowResolution().x),
			static_cast<float>(WindowManager::getInstance().getWindowResolution().y)
		);
		if(	  !(mParticleLiquidShaderType == PARTLIQU_DIRECT_RENDERING_TYPE )
				&&
			  !(mParticleLiquidShaderType == PARTLIQU_SOPHISTICATED_RENDERING_TYPE )   )
		{
			viewPortSizes *= liquidMat->fluidTextureScaleFactor;
		}


		bindFloat("particlePointSizePrecomputedFactor",
			liquidMat->particleDrawRadius * std::max(viewPortSizes.x,viewPortSizes.y)
			/ ( glm::tan( glm::radians(cam->getVerticalFOVAngle()))) // TODO *2 ?
		);


		//-------------------------------------
		Matrix4x4 modelViewMatrix =
				cam->getViewMatrix()
				* so->getOwningWorldObject()->getGlobalTransform().getTotalTransform();
		Matrix4x4 inverseModelViewMatrix = glm::inverse(modelViewMatrix);
		Matrix4x4 modelViewProjectionMatrix = cam->getProjectionMatrix() * modelViewMatrix;
		bindMatrix4x4("modelViewMatrix", modelViewMatrix);
		bindMatrix4x4("inverseModelViewMatrix", inverseModelViewMatrix);
		bindMatrix4x4("modelViewProjectionMatrix", modelViewProjectionMatrix);
		//-------------------------------------

		//-------------------------------------

		bindVector2D("viewPortSizes",viewPortSizes);
		//focal length y == cot(openingAngle/2)
		//focal length x == focal length y / aspecRatioXtoY
		//... seemingly according to gluPerspective... but i don't get is completely.. no time..
		bindVector2D("focalLengths",
			Vector2D(
			    1.0f / glm::tan( glm::radians( 0.5f * cam->getVerticalFOVAngle())) / cam->getAspectRatioXtoY() ,
				1.0f / glm::tan( glm::radians( 0.5f * cam->getVerticalFOVAngle()))	)
		);


		bindFloat("curvatureDisplacementFactorPerStep",liquidMat->curvatureDisplacementFactorPerStep);
		bindFloat("silhouetteThreshold",liquidMat->silhouetteThreshold);
		bindVector2D("sceneTextureSampleInterval",
			Vector2D( 1.0f / WindowManager::getInstance().getWindowResolution().x,
					  1.0f /WindowManager::getInstance().getWindowResolution().y	)
		);
		bindVector2D("fluidTexturesSampleInterval",
			Vector2D( 1.0f / ( liquidMat->fluidTextureScaleFactor * WindowManager::getInstance().getWindowResolution().x) ,
					  1.0f / ( liquidMat->fluidTextureScaleFactor * WindowManager::getInstance().getWindowResolution().y)	)
		);

		bindFloat("particleDrawRadius",liquidMat->particleDrawRadius);
		bindVector4D("liquidColor",liquidMat->liquidColor);
		bindFloat("thicknessAttenuationFactor",liquidMat->thicknessAttenuationFactor);
		bindFloat("refractionStrengthBias",liquidMat->refractionStrengthBias);
		bindFloat("foamGenerationAccelerationThreshold",liquidMat->foamGenerationAccelerationThreshold);
		bindVector4D("foamColor",liquidMat->foamColor);
		bindFloat("reflectivity",liquidMat->reflectivity);
		bindFloat("shininess",liquidMat->shininess);

		//fresnel reflectance for perpendicular incident light ray: ((n1-n2)/(n1+n2))^2
		//needed for Schlick's approximation in Fragment shader:
		float r0=
			  (liquidMat->refractiveIndexNonFluid  - liquidMat->refractiveIndexFluid)
			/ (liquidMat->refractiveIndexNonFluid  + liquidMat->refractiveIndexFluid);
		r0=r0*r0;
		bindFloat("r0",r0);
		//-------------------------------------


		//------------------------------------

		Texture* renderingOfDefaultLightingStage =
			dynamic_cast<Texture*>(
				URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)
				->getStage("DefaultLightingStage")->getRenderingResult(FINAL_RENDERING_SEMANTICS)
			);
		assert("The DefaultLightingStage must expose a Texture with final rendering semantics! "
				&& renderingOfDefaultLightingStage);

		GUARD(glActiveTexture(GL_TEXTURE0 ));
		GUARD(renderingOfDefaultLightingStage->bind(OPEN_GL_CONTEXT_TYPE));
		bindInt("backGroundSceneTexture",0);

		assert("has skydome" && SimulationResourceManager::getInstance().getCurrentSkyDome());
		GUARD(glActiveTexture(GL_TEXTURE1 ));
		GUARD(SimulationResourceManager::getInstance().getCurrentSkyDome()->getCubeMap()->bind(OPEN_GL_CONTEXT_TYPE));
		bindInt("cubeMap",1);

		if(
				(mParticleLiquidShaderType == PARTLIQU_CURVATURE_FLOW_TYPE)
		)
		{
			GUARD(glActiveTexture(GL_TEXTURE1 ));
			//bind the inactive buffer, because the active is the write buffer
			GUARD(liquidMat->mDepthPingPongTexture->getInactiveBuffer()->bind(OPEN_GL_CONTEXT_TYPE));
			bindInt("depthTexture",1);
		}
		if(
				(mParticleLiquidShaderType == PARTLIQU_SOPHISTICATED_RENDERING_TYPE)
				||(mParticleLiquidShaderType == PARTLIQU_THICKNESS_NOISE_GENERATION_TYPE)
		)
		{
			GUARD(glActiveTexture(GL_TEXTURE1 ));
			//bind the active buffer, because there is no write buffer needed anymore
			GUARD(liquidMat->mDepthPingPongTexture->getActiveBuffer()->bind(OPEN_GL_CONTEXT_TYPE));
			bindInt("depthTexture",1);
		}

		if(
				(mParticleLiquidShaderType == PARTLIQU_THICKNESS_NOISE_GENERATION_TYPE)
				||(mParticleLiquidShaderType == PARTLIQU_DIRECT_RENDERING_TYPE)
		)
		{
			GUARD(glActiveTexture(GL_TEXTURE2 ));
			//bind the active buffer, because there is no write buffer needed anymore
			GUARD(liquidMat->mNoiseSourceTexture->bind(OPEN_GL_CONTEXT_TYPE));
			bindInt("noiseSourceTexture",2);
		}

		if(
				(mParticleLiquidShaderType == PARTLIQU_SOPHISTICATED_RENDERING_TYPE)
		)
		{
			GUARD(glActiveTexture(GL_TEXTURE3 ));
			//bind the active buffer, because there is no write buffer needed anymore
			GUARD(liquidMat->mThicknessTexture->bind(OPEN_GL_CONTEXT_TYPE));
			bindInt("thicknessTexture",3);

			GUARD(glActiveTexture(GL_TEXTURE4 ));
			//bind the active buffer, because there is no write buffer needed anymore
			GUARD(liquidMat->mNoiseRenderedTexture->bind(OPEN_GL_CONTEXT_TYPE));
			bindInt("noiseRenderedTexture",4);
		}


	}

}
