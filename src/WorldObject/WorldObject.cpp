/*
 * WorldObject.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "WorldObject.h"

#include "SubObject.h"
#include "Common/AmendedTransform.h"

#include <boost/foreach.hpp>


namespace Flewnit
{

WorldObject::WorldObject(String name,
		SceneNodeTypeFlags typeflags,
		const AmendedTransform& localtransform)
: SceneNode(name, typeflags, localtransform)
{

	// TODO Auto-generated constructor stub

}

WorldObject::~WorldObject()
{
	// TODO Auto-generated destructor stub
}


std::vector<SubObject*>& WorldObject::getSubObjects(SimulationDomain whichDomain)
{
	assert(whichDomain < __NUM_SIM_DOMAINS__);
	return mPhysicalReps[whichDomain];
}

//in which of the lists it will be inserted is determined by the SimulationDomain flag of the subObject;
void WorldObject::addSubObject(SubObject* so)
{
	mPhysicalReps[so->getSimDomain()].push_back(so);
	so->setOwningWorldObject(this);
}


}
