/*
 * SimulationResourceManager.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "SimulatorForwards.h"

namespace Flewnit
{

class SimulationResourceManager
:	public Singleton<SimulationResourceManager>,
	public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SimulationResourceManager();
	virtual ~SimulationResourceManager();

private:

	Scene* mScene;

	Map<ID, InstanceManager*> mRegisteredInstanceManagers;

	Map<String, SimulationPipelineStage*> mRegisteredPipeLineStages;


	Map<String, WorldObject*> mWorldObjects;
	Map<ID, SubObject*>	mSubObjects;
	Map<String, Geometry*> mGeometries;

	Map<String, Material*> mMaterials;

	Map<ID, BufferInterface* > mBuffers;

	Map<String, MPP*>		mMPPs;
};

}

