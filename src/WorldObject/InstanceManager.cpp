/*
 * InstanceManager.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "InstanceManager.h"

#include "WorldObject/SubObject.h"

namespace Flewnit
{

bool InstanceManager::sInstancedRenderingIsCurrentlyActive = false;

InstanceManager::InstanceManager(String name, GLuint numMaxInstances,
		Material* associatedNonInstancedMaterial, Geometry* nonInstancedGeometryToDraw )
:
	SimulationObject(name, VISUAL_SIM_DOMAIN),
	mMaxManagedInstances(numMaxInstances),
	mCreatedManagedInstances(0),
	mNumCurrentlyRegisteredInstancesForNextDrawing(0),
	mInstanceTransformationInfoUniformBuffer(0),
	mAssociatedNonInstancedMaterial(associatedNonInstancedMaterial),
	mNonInstancedGeometryToDraw(nonInstancedGeometryToDraw)
{

	assert(0 && "TODO implement");
}

InstanceManager::~InstanceManager()
{
	//nothing to do, everythin created/managed will be delete by resource manager or the owning WorldObjects
}

//creates a new SubObject, containing a new InstancedGeometry and a re-used InstancedMaterial object
//throws exception if
SubObject* InstanceManager::createInstance()throw(SimulatorException)
{
	assert(0 && "TODO implement");
	//return new SubObject(mName+ String());
}

//called by InstancedGeometry::draw(); the owning WorldObject is backtracked, its relevant matrices extracted
//and the uniform buffer entry filled with the relevant information;
void InstanceManager::registerInstanceForNextDrawing(InstancedGeometry* instancedGeo)
{
	assert(0 && "TODO implement");
}

void InstanceManager::drawRegisteredInstances()
{
	sInstancedRenderingIsCurrentlyActive = true;

	assert(0 && "TODO implement");

	sInstancedRenderingIsCurrentlyActive = false;
}


}
