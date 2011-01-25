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

SubObject::SubObject(String name, SimulationDomain simDomain, WorldObject* owningWorldObject , Geometry* geo, Material* mat)
:
	SimulationObject(name, simDomain),
	mOwningWorldObject(owningWorldObject), mGeometry(geo), mMaterial(mat)
{

}

SubObject::~SubObject()
{
	delete mGeometry;
	delete mMaterial;
}

}
