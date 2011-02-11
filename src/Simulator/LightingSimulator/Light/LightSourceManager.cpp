/*
 * LightSourceManager.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSourceManager.h"

#include "Buffer/Buffer.h"

#include "Buffer/BufferHelperUtils.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"

#include <boost/foreach.hpp>
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit
{

LightSourceManager::LightSourceManager()
:	mNumCurrentActiveLightingLightSources(0),
	mNumCurrentActiveShadowingLightSources(0)
{

	mLightSourceProjectionMatrixNearClipPlane = 0.1f;
	mLightSourceProjectionMatrixFarClipPlane  = 100.0f;

	mLightSourceUniformBuffer = 0;
	if(
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
	)
	{
		mLightSourceUniformBuffer = new Buffer(
			BufferInfo(
				String("LightSourceUniformBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				LIGHT_SOURCE_BUFFER_SEMANTICS,
				TYPE_FLOAT,
				ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources *
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
	if(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
		 == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)
	{
		mShadowMapMatricesUniformBuffer = new Buffer(
			BufferInfo(
				String("ShadowMapMatricesUniformBuffer"),
				ContextTypeFlags( HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				TRANSFORMATION_MATRICES_SEMANTICS,
				TYPE_MATRIX44F,
				ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources,
				UNIFORM_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			//yes, the contents are mostly modded when moving lightsources are involved
			true,
			//set no data yet
			0
		);
	}



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
	//TODO
	assert(0&&"//TODO");
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
	//TODO
	assert(0&&"//TODO");
}




//fill buffers with recent values
void LightSourceManager::setupBuffersForShading(float maxDistanceToMainCam)
{
	//TODO
	assert(0&&"//TODO");
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
