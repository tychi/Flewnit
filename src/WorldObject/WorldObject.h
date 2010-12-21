/*
 * WorldObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

namespace Flewnit
{

class WorldObject : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	WorldObject();
	virtual ~WorldObject();
};

}

