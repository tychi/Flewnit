
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

#include "Simulator/SimulatorMetaInfos.h"


namespace Flewnit
{


enum RenderingTechnique
{
	RENDERING_TECHNIQUE_DIRECT					=0,
	RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL	=1,
	RENDERING_TECHNIQUE_DEFERRED_LIGHTING		=2
};

enum LightSourcesLightingFeature
{
	LIGHT_SOURCES_LIGHTING_FEATURE_NONE						=0,
	LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT			=1,
	LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT			=2,
	LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS			=3,
	LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS			=4,
	LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS	=5
};

enum LightSourcesShadowFeature
{
	LIGHT_SOURCES_SHADOW_FEATURE_NONE			=0,
	LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT	=1,
	LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT	=2,
	LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	=3
};

enum ShadowTechnique
{
	SHADOW_TECHNIQUE_NONE		=0,
	SHADOW_TECHNIQUE_DEFAULT	=1,
	SHADOW_TECHNIQUE_PCFSS		=2
};

enum ShadingFeature
{
	SHADER_LIGHTING_FEATURE_NONE				=0,
	SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING		=1<<0,
	//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented
	SHADER_LIGHTING_FEATURE_GLOBAL_LIGHTING		=1<<1,
	SHADER_LIGHTING_FEATURE_DECAL_TEXTURING		=1<<2,
	SHADER_LIGHTING_FEATURE_NORMAL_MAPPING		=1<<3,
	SHADER_LIGHTING_FEATURE_CUBE_MAPPING		=1<<4,
	SHADER_LIGHTING_FEATURE_AMBIENT_OCCLUSION	=1<<5
};

struct ShaderFeatures
{
	RenderingTechnique renderingTechnique;
	LightSourcesLightingFeature lightSourcesLightingFeature;
	LightSourcesShadowFeature lightSourcesShadowFeature;
	ShadowTechnique shadowTechnique;
	ShadingFeature shadingFeature;
};

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


