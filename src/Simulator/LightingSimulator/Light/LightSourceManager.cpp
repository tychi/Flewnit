/*
 * LightSourceManager.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSourceManager.h"


#include "Buffer/BufferHelperUtils.h"
#include "Buffer/Buffer.h"

#include "LightSource.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"

#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"

#include "MPP/Shader/ShaderManager.h"

#include <boost/foreach.hpp>
#include <sstream>





namespace Flewnit
{

LightSourceManager::LightSourceManager()
	//mNumCurrentActiveLightingLightSources(0),
	//mNumCurrentActiveShadowingLightSources(0)
{

	mLightSourceProjectionMatrixNearClipPlane = 0.1f;
	mLightSourceProjectionMatrixFarClipPlane  = 100.0f;

	mLightSourcesUniformBuffer = 0;
	if(
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
	)
	{
		mLightSourcesUniformBuffer = new Buffer(
			BufferInfo(
				String("LightSourceUniformBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				LIGHT_SOURCE_BUFFER_SEMANTICS,
				TYPE_FLOAT,
				ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources *
					//number of floats inside a LightSourceShaderStruct
					sizeof(LightSourceShaderStruct) / BufferHelper::elementSize(TYPE_FLOAT),
				BufferElementInfo(true),
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
				BufferElementInfo(true),
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
	LightSourcesLightingFeature lslf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature;
	assert(
			( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )) ;

	if(( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT))
	{
		assert("only one lightsource allowed by global shading features;"
				&& (mLightSources.size()==0));
	}

	assert("maximum of lightsources not reached" &&
			(getNumTotalLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources) );

	if(castsShadows)
	{
		LightSourcesShadowFeature lssf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature;
		assert( "pointlight as shadowcaster allowed" && (lssf = LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT));
		assert( "for point lights, only one shadow caster is valid" && (getNumTotalShadowingLightSources() ==0));
	}

	std::stringstream s; s<< mLightSources.size();

	mLightSources.push_back(
		new PointLight(
				String("PointLight")+ s.str(),
				castsShadows,
				true,
				LightSourceShaderStruct(
					position,diffuseColor,specularColor,
					//some ought-to be unused (besides their function as indicator that ist NOT a spot light)
					//default values
					Vector3D(0.0f,0.0f,-1.0f),0.0f,0.0f,0.0f,0.0f)
		)
	);

	return reinterpret_cast<PointLight*> (mLightSources.back());
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
	LightSourcesLightingFeature lslf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature;
	assert(
			( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT)
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )) ;

	if(( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT))
	{
		assert("only one lightsource allowed by global shading features;"
				&& (mLightSources.size()==0));
	}

	assert("maximum of lightsources not reached" &&
			(getNumTotalLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources) );

	if(castsShadows)
	{
		LightSourcesShadowFeature lssf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature;
		assert( "spotlight as shadowcaster allowed" &&
			(	(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT) ||
				(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)	 )
		);
		if(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
		{
			assert( "only one shadow caster is valid" && (getNumTotalShadowingLightSources() ==0));
		}
		else
		{
			assert(getNumTotalShadowingLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxShadowCasters);
		}

	}

	std::stringstream s; s<< mLightSources.size();

	mLightSources.push_back(
		new SpotLight(
				String("SpotLight")+ s.str(),
				castsShadows,
				true,
				LightSourceShaderStruct(
					position,diffuseColor,specularColor,
					direction,
					glm::radians(innerSpotCutOff_Degrees),
					glm::radians(outerSpotCutOff_Degrees),
					spotExponent,
					//the layer is not fixed but dependent on the number of currently active shadow casters
					//on a per-frame basis;
					//but again, non-initialized members make the coder shit into his pants ;)
					static_cast<float>(getNumTotalShadowingLightSources())
				)
		)
	);

	return reinterpret_cast<SpotLight*> (mLightSources.back());
}

int LightSourceManager::getNumCurrentlyActiveLightingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if(ls->isEnabled()){cnt++;}
	}
	return cnt;
}

int LightSourceManager::getNumCurrentlyActiveShadowingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if(ls->isEnabled() && ls->castsShadows() ){cnt++;}
	}
	return cnt;
}

int LightSourceManager::getNumTotalShadowingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if( ls->castsShadows() ){cnt++;}
	}
	return cnt;
}



//fill buffers with recent values
void LightSourceManager::updateLightSourcesUniformBuffer(Camera *mainCam)
{
	if(mLightSourcesUniformBuffer)
	{
//		unsigned int numTotalFloatValuesInBuffer =
//				ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources *
//				//number of floats inside a LightSourceShaderStruct
//				sizeof(LightSourceShaderStruct) / BufferHelper::elementSize(TYPE_FLOAT);

		unsigned int numFloatsPerLightSource = sizeof(LightSourceShaderStruct) / BufferHelper::elementSize(TYPE_FLOAT);


		unsigned int currentLightSourceUniformBufferIndex=0;
		unsigned int currentFloatOffset=0;
		//unsigned int lightSourceMemoryFootprint = sizeof(LightSourceShaderStruct);

		//assuming that there is a CPU component ;(

		//NOTE: maybe on coud just acces the std::vector data and transfer it to the gpu;
		//but i'm concerne about stuff like alignment, this-pointer and other c++-meta data
		//which could corrupt a tigtly-packed assumption;
		//so, at least for the beginning, let's fill the buffer float-by-float

		float* bufferToFill = reinterpret_cast<float*>(mLightSourcesUniformBuffer->getCPUBufferHandle());

		for(unsigned int currentLightSourceHostIndex =0; currentLightSourceHostIndex < mLightSources.size(); currentLightSourceHostIndex++ )
		{

			if(mLightSources[currentLightSourceHostIndex]->isEnabled())
			{
				currentFloatOffset = 0;
				const LightSourceShaderStruct lsss = mLightSources[currentLightSourceHostIndex]->getdata();
#define CURRENT_FLOAT_VALUE	bufferToFill[currentLightSourceUniformBufferIndex * numFloatsPerLightSource + currentFloatOffset++]

			    Vector4D lightPosViewSpace =
			    		mainCam->getGlobalTransform().getLookAtMatrix()
			    		* Vector4D( mLightSources[currentLightSourceHostIndex]->getGlobalTransform().getPosition(), 1.0f);
			    		//* Vector4D(lsss.position, 1.0f);
			    Vector4D lightDirViewSpace =
			    		mainCam->getGlobalTransform().getLookAtMatrix()
			    		* Vector4D( mLightSources[currentLightSourceHostIndex]->getGlobalTransform().getDirection(), 1.0f);
			    		//* Vector4D(lsss.direction, 0.0f);


				CURRENT_FLOAT_VALUE   =  lightPosViewSpace.x;
				CURRENT_FLOAT_VALUE   =  lightPosViewSpace.y;
				CURRENT_FLOAT_VALUE   =  lightPosViewSpace.z;

				CURRENT_FLOAT_VALUE   =  lsss.diffuseColor.x;
				CURRENT_FLOAT_VALUE   =  lsss.diffuseColor.y;
				CURRENT_FLOAT_VALUE   =  lsss.diffuseColor.z;

				CURRENT_FLOAT_VALUE   =  lsss.specularColor.x;
				CURRENT_FLOAT_VALUE   =  lsss.specularColor.y;
				CURRENT_FLOAT_VALUE   =  lsss.specularColor.z;

				CURRENT_FLOAT_VALUE   =  lightDirViewSpace.x;
				CURRENT_FLOAT_VALUE   =  lightDirViewSpace.y;
				CURRENT_FLOAT_VALUE   =  lightDirViewSpace.z;

				CURRENT_FLOAT_VALUE   =  lsss.innerSpotCutOff_Radians;
				CURRENT_FLOAT_VALUE   =  lsss.outerSpotCutOff_Radians;
				CURRENT_FLOAT_VALUE   =  lsss.spotExponent;

				CURRENT_FLOAT_VALUE   =  lsss.shadowMapLayer;

#undef CURRENT_FLOAT_VALUE
				currentLightSourceUniformBufferIndex++;
			}
		} //endfor

		mLightSourcesUniformBuffer->copyFromHostToGPU();

	}
}

void LightSourceManager::updateShadowMapMatricesUniformBuffer(Camera *mainCam)
{
	return;
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
