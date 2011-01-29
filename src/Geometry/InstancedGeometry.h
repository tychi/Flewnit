/*
 * InstancedGeometry.h
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#pragma once

#include "Geometry.h"
//#include "WorldObject/InstanceManager.h"

namespace Flewnit
{

class InstancedGeometry
: public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	InstanceManager* mInstanceManager;
	// is FLEWNIT_INVALID_ID if subobject is not instanced
	ID mInstanceID;

public:
	InstancedGeometry(String name, GeometryRepresentation geoRep,InstanceManager* instanceManager, ID instanceID);
	virtual ~InstancedGeometry();

	virtual void draw(
				//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
				unsigned int numInstances,
				GeometryRepresentation desiredGeomRep);

};

}

