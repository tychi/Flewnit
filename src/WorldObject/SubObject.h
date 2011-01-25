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

#include "../Simulator/SimulationObject.h"

#include "Simulator/SimulatorMetaInfos.h"


namespace Flewnit
{

class SubObject
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


	//backtracking, to get transform and all stuff ;)
	WorldObject* mOwningWorldObject;


	Geometry* mGeometry;
	Material* mMaterial;


public:
	SubObject(String name, SimulationDomain simDomain, WorldObject* owningWorldObject , Geometry* geo, Material* mat);
	virtual ~SubObject();

	inline Geometry* getGeometry()const{return mGeometry;}
	inline Material* getMaterial()const{return mMaterial;}
	inline WorldObject* getOwningWorldObject()const{return mOwningWorldObject;}
};

}

