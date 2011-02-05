/*
 * InstancedGeometry.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "InstancedGeometry.h"
#include "WorldObject/InstanceManager.h"

namespace Flewnit
{

InstancedGeometry::InstancedGeometry(String name, GeometryRepresentation geoRep,InstanceManager* instanceManager, ID instanceID)
:
		Geometry(name,  geoRep),
		mInstanceManager(instanceManager), mInstanceID(instanceID	)
{
	// TODO Auto-generated constructor stub

}

InstancedGeometry::~InstancedGeometry()
{
	// TODO Auto-generated destructor stub
}

}
