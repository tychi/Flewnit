/*
 * SubObject.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SubObject.h"

#include "Geometry/Geometry.h"
#include "Material/Material.h"

namespace Flewnit
{

//WorldObject::WorldObject()
//{
//	// TODO Auto-generated constructor stub
//
//}
//
//WorldObject::~WorldObject()
//{
//	// TODO Auto-generated destructor stub
//}

SubObject::SubObject(String name, SimulationDomain simDomain, Geometry* geo, Material* mat)
: 		SimulationObject(name,simDomain),
		mOwningWorldObject(0), mGeometry(geo), mMaterial(mat)
{
	//mGeometry->setOwningSubObject(getSimDomain(),this);
}

SubObject::~SubObject()
{
	//don't delete stuff as geo and mat are shared and maintained by the SimResourceManager;
}

}
