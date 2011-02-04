/*
 * SimulatorMetaInfo.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SimulatorMetaInfo.h"

namespace Flewnit
{


ShaderFeatures::ShaderFeatures(
		RenderingTechnique renderingTechnique,
		TextureType renderTargetTextureType,
		ShadingFeature shadingFeature,
		LightSourcesLightingFeature lightSourcesLightingFeature,
		LightSourcesShadowFeature lightSourcesShadowFeature ,
		ShadowTechnique shadowTechnique,
		int numMaxLightSources,
		int numMultiSamples,
		int numMaxInstancesRenderable,
		TextureType GBufferType,
		bool renderIndices
)
:
	renderingTechnique(renderingTechnique),
	renderTargetTextureType(renderTargetTextureType),
	shadingFeature(shadingFeature),
	lightSourcesLightingFeature(lightSourcesLightingFeature),
	lightSourcesShadowFeature (lightSourcesShadowFeature),
	shadowTechnique(shadowTechnique),
	numMaxLightSources(numMaxLightSources),
	numMultiSamples(numMultiSamples),
	numMaxInstancesRenderable(numMaxInstancesRenderable),
	GBufferType(GBufferType),
	renderIndices(renderIndices)
{

}

ShaderFeatures::ShaderFeatures(const ShaderFeatures& rhs)
{
	(*this) = rhs;
}

const ShaderFeatures& ShaderFeatures::operator=(const ShaderFeatures& rhs)
{
			renderingTechnique = rhs.renderingTechnique;
			renderTargetTextureType = rhs.renderTargetTextureType;
			shadingFeature = rhs.shadingFeature;
			lightSourcesLightingFeature = rhs.lightSourcesLightingFeature;
			lightSourcesShadowFeature  = rhs.lightSourcesShadowFeature;
			shadowTechnique = rhs.shadowTechnique;
			numMaxLightSources = rhs.numMaxLightSources;
			numMultiSamples = rhs.numMultiSamples;
			numMaxInstancesRenderable = rhs.numMaxInstancesRenderable;
			GBufferType = rhs.GBufferType;
			renderIndices = rhs.renderIndices;

	return *this;
}

//return false if some stuff on one side has "custom"...
bool ShaderFeatures::operator==(const ShaderFeatures& rhs)
{
	bool dataIsEqual =
			renderingTechnique == rhs.renderingTechnique &&
			renderTargetTextureType == rhs.renderTargetTextureType &&
			shadingFeature == rhs.shadingFeature &&
			lightSourcesLightingFeature == rhs.lightSourcesLightingFeature &&
			lightSourcesShadowFeature  == rhs.lightSourcesShadowFeature &&
			shadowTechnique == rhs.shadowTechnique &&
			numMaxLightSources == rhs.numMaxLightSources &&
			numMultiSamples == rhs.numMultiSamples &&
			numMaxInstancesRenderable == rhs.numMaxInstancesRenderable &&
			GBufferType == rhs.GBufferType &&
			renderIndices == rhs.renderIndices;

	if(! dataIsEqual) return false;

	//indicator for non-sharable shader
	if(renderingTechnique == RENDERING_TECHNIQUE_CUSTOM)
	{
		return false;
	}

	return true;


}




}
