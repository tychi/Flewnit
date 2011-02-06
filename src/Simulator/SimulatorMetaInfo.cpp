/*
 * SimulatorMetaInfo.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SimulatorMetaInfo.h"

#include <boost/unordered_map.hpp>

namespace Flewnit
{

ShaderFeaturesLocal::ShaderFeaturesLocal(
		RenderingTechnique renderingTechnique,
		TextureType renderTargetTextureType,
		ShadingFeatures shadingFeatures,
		bool instancedRendering
)
:renderingTechnique(renderingTechnique),renderTargetTextureType(renderTargetTextureType),
 shadingFeatures(shadingFeatures),instancedRendering(instancedRendering)
{
}

ShaderFeaturesLocal::ShaderFeaturesLocal(const ShaderFeaturesLocal& rhs)
{
	(*this) = rhs;
}

bool ShaderFeaturesLocal::isSharable()const
{
	return (renderingTechnique == RENDERING_TECHNIQUE_CUSTOM);
}

bool operator==(ShaderFeaturesLocal const& lhs, ShaderFeaturesLocal const& rhs)
//bool ShaderFeaturesLocal::operator==(const ShaderFeaturesLocal& rhs)
{
	return
			lhs.renderingTechnique == rhs.renderingTechnique &&
			lhs.renderTargetTextureType == rhs.renderTargetTextureType &&
			lhs.shadingFeatures == rhs.shadingFeatures &&
			lhs.instancedRendering == rhs.instancedRendering;
}

const ShaderFeaturesLocal& ShaderFeaturesLocal::operator=(const ShaderFeaturesLocal& rhs)
{
	renderingTechnique = rhs.renderingTechnique;
	renderTargetTextureType = rhs.renderTargetTextureType;
	shadingFeatures = rhs.shadingFeatures;
	instancedRendering = rhs.instancedRendering;

	return *this;
}

std::size_t hash_value(ShaderFeaturesLocal const& sfl)
   {
       boost::hash<int> hasher;

       //generating unique integer for every possible permutaion of values;
       int integerizedSFLvalue =
    		   //bit 31..24 (8bits) for number in [0..7]=3 bits --> fits
    		   ( static_cast<int>(sfl.renderingTechnique) <<24 ) |
    		   //bit 23..16 (8bits) for number in [0..14]=4 bits --> fits
    		   ( static_cast<int>(sfl.renderTargetTextureType) <<16 ) |
    		   //bit 15..1 (15 bits) for 7-bit bitfield --> fits
    		   ( static_cast<int>(sfl.shadingFeatures) << 1 ) |
    		   //bit 0 for integer value (1 bit) --> fits
    		   (sfl.instancedRendering? 1: 0)
    		   ;

       return hasher(integerizedSFLvalue);
   }




ShaderFeaturesGlobal::ShaderFeaturesGlobal(
		LightSourcesLightingFeature lightSourcesLightingFeature ,
		LightSourcesShadowFeature lightSourcesShadowFeature,
		ShadowTechnique shadowTechnique,
		int numMaxLightSources ,

		int numMaxInstancesRenderable,

		TextureType GBufferType,
		int numMultiSamples
)
:
	lightSourcesLightingFeature(lightSourcesLightingFeature),
	lightSourcesShadowFeature (lightSourcesShadowFeature),
	shadowTechnique(shadowTechnique),
	numMaxLightSources(numMaxLightSources),

	numMaxInstancesRenderable(numMaxInstancesRenderable),

	GBufferType(GBufferType),
	numMultiSamples(numMultiSamples)
{

}

ShaderFeaturesGlobal::ShaderFeaturesGlobal(const ShaderFeaturesGlobal& rhs)
{
	(*this) = rhs;
}

const ShaderFeaturesGlobal& ShaderFeaturesGlobal::operator=(const ShaderFeaturesGlobal& rhs)
{
			lightSourcesLightingFeature = rhs.lightSourcesLightingFeature;
			lightSourcesShadowFeature  = rhs.lightSourcesShadowFeature;
			shadowTechnique = rhs.shadowTechnique;
			numMaxLightSources = rhs.numMaxLightSources;

			numMaxInstancesRenderable = rhs.numMaxInstancesRenderable;

			numMaxInstancesRenderable = rhs.numMaxInstancesRenderable;

			GBufferType = rhs.GBufferType;
			numMultiSamples = rhs.numMultiSamples;

	return *this;
}

//return false if some stuff on one side has "custom"...
bool ShaderFeaturesGlobal::operator==(const ShaderFeaturesGlobal& rhs)
{
	//bool dataIsEqual =
	return
			lightSourcesLightingFeature == rhs.lightSourcesLightingFeature &&
			lightSourcesShadowFeature  == rhs.lightSourcesShadowFeature &&
			shadowTechnique == rhs.shadowTechnique &&
			numMaxLightSources == rhs.numMaxLightSources &&

			numMaxInstancesRenderable == rhs.numMaxInstancesRenderable &&

			numMaxInstancesRenderable == rhs.numMaxInstancesRenderable &&

			GBufferType == rhs.GBufferType &&
			numMultiSamples == rhs.numMultiSamples;

//	if(! dataIsEqual) return false;
//
//	//indicator for non-sharable shader
//	if(renderingTechnique == RENDERING_TECHNIQUE_CUSTOM)
//	{
//		return false;
//	}
//
//	return true;


}




}
