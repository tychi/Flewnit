/*
 * ShaderFeatureRelevantEnums.h
 *
 *  Created on: Feb 9, 2011
 *      Author: tychi
 *
 *  Enums outsourced from SimulatorMetaInfo.h because we need the definitions in the
 *  GrantleeShaderFeaturesContext class derived from qt;
 * 	I don't want qt code spread through my project, so to reduce the amount of
 *  source files having to be mata-object-compiled, I refuse to declare those
 *  enums globally in a QT-context. Hence, I outsource the the enums,
 *  so that they can be used both globally qt-free as locally in a qt/grantlee-context,
 *  and synchronization will be alway guaranteed;
 *
 */

#pragma once


//#ifndef FLEWNIT_NOT_INCLUDED_FOR_QT_META_OBJECT
//#include <QtCore/qvariant.h>
//#include <QtCore/qobject.h>
//
//	class ShaderFeatureEnumQTClass : public QObject
//	{
//		Q_OBJECT
//
//		Q_ENUMS(RenderingTechnique)
//		Q_ENUMS(TextureType)
//		Q_ENUMS(VisualMaterialType)
//		Q_ENUMS(ShadingFeatures)
//		Q_ENUMS(LightSourcesLightingFeature)
//		Q_ENUMS(LightSourcesShadowFeature)
//		Q_ENUMS(ShadowTechnique)
//
//	public:
//#endif
namespace Flewnit
{

	//is mirrored as defines in the shaders; don't let them get out of sync
	//TODO in far future: wirte script to generate shaderdefs and C++-enums consitstently
	enum TextureType
	{
		TEXTURE_TYPE_1D						=0,
		TEXTURE_TYPE_1D_ARRAY				=1,

		TEXTURE_TYPE_2D						=2,
		TEXTURE_TYPE_2D_RECT				=3,
		TEXTURE_TYPE_2D_CUBE				=4,
		TEXTURE_TYPE_2D_ARRAY				=5,
		TEXTURE_TYPE_2D_MULTISAMPLE			=6,
		TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE 	=7,

		TEXTURE_TYPE_2D_DEPTH				=8,
		TEXTURE_TYPE_2D_RECT_DEPTH			=9,
		TEXTURE_TYPE_2D_CUBE_DEPTH			=10,
		TEXTURE_TYPE_2D_ARRAY_DEPTH			=11,
		TEXTURE_TYPE_2D_MULTISAMPLE_DEPTH 		=12, //not supported yet
		TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE_DEPTH 	=13, //not supported yet

		TEXTURE_TYPE_3D						=14,
		__NUM_TEXTURE_TYPES__				=15
	};


	enum RenderingTechnique
	{
		/*
			render depth values to a depth texture, attached to the framebuffer's
			depth attachment:
			- if ShaderFeatures.lightSourcesShadowFeature==
						LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT:
				just a vertex shader, rendering implicitly only the "1/z" gl_FragDepth value
				to 2d-depth texture, no color stuff

			- if ShaderFeatures.lightSourcesShadowFeature==
						LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT:
				vertex + geometry + fragment shader, rendering the z-value in Camera Coordinates
				into a cubic depth texture by writing explicitly to gl_FragDepth;
				The geometry shader generates a primitive for every cubemap face;
			if ShaderFeatures.lightSourcesShadowFeature==
						LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS:
				vertex +geometry shader, rendering implicitly only the "1/z" gl_FragDepth value
				to a 2d-depth texture array, no color stuff.
				The geometry shader generates a primitive for every array layer;
		*/
		RENDERING_TECHNIQUE_SHADOWMAP_GENERATION		=0,
		//same as RENDERING_TECHNIQUE_CAMERA_SPACE_DEPTH_IMAGE_GENERATION, except we don't render
		//just the camera space z-value to gl_FragDepth, but the whole
		//camera space vec4 to a color texture
		RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION 	=1,


		//setup: same as RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION,
		//but as memory-footprint-optimization, we write only the z-value
		//to a one-component (GL_RED) texture; the other values will be reconstructed
		//from view frustum and frag coord when needed.
		RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION		=2,


		RENDERING_TECHNIQUE_DEFAULT_LIGHTING			=3,
		RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING	=4,
		RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL		=5,
		RENDERING_TECHNIQUE_DEFERRED_LIGHTING			=6,
		//value to indicate a special lighting stage, involving only
		//a few objects (like fluid objects), with special shader not
		//fitting the automatically-generated structure; materials with a custom-tag
		//will be ignored by the management logic of the ShaderManager;
		//The special stage and the special material with the special shader
		//will interact in their own way;
		RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION	=7,
		RENDERING_TECHNIQUE_CUSTOM						=8,
		__NUM_RENDERING_TECHNIQUES__					=9
	};

	enum LightSourcesLightingFeature
	{
		LIGHT_SOURCES_LIGHTING_FEATURE_NONE						=0,
		LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT			=1,
		LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT			=2,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS			=3,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS			=4,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS	=5,
		__NUM_LIGHT_SOURCES_LIGHTING_FEATURES__					=6
	};

	enum LightSourcesShadowFeature
	{
		LIGHT_SOURCES_SHADOW_FEATURE_NONE			=0,
		LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT	=1,
		LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT	=2,
		LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	=3,
		__NUM_LIGHT_SOURCES_SHADOW_FEATURES__		=4
	};

	enum ShadowTechnique
	{
		SHADOW_TECHNIQUE_NONE		=0,
		SHADOW_TECHNIQUE_DEFAULT	=1,
		SHADOW_TECHNIQUE_PCFSS		=2,
		__NUM_SHADOW_TECHNIQUES__	=3
	};

	enum ShadingFeatures
	{
		SHADING_FEATURE_NONE				=1<<0,
		SHADING_FEATURE_DIRECT_LIGHTING		=1<<1,
		//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented
		SHADING_FEATURE_GLOBAL_LIGHTING		=1<<2,
		SHADING_FEATURE_DECAL_TEXTURING		=1<<3,
		SHADING_FEATURE_DETAIL_TEXTURING	=1<<4,
		SHADING_FEATURE_NORMAL_MAPPING		=1<<5,
		SHADING_FEATURE_CUBE_MAPPING		=1<<6,
		SHADING_FEATURE_AMBIENT_OCCLUSION	=1<<7,
		SHADING_FEATURE_TESSELATION			=1<<8,
		__NUM_SHADING_FEATURES__			=9
	};


	//rough categorization of materials:
	//these are distinct visual rendering techniques which
	//use different shader generation templates;
	enum VisualMaterialType
	{
		VISUAL_MATERIAL_TYPE_NONE					=0,
		VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING  		=1,
		VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING		=2,
		VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY		=3,	//just set a color value or something

		VISUAL_MATERIAL_TYPE_GAS_RENDERING			=4,
		VISUAL_MATERIAL_TYPE_LIQUID_RENDERING		=5,
		__NUM_VISUAL_MATERIAL_TYPES__				=6
	};


	//------------------------------------------------------------------------------

	const String TextureTypeStrings[] =
	{
		"TEXTURE_TYPE_1D",
		"TEXTURE_TYPE_1D_ARRAY",

		"TEXTURE_TYPE_2D",
		"TEXTURE_TYPE_2D_RECT",
		"TEXTURE_TYPE_2D_CUBE",
		"TEXTURE_TYPE_2D_ARRAY",
		"TEXTURE_TYPE_2D_MULTISAMPLE",
		"TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE",

		"TEXTURE_TYPE_2D_DEPTH",
		"TEXTURE_TYPE_2D_RECT_DEPTH",
		"TEXTURE_TYPE_2D_CUBE_DEPTH",
		"TEXTURE_TYPE_2D_ARRAY_DEPTH",
		"TEXTURE_TYPE_2D_MULTISAMPLE_DEPTH",
		"TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE_DEPTH",

		"TEXTURE_TYPE_3D",
		"__NUM_TEXTURE_TYPES__"
	};


	const String  RenderingTechniqueStrings[] =
	{
		"RENDERING_TECHNIQUE_SHADOWMAP_GENERATION",
		"RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION",
		"RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION",
		"RENDERING_TECHNIQUE_DEFAULT_LIGHTING",
		"RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING",
		"RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL",
		"RENDERING_TECHNIQUE_DEFERRED_LIGHTING",
		"RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION",
		"RENDERING_TECHNIQUE_CUSTOM",
		"__NUM_RENDERING_TECHNIQUES__"
	};

	const String  LightSourcesLightingFeatureStrings[] =
	{
		"LIGHT_SOURCES_LIGHTING_FEATURE_NONE",
		"LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT",
		"LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT",
		"LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS",
		"LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS",
		"LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS",
		"__NUM_LIGHT_SOURCES_LIGHTING_FEATURES__"
	};

	const String LightSourcesShadowFeatureStrings[] =
	{
		"LIGHT_SOURCES_SHADOW_FEATURE_NONE",
		"LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT",
		"LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT",
		"LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS",
		"__NUM_LIGHT_SOURCES_SHADOW_FEATURES__"
	};

	const String  ShadowTechniqueStrings[] =
	{
		"SHADOW_TECHNIQUE_NONE",
		"SHADOW_TECHNIQUE_DEFAULT",
		"SHADOW_TECHNIQUE_PCFSS",
		"__NUM_SHADOW_TECHNIQUES__"
	};

	const String  ShadingFeatureStrings[] =
	{
		"SHADING_FEATURE_NONE",
		"SHADING_FEATURE_DIRECT_LIGHTING",
		"SHADING_FEATURE_GLOBAL_LIGHTING",
		"SHADING_FEATURE_DECAL_TEXTURING",
		"SHADING_FEATURE_DETAIL_TEXTURING",
		"SHADING_FEATURE_NORMAL_MAPPING",
		"SHADING_FEATURE_CUBE_MAPPING",
		"SHADING_FEATURE_AMBIENT_OCCLUSION",
		"SHADING_FEATURE_TESSELATION",
		"__NUM_SHADING_FEATURES__"
	};


	//rough categorization of materials:
	//these are distinct visual rendering techniques which
	//use different shader generation templates;
	const String VisualMaterialTypeStrings[] =
	{
		"VISUAL_MATERIAL_TYPE_NONE",
		"VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING",
		"VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING",
		"VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY",
		"VISUAL_MATERIAL_TYPE_GAS_RENDERING",
		"VISUAL_MATERIAL_TYPE_LIQUID_RENDERING",
		"__NUM_VISUAL_MATERIAL_TYPES__"
	};


//#ifndef FLEWNIT_NOT_INCLUDED_FOR_QT_META_OBJECT
//	};
//	 Q_DECLARE_METATYPE(ShaderFeatureEnumQTClass*)
//#endif

}
