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
#include "Simulator/SimulationObject.h"

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

	friend class WorldObject;
	void setOwningWorldObject(WorldObject* wo){mOwningWorldObject= wo;}

public:
	SubObject(String name, SimulationDomain simDomain, Geometry* geo, Material* mat);
	virtual ~SubObject();

	Geometry* getGeometry()const{return mGeometry;}
	Material* getMaterial()const{return mMaterial;}

	WorldObject* getOwningWorldObject(){return mOwningWorldObject;}
};

}

