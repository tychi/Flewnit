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
: SceneNode(
		name,
		//add visual flag, as a world object is always kind of visible
		SceneNodeTypeFlags(typeflags | VISUAL_OBJECT),
		localtransform)
{

	// TODO Auto-generated constructor stub

}

WorldObject::~WorldObject()
{
	for (unsigned int i= 0; i<__NUM_SIM_DOMAINS__; i++)
		BOOST_FOREACH(SubObject* so, mPhysicalReps[i])
		{
			delete so;
		}
}


std::vector<SubObject*>& WorldObject::getSubObjects(SimulationDomain whichDomain)
{
	assert(whichDomain < __NUM_SIM_DOMAINS__);
	return mPhysicalReps[whichDomain];
}

//in which of the lists it will be inserted is determined by the SimulationDomain flag of the subObject;
void WorldObject::addSubObject(SubObject* so)throw(SimulatorException)
{
	mPhysicalReps[so->getSimDomain()].push_back(so);
	so->mOwningWorldObject=this;
}


}
