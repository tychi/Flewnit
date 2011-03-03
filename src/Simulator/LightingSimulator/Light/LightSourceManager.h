
/*
 * LightSourceManager.h
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 *
 * Maintainer of all LightSource instances. Manages
 * 	- the uniform buffers for the lightsource data (matrices, colors, positions, directions etc..)
 *  - the shadowmap texture (2D/Cube/Array)
 *  - relevant other data to be passed as uniforms to shaders
 *
 *  This class has the "ultimate" authority to dominate shader settings;
 *  Example: if the LightSourceManager is configured only for one pointlight,
 *  a shader must override contradicting shaderfeatures;  Only this way a constistent
 *  rendering can be achieved;
 *
 */


#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"
#include "Common/Math.h"

#include "LightSource.h"


namespace Flewnit
{


class LightSourceManager
: 	public Singleton<LightSourceManager>,
	public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//will configure itself according to config file in future; at the moment, there
	//are hard codes in the constructor ;(
	LightSourceManager();

	virtual ~LightSourceManager();

	//to be called by LightingSimulator::init();
	void init();

	//throws exception if mNumMaxLightSources lightsources already exists or if the
	//lighting feature contradicts pointlights;
	//if castsShadows contradicts the shadowing feature, a warning is issues,
	//and the compatible value is set, i.e. there is o guarantee that the user's
	//wish is fulfilled
	PointLight* createPointLight(
			const Vector4D& position,
			bool castsShadows,
			const Vector4D& diffuseColor = Vector4D(1.0f, 1.0f, 1.0f,1.0f),
			const Vector4D& specularColor = Vector4D(1.0f, 1.0f, 1.0f,1.0f)
	) throw(SimulatorException);

	//throws exception if mNumMaxLightSources lightsources already exists or if the
	//lighting feature contradicts pointlights;
	//if castsShadows contradicts the shadowing feature, a warning is issues,
	//and the compatible value is set, i.e. there is o guarantee that the user's
	//wish is fulfilled
	SpotLight* createSpotLight(
			const Vector4D& position,
			const Vector4D& direction,
			bool castsShadows,
			float innerSpotCutOff_Degrees = 30.0f,
			float outerSpotCutOff_Degrees= 45.0f,
			float spotExponent = 10.0f,
			const Vector4D& diffuseColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f),
			const Vector4D& specularColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f)
	) throw(SimulatorException);


	int getNumCurrentlyActiveLightingLightSources()const;
	int getNumCurrentlyActiveShadowingLightSources()const;
	int getNumTotalShadowingLightSources()const;
	inline int getNumTotalLightSources()const{return mLightSources.size();}

	inline LightSource* getLightSource(unsigned int index)const{assert(index<mLightSources.size()); return mLightSources[index];}
	//for single light source scenarios:
	LightSource* getFirstShadowCaster()const;


	inline Buffer* getShadowCameraTransformBuffer()const{return mShadowCameraTransformBuffer;}
	inline Buffer* getLightSourceUniformBuffer()const{return mLightSourcesUniformBuffer;}
	//inline Texture* getShadowMapDepthTexture()const{return mShadowMapDepthTexture;}
	//dictates the clip planes for all lightsource projection matrices;
	inline float getLightSourceProjectionMatrixNearClipPlane()const{return mLightSourceProjectionMatrixNearClipPlane;}
	inline float getLightSourceProjectionMatrixFarClipPlane()const{return mLightSourceProjectionMatrixFarClipPlane;}



	//fill buffers with recent values
	//{
	//if ShaderManager::currentRenderingScenarioNeedsWorldSpaceTransform(),
	//	then the light source pos and dir will be set in worldspace coords
	void updateLightSourcesUniformBuffer(Camera *cam);
	//cam param only needed for lookup in view space; if viewspace or not
	//is queried via ShaderManager::currentRenderingScenarioNeedsWorldSpaceTransform();
	//!lookup: shadowCamProjection * shadowCamView
	//lookup && worldspace: hadowCamBias * shadowCamProjection * shadowCamView
	//lookup && !worldspace: hadowCamBias * shadowCamProjection * shadowCamView * (camView)⁻1
	void setupShadowCamMatricesUniformBuffer(bool lookupMatrices, Camera* cam = 0);
	//}

private:
	friend LightSource::~LightSource();
	//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
	//has to remove the list-entry before actually call the destructor on the LS;
	//this has to be done to resolve the posession-concurrence between scengraph and LS manager
	//(as we cannot rely on the fact that every lightsource is IN the scenegraph,
	//AND we might also want to remove lightsources dynamically)
	void unregisterLightSource(LightSource* ls);

	///\{
	float mLightSourceProjectionMatrixNearClipPlane;
	float mLightSourceProjectionMatrixFarClipPlane;

	//int mNumCurrentActiveLightingLightSources;
	//int mNumCurrentActiveShadowingLightSources;
	std::vector<LightSource*> mLightSources;

	/*
	 * Only used if mLightSourcesLightingFeature corresponds to multiple lightsource shading;
	 * otherwise, the shaders are expected to set the single lightsource via "classic" uniforms,
	 *  as uniform buffers are expected to slower and hence only amortize when a big amount
	 *  of data is needed ;).
	 */
	Buffer* mLightSourcesUniformBuffer;
	UniformBufferMetaInfo* mLightSourceBufferMetaInfo;

	/*
	 * Only used if mLightSourcesShadowFeature == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS;
	 * In shadowmap generation passes, it contains the viewProjectionMatrices of the
	 * "lightsource cameras" for the geometry shader; in lighting passes, it contains
	 *  the biased viewProjectionMatrices for the fragment shader;
	 */
	Buffer* mShadowCameraTransformBuffer;
	UniformBufferMetaInfo* mShadowCameraTransformBufferMetaInfo_SMGeneration;
	UniformBufferMetaInfo* mShadowCameraTransformBufferMetaInfo_SMLookup;
	///\}



	GLint	mNumMaxLightSources; //store this value to be independent of ShaderManager deletion (critical when engine shuts down)

//legacy :
//	GLint** mLightSourceBufferOffsets;
//	GLint 	mRequiredLightSourceBufferSize;

//	//WARNING: 	keep this implementation in synch with the LightSourceShaderStruct definition!!1
//	//			I've found no way yet to automize synch in a realible way; And I'm doing all this
//	//			querying for reliability!
//	void queryLightSourceBufferSizeAndOffsets();


};

}


