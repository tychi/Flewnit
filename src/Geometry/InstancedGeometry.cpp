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

InstancedGeometry::InstancedGeometry(String name, InstanceManager* instanceManager,  ID instanceID)
:
		Geometry(name,  INSTANCED_GEOMETRY_REPESENTATION),
		mInstanceManager(instanceManager), mOwningSubObject(0), mInstanceID(instanceID	)
{
	// TODO Auto-generated constructor stub

}

InstancedGeometry::~InstancedGeometry()
{
	//nothing to do
}

//hen-egg-problem enforces the backtracking setting after construction ;(
void InstancedGeometry::setOwningSubObject(SubObject* owningSubObject)
{
	mOwningSubObject= owningSubObject;
}

void InstancedGeometry::draw(
		unsigned int numInstances,
		GeometryRepresentation desiredGeomRep)
{
	assert(mOwningSubObject && "owning subobject must have been set!");
	mInstanceManager->registerInstanceForNextDrawing(this);
	//do nothing, as actual batched drawing happens at the end of a render pass
}


}
