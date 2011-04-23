/*
 * WorldObject.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Scene/SceneNode.h"

#include "Simulator/SimulatorMetaInfo.h"

namespace Flewnit
{

//class PhysicalRepresentation
//{
//	friend class WorldObject;
//	//if more than one SubObject for the mechanical domain is useful is in doubt.
//	//But it makes sense for lighting and sound domains. Flexibility brings responsiblity :P.
//	List<SubObject*> mSubObjects[__NUM_SIM_DOMAINS__];
//};

class WorldObject : public SceneNode
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


public:
	WorldObject(
			String name,
			SceneNodeTypeFlags typeflags,
			const AmendedTransform& localtransform = AmendedTransform());

	virtual ~WorldObject();

	std::vector<SubObject*>& getSubObjects(SimulationDomain whichDomain);

	//in which of the lists it will be inserted is determined by the SimulationDomain flag of the subObject;
	//is virtual as some derived world objects might do some compatibility checks etc.
	//beforre calling the super implementation
	virtual void addSubObject(SubObject* so)throw(SimulatorException);




private:

	//if more than one SubObject for the mechanical domain is useful is in doubt.
	//But it makes sense for lighting and sound domains. Flexibility brings responsiblity :P.
	std::vector<SubObject*> mPhysicalReps[__NUM_SIM_DOMAINS__];


};

}

