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

	//neede by materials to get info about current sim pipeline stage
	inline SimulationPipelineStage* getCurrentSimulationPipelineStage()const
			{return mCurrentSimulationPipelineStage;}


	RenderTarget* getGlobalRenderTarget()const;

	Scene* getScene()const;

	void registerInstanceManager();

private:

	friend class URE;
	void setCurrentSimulationPipelineStage(SimulationPipelineStage* current);

	SimulationPipelineStage* mCurrentSimulationPipelineStage;

	Scene* mScene;
	//RenderTarget* mGlobalRenderTarget;

	Map<String, InstanceManager*> mRegisteredInstanceManagers;

	Map<String, SimulationPipelineStage*> mRegisteredPipeLineStages;


	Map<String, WorldObject*> mWorldObjects;
	Map<ID, SubObject*>	mSubObjects;



	Map<String, Material*> mMaterials;
	Map<String, Geometry*> mGeometries;

	Map<ID, BufferInterface* > mBuffers;

	Map<String, MPP*>		mMPPs;
};

}

