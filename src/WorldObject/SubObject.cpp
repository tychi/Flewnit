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

SubObject::SubObject(WorldObject* owningWorldObject ,SimulationDomain simDomain, Geometry* geo, Material* mat)
: mOwningWorldObject(owningWorldObject), mSimDomain(simDomain), mGeometry(geo), mMaterial(mat)
{

}

SubObject::~SubObject()
{
	delete mGeometry;
	delete mMaterial;
}

}
