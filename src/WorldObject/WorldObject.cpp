/*
 * WorldObject.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "WorldObject.h"

#include "SubObject.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

WorldObject::WorldObject(String name,
		SceneNodeTypeFlags typeflags,
		Matrix4x4 localtransform = Matrix4x4() )
: SceneNode(name, typeflags, localtransform)
{
	// TODO Auto-generated constructor stub

}

WorldObject::~WorldObject()
{
	// TODO Auto-generated destructor stub
}


List<Subobject*>& WorldObject::getSubObjects(SimulationDomain whichDomain)
{
	assert(whichDomain < __NUM_SIM_DOMAINS__);
	return mPhysicalRep[whichDomain];
}

//in which of the lists it will be inserted is determined by the SimulationDomain flag of the subObject;
void WorldObject::addSubObject(SubObject* so)
{
	mPhysicalRep.mSubObjects[so->getSimDomain()].push_back(so);
}


}
