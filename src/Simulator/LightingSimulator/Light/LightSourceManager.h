
/*
 * LightSourceManager.h
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 *
 * Maintainer of all LightSource instances
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
	LightSourceManager(int maxLightingLightSources, int NumMaxShadowMaps = 1 );
	virtual ~LightSourceManager();


private:
	//friend class LightSource;


	///\{
	int mNumMaxLightingLightSources;
	Buffer* mLightSourceUniformBuffer;
	///\}

	///\{
	int mNumMaxShadowMaps;
	Buffer* mShadowMapMatricesUniformBuffer;
	//Texture2DDepth, Texture2DDepthArray or Texture2DDepthCube
	Texture* mShadowMapTexture;
	///\}

	ShaderFeatures mShaderFeatures;

};

}


