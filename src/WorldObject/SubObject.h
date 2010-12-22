/*
 * SubObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

namespace Flewnit
{

#include "Common/BasicObject.h"

class SubObject : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	SimulationDomain mSimDomain;
public:
	SubObject(SimulationDomain simDomain,);
	virtual ~SubObject();

	SimulationDomain getSimDomain()const {return mSimDomain;}
};

}

