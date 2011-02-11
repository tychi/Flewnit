/*
 * SimulatorMetaInfo.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "Simulator/SimulatorForwards.h"

#include "Buffer/BufferSharedDefinitions.h"

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




/*
 * Subset of the ShaderFeatures struct, which is customizable at runtime, depending on
 * material and LightingPipelineStage and Render target;
 *
 * */
struct ShaderFeaturesLocal
{
	//lighting stage-dependent features:
	//{
	RenderingTechnique renderingTechnique;
	//renderTargetTextureType delegates creation and/or configuration of a geometry shader,
	//defining layers for cubemap rendering or general layered rendering
	TextureType renderTargetTextureType;
	//}
	//material dependent features: they play no role for "global" shaders
	//like depthmap generation or deferred lighting
	//{
	VisualMaterialType visualMaterialType;
	ShadingFeatures shadingFeatures;
	//}
	//geometry dependent feature
	bool instancedRendering;

	explicit ShaderFeaturesLocal(
			RenderingTechnique renderingTechnique = RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
			TextureType renderTargetTextureType = TEXTURE_TYPE_2D,
			VisualMaterialType visualMaterialType = VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING ,
			ShadingFeatures shadingFeatures = SHADING_FEATURE_DIRECT_LIGHTING,
			bool instancedRendering = false
	);

	ShaderFeaturesLocal(const ShaderFeaturesLocal& rhs);

	//bool isSharable()const;
	//bool operator==(const ShaderFeaturesLocal& rhs);
	const ShaderFeaturesLocal& operator=(const ShaderFeaturesLocal& rhs);

	//convert to a string unique for every permutation
	String stringify()const;
};

//we need this function to use  ShaderFeaturesLocal as key value to boost::unordered map
std::size_t hash_value(ShaderFeaturesLocal const& sfl);
bool operator==(ShaderFeaturesLocal const& lhs, ShaderFeaturesLocal const& rhs);





/*
 * Shader feature values globally defined on Engine initialization;
 * a shader will be generated on the base of both an per-stage/per-material customizable
 * ShaderFeaturesLocal and the ShaderFeaturesGlobal struct;
 * We need some values to be globally and uniquely defined in order to assure rendering
 * consistency; e.g. a shadowmap should be generated and sampled by every shader in the same way;
 *
 * This struct is stored in and queriable from LightingSimulator
 */
struct ShaderFeaturesGlobal
{

	LightSourcesLightingFeature lightSourcesLightingFeature;
	LightSourcesShadowFeature lightSourcesShadowFeature;
	ShadowTechnique shadowTechnique;
	int numMaxLightSources;
	int numMaxShadowCasters;

	int numMaxInstancesRenderable;
	//{  only relevant for the deferred lighting stage
	//   (for g-buffer texel fetch from MS textures, accumulation of samples etc.);
	TextureType GBufferType;
	int numMultiSamples;
	//}

	//custom stuff: render to integer texture; will be handled in a custom shader;
	//bool renderIndices;


	explicit ShaderFeaturesGlobal(
			LightSourcesLightingFeature lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT,
			LightSourcesShadowFeature lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_NONE,
			ShadowTechnique shadowTechnique = SHADOW_TECHNIQUE_NONE,
			int numMaxLightSources = 1,
			int numMaxShadowCasters = 1,
			int numMaxInstancesRenderable = 1,

			TextureType GBufferType = TEXTURE_TYPE_2D_RECT,
			int numMultiSamples	= 1

			//bool renderIndices = false
	);


	ShaderFeaturesGlobal(const ShaderFeaturesGlobal& rhs);

	bool operator==(const ShaderFeaturesGlobal& rhs);
	const ShaderFeaturesGlobal& operator=(const ShaderFeaturesGlobal& rhs);

	void validate()throw(SimulatorException);
};

////for number values, also define its inverse, no matter if needed or not
//enum CustomShaderDefinitionIDs
//{
//	 RENDERING_TECHNIQUE,
//	 RENDER_TARGET_TEXTURE_TYPE,
//	 VISUAL_MATERIAL_TYPE,
//	 SHADING_FEATURES,
//	 INSTANCED_RENDERING,
//	 LIGHTSOURCES_LIGHTING_FEATURE,
//	 LIGHTSOURCES_SHADOW_FEATURE,
//	 SHADOW_TECHNIQUE,
//
//	 NUM_MAX_LIGHTSOURCES,
//	 INV_NUM_MAX_LIGHTSOURCES,
//
//	 NUM_MAX_SHADOW_CASTERS,
//	 INV_NUM_MAX_SHADOW_CASTERS,
//
//	 NUM_MAX_INSTANCES_RENDERABLE,
//
//	 G_BUFFER_TYPE,
//
//	 NUM_MULTISAMPLES,
//	 INV_NUM_MULTISAMPLES,
//
//	 __NUM_CUSTOM_SHADER_DEFINITIONS__
//};



}

