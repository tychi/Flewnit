/*
 * LightSourceManager.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSourceManager.h"

#include "Buffer/Buffer.h"

#include <boost/foreach.hpp>
#include "Buffer/BufferHelperUtils.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"


namespace Flewnit
{

LightSourceManager::LightSourceManager()
:	mNumCurrentActiveLightingLightSources(0),
	mNumCurrentActiveShadowingLightSources(0)
{
	//hardCode init: TODO make config loadable
	mLightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT;
	mLightSourcesShadowFeature	= LIGHT_SOURCES_SHADOW_FEATURE_NONE;
	mNumMaxLightSources = 4;
	mNumMaxShadowCasters = 4;
	mShadowMapResolution = 512;
	//end hardcode

	assert(mNumMaxShadowCasters <= mNumMaxLightSources);

	if(
		(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT )
	 ||	(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT )
	)
	{
		mNumMaxLightSources = 1;
		mNumMaxShadowCasters = 1;
		assert("shadow feature may not involve more lightsources than the lighting feature" &&
			(mLightSourcesShadowFeature != LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	)
		);
	}
	else
	{
		if(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_NONE )
		{
			assert(mLightSourcesShadowFeature == LIGHT_SOURCES_SHADOW_FEATURE_NONE);
			mNumMaxLightSources = 0;
			mNumMaxShadowCasters = 0;
		}
		else
		{
			//keep init-values
		}



		//further validation comes later, if necessary
	}

	mLightSourceProjectionMatrixNearClipPlane = 0.1f;
	mLightSourceProjectionMatrixFarClipPlane  = 100.0f;

	mLightSourceUniformBuffer = 0;
	if(
		(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
	 ||	(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	 ||	(mLightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
	)
	{
		mLightSourceUniformBuffer = new Buffer(
			BufferInfo(
				String("LightSourceUniformBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				LIGHT_SOURCE_BUFFER_SEMANTICS,
				TYPE_FLOAT,
				mNumMaxLightSources *
					//number of floats inside a LightSourceShaderStruct
					sizeof(LightSourceShaderStruct) / BufferHelper::elementSize(TYPE_FLOAT),
				UNIFORM_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			//yes, the contents are mostly modded when moving lightsources are involved
			true,
			//set no data yet
			0
		);
	}


	mShadowMapMatricesUniformBuffer=0;
	if(mLightSourcesShadowFeature == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS)
	{
		mShadowMapMatricesUniformBuffer = new Buffer(
			BufferInfo(
				String("ShadowMapMatricesUniformBuffer"),
				ContextTypeFlags( HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				TRANSFORMATION_MATRICES_SEMANTICS,
				TYPE_MATRIX44F,
				mNumMaxLightSources,
				UNIFORM_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			//yes, the contents are mostly modded when moving lightsources are involved
			true,
			//set no data yet
			0
		);
	}

	//ShaderFeatures shaderFeatures= ShaderFeatures();

	mShadowMapRenderTarget=0;
	if(mLightSourcesShadowFeature != LIGHT_SOURCES_SHADOW_FEATURE_NONE)
	{
		TextureType texType;

		switch(mLightSourcesShadowFeature)
		{
		case LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT:
			texType = TEXTURE_TYPE_2D;
			mNumMaxShadowCasters = 1;
			break;
		case LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT:
			texType = TEXTURE_TYPE_2D_CUBE;
			mNumMaxShadowCasters = 1;
			break;
		case LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS:
			texType = TEXTURE_TYPE_2D_ARRAY;
			//keep ini shadowcaster value
			break;
		default: break; //omit waning that the NONE value wasn't checked
		}

		mShadowMapRenderTarget= new RenderTarget(
				String("LightSourceManagerShadowMapGernerationRT"),
				Vector2Dui(mShadowMapResolution,mShadowMapResolution),
				texType,
				DEPTH_TEXTURE,
				TexelInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
				1,
				mNumMaxShadowCasters
				);
	}

	//mShadowMapGenerationShader;
	//TODO outsource shadowmap logic to simpipelinestage
}


LightSourceManager::~LightSourceManager()
{
	for(unsigned int i=0; i< mLightSources.size();i++)
	{
		//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
		//has to remove the list-entry before actually call the destructor on the LS;
		//as calls to erase() make problems during itteration, we just set the entry to zero
		LightSource* tmp = mLightSources[i];
		mLightSources[i] = 0;
		delete tmp;
	}
}


//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
PointLight* LightSourceManager::createPointLight(
		const Vector3D& position,
		bool castsShadows,
		const Vector3D& diffuseColor,
		const Vector3D& specularColor
) throw(SimulatorException)
{

}


//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
SpotLight* LightSourceManager::createSpotLight(
		const Vector3D& position,
		const Vector3D& direction,
		bool castsShadows,
		float innerSpotCutOff_Degrees,
		float outerSpotCutOff_Degrees,
		float spotExponent,
		const Vector3D& diffuseColor,
		const Vector3D& specularColor
) throw(SimulatorException)
{

}



//FrustumCulling wont't be implemented too soon ;(
void LightSourceManager::renderShadowMaps(float maxDistanceToMainCam, bool doFrustumCulling)
{

}

//fill buffers with recent values
void LightSourceManager::setupBuffersForShading(float maxDistanceToMainCam)
{

}




//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
//has to remove the list-entry before actually call the destructor on the LS;
void LightSourceManager::unregisterLightSource(LightSource* ls)
{
	//BOOST_FOREACH(LightSource* currentLs, mLightSources)
	for(unsigned int i=0; i< mLightSources.size();i++)
	{
		if(ls == mLightSources[i])
		{
			//this is an indicator that the manager has NOT deleted the lightsource itself;
			//otherwise, the entry where the ls was stored before would now be 0;
			mLightSources.erase( mLightSources.begin() + i);
			break;
		}
	}
}



}
