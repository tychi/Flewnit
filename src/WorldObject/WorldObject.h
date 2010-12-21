/*
 * WorldObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Scene/SceneNode.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class WorldObject : public SceneNode
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


public:
	WorldObject();
	virtual ~WorldObject();
};

}

