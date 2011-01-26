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

enum LightingFeatures
{
	DEBUG_DRAW				= 1<<0,
	SKYDOME					= 1<<1,
	SHADOW_MAP_GENERATION	= 1<<2,
	SHADOWING				= 1<<3,
	ENVIRONMENT_MAPPING		= 1<<4,
	PHONG_SHADING			= 1<<5,
	NORMAL_MAPPING			= 1<<6,
	TESSELATION				= 1<<7,
	AMBIENT_OCCLUSION		= 1<<8,
	GAS_RENDERING			= 1<<9,
	LIQUID_RENDERING		= 1<<10,
};

enum RenderTargetType
{
	SCREEN_RTT,
	FBO_RTT
};

enum LightingRenderingTechnique
{
	DIRECT,
	DEFERRED
};



////features the pipeline stage provides itself;
//class SimulationPipelineFeatures
//: public BasicObject
//{
//	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
//public:
//
//
//
//};
//
////required features a pipeline stage needs fulfilled from the rest of the simulators/pipelines to work properly
//class SimulationPipelineRequirements
//: public BasicObject
//{
//	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
//
//	List<String> mRequiredOtherStages;
//
//	//required render target
//
//	//required objects
//public:
//
//};

}

