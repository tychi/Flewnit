/*
 * AccelerationStructure.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "AccelerationStructure.h"

namespace Flewnit
{

	AccelerationStructure::AccelerationStructure(String name, AccelerationStructureType type)
	: 		WorldObject(name,SPATIAL_DATA_STRUCTURE_OBJECT),
			mAccelerationStructureType(type)
	{
		// TODO Auto-generated constructor stub

	}

	AccelerationStructure::~AccelerationStructure() {
		// TODO Auto-generated destructor stub
	}

};
