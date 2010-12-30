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

enum RenderTargetType
{
	SCREEN_RTT,
	FBO_RTT
};

enum SceneNodeTypeFlags
{
	PURE_NODE =0,
	CAMERA_NODE = 0<<1,

	RIGID_BODY = 0<<2,
	FLUID_OBJECT = 0<<3,
	SOFT_BODY_OBJECT = 0<<4,
	CLOTH_OBJECT = 0<<5,
	HAIR_OBJECT = 0<<6,

	VISUAL_OBJECT = 0<<7,
	SOUND_OBJECT = 0<<8

};

enum LightingFeatures
{
//	DEBUG_DRAW,
//	SKYDOME,
//	ENVIRONMENT_MAPPING,
//	PHONG_SHADING,
//	NORMAL_MAPPING,
//	TESSELATION,
//	AMBIENT_OCCLUSION,
//	GAS_RENDERING,
//	LIQUID_REDNERING
};





//features the pipeline stage provides itself;
class SimulationPipelineFeatures
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	enum LightingRenderingTechnique
	{
		DIRECT,
		DEFERRED
	};

};

//required features a pipeline stage needs fulfilled from the rest of the simulators/pipelines to work properly
class SimulationPipelineRequirements
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	List<String> mRequiredOtherStages;
public:

};

}

