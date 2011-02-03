/*
 * SimulatorMetaInfos.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorForwards.h"

#include <typeinfo>



namespace Flewnit
{


class SimulatorException : public std::exception
{
	String mDescription;
 public:
	SimulatorException(String description = "unspecified simulator error") throw()
	: mDescription(description)
	{ }

	virtual ~SimulatorException() throw(){}

	virtual const char* what() const throw()
	{
	    return mDescription.c_str();
	}
};


//class SimulatorMetaInfos
//: public BasicObject
//{
//	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
//public:
//	SimulatorMetaInfos();
//	virtual ~SimulatorMetaInfos();
//};





enum SimulationDomain
{
	MECHANICAL_SIM_DOMAIN 	= 0,
	VISUAL_SIM_DOMAIN 		= 1,
	ACUSTIC_SIM_DOMAIN		= 2,
	__NUM_SIM_DOMAINS__		= 3,
	//useful for stuff like vertex based geometry, which can be shared in several domains
	GENERIC_SIM_DOMAIN		= 4,
	__NO_SIM_DOMAIN__		= 5
};

//ID flags for Simulators and their pipeline stages
enum SimulationKindFlags
{
	FLUID_SIMULATION		= 1<<0,
	RIGID_BODY_SIMULATION	= 1<<1,
	SOFT_BODY_SIMULATION	= 1<<2,
	CLOTH_SIMULATION		= 1<<3,
	HAIR_SIMULATION			= 1<<4,

	LIGHTING_SIMULATION		= 1<<5,
	SOUND_SIMULATION		= 1<<6
};

//ID flags for scene nodes and theri derived world objects;
enum SceneNodeTypeFlags
{
	PURE_NODE 			=	0,


	FLUID_OBJECT 		= 1<<0,	//particles or voxels
	RIGID_BODY 			= 1<<1,	//particles or triangles or voxelized or primitive based..
								//doesn't matter during thesis but the first representation
	SOFT_BODY_OBJECT	= 1<<2,	//triangles and maybe many more .. doesn't matter during thesis
	CLOTH_OBJECT	 	= 1<<3,	//triangle strips and maybe many more .. doesn't matter during thesis
	HAIR_OBJECT 		= 1<<4, //line strips and maybe many more .. doesn't matter during thesis

	STATIC_OBJECT		= 1<<5,	//triangles with special access pattern and several index buffers

	VISUAL_OBJECT 		= 1<<6,	//everything geom. rep. imaginable
	SOUND_OBJECT 		= 1<<7,	//no geometric representation needed

	CAMERA_NODE			= 1<<8,	//no geometric representation needed
	LIGHT_NODE			= 1<<9, //no geometric representation needed

	//for debug drawing, we'll treat structures like uniform grids, kd trees etc. like WolrdObjects
	SPATIAL_DATA_STRUCTURE_OBJECT = 1<<9	//implicit representation, debug drawn via instancing

};



///\{


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
	//value to indicate a special shader; test for equality will fail;
	RENDERING_TECHNIQUE_CUSTOM						=7
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
	SHADING_FEATURE_NONE				=0,
	SHADING_FEATURE_DIRECT_LIGHTING		=1<<0,
	//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented
	SHADING_FEATURE_GLOBAL_LIGHTING		=1<<1,
	SHADING_FEATURE_DECAL_TEXTURING		=1<<2,
	SHADING_FEATURE_NORMAL_MAPPING		=1<<3,
	SHADING_FEATURE_CUBE_MAPPING		=1<<4,
	SHADING_FEATURE_AMBIENT_OCCLUSION	=1<<5,
	SHADING_FEATURE_TESSELATION			=1<<6,

};

enum TextureType
{
	TEXTURE_TYPE_2D_DEFAULT 			=0,
	TEXTURE_TYPE_2D_RECT				=1,
	TEXTURE_TYPE_2D_CUBE				=2,
	TEXTURE_TYPE_2D_ARRAY				=3,
	TEXTURE_TYPE_2D_MULTISAMPLE			=4,
	TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE 	=5,
	TEXTURE_TYPE_3D						=6
};


struct ShaderFeatures
{
	RenderingTechnique renderingTechnique;
	//renderTargetType delegates creation and/or configuration of a geometry shader,
	//defining layers for cubemap rendering or general layered rendering
	TextureType renderTargetType;
	ShadingFeature shadingFeature;
	LightSourcesLightingFeature lightSourcesLightingFeature;
	LightSourcesShadowFeature lightSourcesShadowFeature;
	ShadowTechnique shadowTechnique;

	//features not needed by every kind of shader:
	TextureType GBufferType;
	bool renderIndices;
	int numMaxInstancesRenderable;
	int numMaxLightSources;
	int numMultiSamples;
};


//rough categorization of materials:
//these are distinct visual rendering techniques which
//use different shader generation templates;
enum VisualMaterialType
{
	VISUAL_MATERIAL_TYPE_LIGHTING,
	VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING,
	VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY,

	VISUAL_MATERIAL_TYPE_GAS_RENDERING,
	VISUAL_MATERIAL_TYPE_LIQUID_RENDERING
};

///\}



//enum LightingSimulationStageType
//{
//
//};


}

