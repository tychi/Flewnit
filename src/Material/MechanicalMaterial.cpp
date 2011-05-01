/*
 * MechanicalMaterial.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "MechanicalMaterial.h"

namespace Flewnit
{

MechanicalMaterial::MechanicalMaterial(String name, float mass)
:
	Material(name, MECHANICAL_SIM_DOMAIN),
	mMass(mass),
	mFriction(0.01f),
	mRestitution(0.5f)
{

}

MechanicalMaterial::MechanicalMaterial(String name, float mass, float friction, float restitution)
:
	Material(name, MECHANICAL_SIM_DOMAIN),
	mMass(mass),
	mFriction(friction),
	mRestitution(restitution)
{

}

MechanicalMaterial::~MechanicalMaterial()
{

}

}
