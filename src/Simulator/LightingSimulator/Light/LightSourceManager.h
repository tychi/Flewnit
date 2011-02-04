
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


namespace Flewnit
{


class LightSourceManager
: 	public Singleton<LightSourceManager>,
	public BasicObject
{
public:
	LightSourceManager(
			LightSourcesLightingFeature lightSourcesLightingFeature,
			LightSourcesShadowFeature lightSourcesShadowFeature,
			int maxLightSources);

	virtual ~LightSourceManager();




private:
	//friend class LightSource;


	///\{
	int mNumMaxLightingLightSources;
	Buffer* mLightSourceUniformBuffer;
	///\}

	///\{
	LightSourcesLightingFeature mLightSourcesLightingFeature;
	LightSourcesShadowFeature mLightSourcesShadowFeature;
	int mMaxLightSources;

	/*
	 * Only used if mLightSourcesShadowFeature == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS;
	 * In shadowmap generation passes, it contains the viewProjectionMatrices of the
	 * "lightsource cameras" for the geometry shader, in lightin passes, it constains
	 *  the biased viewProjectionMatrices;
	 */
	Buffer* mShadowMapMatricesUniformBuffer;

	//Texture2DDepth, Texture2DDepthArray or Texture2DDepthCube,
	//depending on the shadow feature
	Texture* mShadowMapDepthTexture;

	RenderTarget* mShadowMapRenderTarget;
	Shader* mShadowMapGenerationShader;
	///\}

};

}


