/*
 * SimulationDataBase.h
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

class SimulationDataBase
:	public Singleton<SimulationDataBase>,
	public Flewnit::BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SimulationDataBase();
	virtual ~SimulationDataBase();

private:

	SceneGraph* mSceneGraph;

	Map<String, Material*> mMaterials;
};

}

