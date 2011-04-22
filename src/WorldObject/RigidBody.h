/*
 * RigidBody.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#pragma once

#include "WorldObject/WorldObject.h"


namespace Flewnit
{

class RigidBody
	: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	RigidBody(String name, const AmendedTransform& localtransform);
	virtual ~RigidBody();

protected:

	Vector4D linearVelocity;
    Vector4D angularVelocity;
    //to be synch'ed with scenegraph transform (maybe with a scale and centgre of mass-position-offset)
    //after every simulation step
    AmendedTransform mMechanicalWorldTransform;

    //WorldObject::mGlobalTransform =
    //		MechanicalMaterial::mMechanicalWorldTransform * translate(- mLocalCentreOfMass);
    //
    Vector4D mLocalCentreOfMass;
};


}
