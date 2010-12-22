/*
 * SubObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *  Interface class for the assets of a specific Simulation domain;
 *  For "normal" Light rendering, this class would be called "Mesh"; But we have to abtract it in order to be also used for other simulations;
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class SubObject
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	//backtracking, to get transform and all stuff ;)
	WorldObject* mOwningWorldObject;

	SimulationDomain mSimDomain;

	Geometry* mGeometry;
	Material* mMaterial;

	//TODO IN KIEL FUCKING INSTANCE MANAGER
	//is NULL if subobject is not instanced
	InstanceManager* mInstanceManager;
	// is FLEWNIT_INVALID_ID if subobject is not instanced
	ID mInstanceID;

public:
	SubObject(WorldObject* owningWorldObject ,SimulationDomain simDomain, Geometry* geo, Material* mat);
	virtual ~SubObject();

	Geometry* getGeometry()const{return mGeometry;}
	Material* getMaterial()const{return mMaterial;}

	inline SimulationDomain getSimDomain()const {return mSimDomain;}
};

}

