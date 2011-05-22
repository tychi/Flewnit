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

#include "Simulator/SimulatorMetaInfo.h"
#include "Simulator/SimulationObject.h"

namespace Flewnit
{

class SubObject
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	SubObject(String name, SimulationDomain simDomain, Geometry* geo, Material* mat);
	virtual ~SubObject();

	Geometry* getGeometry()const{return mGeometry;}
	Material* getMaterial()const{return mMaterial;}

	WorldObject* getOwningWorldObject(){return mOwningWorldObject;}
private:
	friend class WorldObject;
	//void setOwningWorldObject(WorldObject* wo){mOwningWorldObject= wo;}
	//backtracking, to get transform and all stuff ;); is set by friend'ed WorldObject directly
	WorldObject* mOwningWorldObject;

	Geometry* mGeometry;
	Material* mMaterial;


};

}

