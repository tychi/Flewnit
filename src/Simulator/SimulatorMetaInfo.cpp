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
		VisualMaterialType visualMaterialType,
		ShadingFeatures shadingFeatures,
		bool instancedRendering
)
:renderingTechnique(renderingTechnique),renderTargetTextureType(renderTargetTextureType),
 visualMaterialType(visualMaterialType),
 shadingFeatures(shadingFeatures),instancedRendering(instancedRendering)
{
}

ShaderFeaturesLocal::ShaderFeaturesLocal(const ShaderFeaturesLocal& rhs)
{
	(*this) = rhs;
}

//bool ShaderFeaturesLocal::isSharable()const
//{
//	return (renderingTechnique == RENDERING_TECHNIQUE_CUSTOM);
//}

bool operator==(ShaderFeaturesLocal const& lhs, ShaderFeaturesLocal const& rhs)
//bool ShaderFeaturesLocal::operator==(const ShaderFeaturesLocal& rhs)
{
	return
			lhs.renderingTechnique == rhs.renderingTechnique &&
			lhs.renderTargetTextureType == rhs.renderTargetTextureType &&
			lhs.visualMaterialType == rhs.visualMaterialType &&
			lhs.shadingFeatures == rhs.shadingFeatures &&
			lhs.instancedRendering == rhs.instancedRendering;
}

const ShaderFeaturesLocal& ShaderFeaturesLocal::operator=(const ShaderFeaturesLocal& rhs)
{
	this->renderingTechnique = rhs.renderingTechnique;
	this->renderTargetTextureType = rhs.renderTargetTextureType;
	this->visualMaterialType = rhs.visualMaterialType;
	this->shadingFeatures = rhs.shadingFeatures;
	this->instancedRendering = rhs.instancedRendering;

	return *this;
}

String ShaderFeaturesLocal::stringify()const
{
	String returnString = "";

	switch (renderingTechnique) {
		case RENDERING_TECHNIQUE_SHADOWMAP_GENERATION:
			returnString.append("_SMGen");
			break;
		case RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION:
			returnString.append("_PosImgGen");
			break;
		case RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION:
			returnString.append("_DepthImgGen");
			break;
		case RENDERING_TECHNIQUE_DEFAULT_LIGHTING:
			returnString.append("_DefaultLighting");
			break;
		case RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING:
			returnString.append("_TransparentLighting");
			break;
		case RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL:
			returnString.append("_GBufferFill");
			break;
		case RENDERING_TECHNIQUE_DEFERRED_LIGHTING:
			returnString.append("_DeferredLighting");
			break;
		case RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION:
			returnString.append("_PrimIDRasterize");
			break;
		case RENDERING_TECHNIQUE_CUSTOM:
			returnString.append("_CustomRenderingTechnique");
			break;
		default:
			assert("need valid rendering technique" && 0);
			break;
	}


	switch (renderTargetTextureType) {
		case TEXTURE_TYPE_1D:
			returnString.append("_RT_1D");
			break;
		case TEXTURE_TYPE_1D_ARRAY:
			returnString.append("_RT_1Darray");
			break;
		case TEXTURE_TYPE_2D:
			returnString.append("_RT_2D");
			break;
		case TEXTURE_TYPE_2D_RECT:
			returnString.append("_RT_2DRect");
			break;
		case TEXTURE_TYPE_2D_CUBE:
			returnString.append("_RT_2DCube");
			break;
		case TEXTURE_TYPE_2D_ARRAY:
			returnString.append("_RT_2DArray");
			break;
		case TEXTURE_TYPE_2D_MULTISAMPLE:
			returnString.append("_RT_2DMS");
			break;
		case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
			returnString.append("_RT_2DMSArray");
			break;
		case TEXTURE_TYPE_2D_DEPTH:
			returnString.append("_RT_2DDepth");
			break;
		case TEXTURE_TYPE_2D_RECT_DEPTH:
			returnString.append("_RT_2DRectDepth");
			break;
		case TEXTURE_TYPE_2D_CUBE_DEPTH:
			returnString.append("_RT_2DCubeDepth");
			break;
		case TEXTURE_TYPE_2D_ARRAY_DEPTH:
			returnString.append("_RT_2DArrayDepth");
			break;
		case TEXTURE_TYPE_2D_MULTISAMPLE_DEPTH:
			returnString.append("_RT_2DMSDepth");
			break;
		case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE_DEPTH:
			returnString.append("_RT_2DArrayMSDepth");
			break;
		case TEXTURE_TYPE_3D:
			returnString.append("_RT_3D");
			break;
		default:
			assert("need valid render target textures type" && 0);
			break;
	}


	switch (visualMaterialType) {
		case VISUAL_MATERIAL_TYPE_NONE:
			returnString.append("_MatTypeNone");
			break;
		case VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING:
			returnString.append("_MatDefaultLighting");
			break;
		case VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING:
			returnString.append("_MatTypeSkyDome");
			break;
//		case VISUAL_MATERIAL_TYPE_PRIMITIVE_ID_RENDERING:
//			returnString.append("_MatPrimID");
//			break;
		case VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY:
			returnString.append("_MatTypePureDebugDraw");
			break;
		case VISUAL_MATERIAL_TYPE_GAS_RENDERING:
			returnString.append("_MatTypeGas");
			break;
		case VISUAL_MATERIAL_TYPE_LIQUID_RENDERING:
			returnString.append("_MatTypeLiquid");
			break;
//		case VISUAL_MATERIAL_TYPE_INSTANCED:
//			returnString.append("_MatInstanced");
//			break;
		default:
			assert("need valid material type" && 0);
			break;
	}


	for(uint  currentShadingFeatureBitPosition = 0;
			currentShadingFeatureBitPosition < __NUM_SHADING_FEATURES__;
			currentShadingFeatureBitPosition ++)
	{
		ShadingFeatures currentShadingFeatureToTest =
				ShadingFeatures(
						shadingFeatures &
						(ShadingFeatures(1<<currentShadingFeatureBitPosition))
				);
		switch (currentShadingFeatureToTest) {
			case 0: break; // nothing active in current run
			case SHADING_FEATURE_NONE:
				returnString.append("_ShadeNone");
				break;
			case SHADING_FEATURE_DIRECT_LIGHTING:
				returnString.append("_ShadeDirectLighting");
				break;
			case SHADING_FEATURE_GLOBAL_LIGHTING:
				returnString.append("_ShadeGlobalLighting");
				break;
			case SHADING_FEATURE_DIFFUSE_TEXTURING:
				returnString.append("_ShadeDiffuse");
				break;
			case SHADING_FEATURE_DETAIL_TEXTURING:
				returnString.append("_ShadeDetail");
				break;
			case SHADING_FEATURE_NORMAL_MAPPING:
				returnString.append("_ShadeNormalMapping");
				break;
			case SHADING_FEATURE_CUBE_MAPPING:
				returnString.append("_ShadeCubeMapping");
				break;
			case SHADING_FEATURE_AMBIENT_OCCLUSION:
				returnString.append("_ShadeAO");
				break;
			case SHADING_FEATURE_TESSELATION:
				returnString.append("_ShadeTesselation");
				break;
			default:
				assert("need valid shading feature type" && 0);
				break;
		}
	}

	if(instancedRendering)
	{
		returnString.append("_instanced");
	}

	return returnString;


}

std::size_t hash_value(ShaderFeaturesLocal const& sfl)
   {
      boost::hash<uint> hasher;

       //generating unique integer for every possible permutaion of values;
//       int integerizedSFLvalue =
//    		   //bit 31..24 (8bits) for number in [0..7]=3 bits --> fits
//    		   ( static_cast<int>(sfl.renderingTechnique) <<24 ) |
//    		   //bit 23..16 (8bits) for number in [0..14]=4 bits --> fits
//    		   ( static_cast<int>(sfl.renderTargetTextureType) <<16 ) |
//    		   //bit 15..1 (15 bits) for 7-bit bitfield --> fits
//    		   ( static_cast<int>(sfl.shadingFeatures) << 1 ) |
//    		   //bit 0 for integer value (1 bit) --> fits
//    		   (sfl.instancedRendering? 1: 0)
//    		   ;

//       int integerizedSFLvalue =
//    		   //bit 31..28 (4bits) for number in [0..8]=4 bits --> fits
//    		   ( static_cast<int>(sfl.renderingTechnique) <<28 ) |
//    		   //bit 27..24 (4bits) for number in [0..14]=4 bits --> fits
//    		   ( static_cast<int>(sfl.renderTargetTextureType) <<24 ) |
//    		   //bit 23..20 (4bits) for number in [0..7]=3 bits --> fits
//    		   ( static_cast<int>(sfl.visualMaterialType) <<20 ) |
//    		   //bit 19..10 (10 bits) for 9-bit bitfield --> fits
//    		   ( static_cast<int>(sfl.shadingFeatures) << 10 ) |
//    		   //bit  9.. 0 (10 bits) for  max  val 1023 integer value (10 bit) --> fits
//    		   (sfl.instancedRendering? 1: 0)
//    		   ;

       uint integerizedSFLvalue =
    		   //bit 21..18 (4bits) for number in [0..8]=4 bits --> fits
    		   ( static_cast<uint>(sfl.renderingTechnique) <<18 ) |
    		   //bit 17..14 (4bits) for number in [0..14]=4 bits --> fits
    		   ( static_cast<uint>(sfl.renderTargetTextureType) <<14 ) |
    		   //bit 13..10 (4bits) for number in [0..7]=3 bits --> fits
    		   ( static_cast<uint>(sfl.visualMaterialType) <<10 ) |
    		   //bit 09..01 (9 bits) for 9-bit bit field --> fits
    		   ( static_cast<uint>(sfl.shadingFeatures) << 1 ) |
    		   //bit  0.. 0 (1 bit) for instanced- flag (1 bit) --> fits
    		   (sfl.instancedRendering? 1: 0)
    		   ;

       //return hasher(integerizedSFLvalue);
       return static_cast<size_t>( integerizedSFLvalue );
   }




ShaderFeaturesGlobal::ShaderFeaturesGlobal(
		LightSourcesLightingFeature lightSourcesLightingFeature ,
		LightSourcesShadowFeature lightSourcesShadowFeature,
		ShadowTechnique shadowTechnique,
		int numMaxLightSources ,
		int numMaxShadowCasters,
		int numMaxInstancesRenderable,

		TextureType GBufferType,
		int numMultiSamples
)
:
	lightSourcesLightingFeature(lightSourcesLightingFeature),
	lightSourcesShadowFeature (lightSourcesShadowFeature),
	shadowTechnique(shadowTechnique),
	numMaxLightSources(numMaxLightSources),
	numMaxShadowCasters(numMaxShadowCasters),
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
			numMaxShadowCasters = rhs.numMaxShadowCasters;

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
			numMaxShadowCasters == rhs.numMaxShadowCasters &&
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

void ShaderFeaturesGlobal::validate()throw(SimulatorException)
{
	assert(numMaxShadowCasters <= numMaxLightSources);

	if(
		(lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT )
	 ||	(lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT )
	)
	{
		numMaxLightSources = 1;
		numMaxShadowCasters = 1;
		assert("shadow feature may not involve more lightsources than the lighting feature" &&
			(lightSourcesShadowFeature != LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS	)
		);
	}
	else
	{
		if(lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_NONE )
		{
			assert(lightSourcesShadowFeature == LIGHT_SOURCES_SHADOW_FEATURE_NONE);
			numMaxLightSources = 0;
			numMaxShadowCasters = 0;
		}
		else
		{
			//keep init-values as we have many lightsources
		}



		//further validation comes later, if necessary
	}
}


}
