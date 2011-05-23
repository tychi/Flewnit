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
public:
	virtual ~InstancedGeometry();
	inline InstanceManager* getInstanceManager()const{return mInstanceManager;}

	virtual void draw(
				//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
				unsigned int numInstances,
				GeometryRepresentation desiredGeomRep);
private:
	friend class InstanceManager;
	//only InstanceManager may create objects of this type;
	InstancedGeometry(String name,InstanceManager* instanceManager, ID instanceID);
	//hen-egg-problem enforces the backtracking setting after construction ;(
	void setOwningSubObject(SubObject* owningSubObject);

	InstanceManager* mInstanceManager;
	//the InstancedGeometry class is the only class with a backtrack to a SubObject;
	//"real" Geometry can have many users, hence a backtrack would be senseless, but this
	//kind of proxy geometry is strongly associated to one single SubObject and hence WorldObject
	//(where we get the transformation matrices from)
	SubObject* mOwningSubObject;
	ID mInstanceID;
};

}

