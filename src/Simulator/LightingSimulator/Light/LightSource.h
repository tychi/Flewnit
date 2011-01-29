/*
 * LightSource.h
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#pragma once

#include "WorldObject/WorldObject.h"

namespace Flewnit
{

class LightSource
:public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	LightSource();
	virtual ~LightSource();
};

}
