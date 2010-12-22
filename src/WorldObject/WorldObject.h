/*
 * WorldObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Scene/SceneNode.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class PhysicalRepresentation
{
	friend class WorldObject;
	List<SubObject*> mSubObjects[__NUM_SIM_DOMAINS__];
};

class WorldObject : public SceneNode
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


public:
	WorldObject(
			String name,
			SceneNodeTypeFlags typeflags,
			Matrix4x4 localtransform = Matrix4x4() );

	virtual ~WorldObject();

	List<SubObject*>& getSubObjects(SimulationDomain whichDomain);

	//in which of the lists it will be inserted is determined by the SimulationDomain flag of the subObject;
	void addSubObject(SubObject* so);




private:

	PhysicalRepresentation mPhysicalRep;


};

}

