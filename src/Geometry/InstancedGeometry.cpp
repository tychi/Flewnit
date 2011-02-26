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

InstancedGeometry::InstancedGeometry(String name, InstanceManager* instanceManager,  SubObject* owningSubObject, ID instanceID)
:
		Geometry(name,  INSTANCED_GEOMETRY_REPESENTATION),
		mInstanceManager(instanceManager), mOwningSubObject(owningSubObject), mInstanceID(instanceID	)
{
	// TODO Auto-generated constructor stub

}

InstancedGeometry::~InstancedGeometry()
{
	// TODO Auto-generated destructor stub
}

void InstancedGeometry::draw(
		unsigned int numInstances,
		GeometryRepresentation desiredGeomRep)
{
	mInstanceManager->registerInstanceForNextDrawing(this);
	//do nothing, as actual batched drawing happens at the end of a render pass
}


}
