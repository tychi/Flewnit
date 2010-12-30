/*
 * Material.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class Material
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	SimulationDomain mSimDomain;
	String mName;

	SubObject* mOwningSubObject;

public:
	//TODO IN KIEL INTEGRATE INSTANCING STUFF ETC IN CONCEPT AND IMPLEMENT IT DIRECTLY
	Material(SimulationDomain sd);
	virtual ~Material();


	inline SimulationDomain getSimDomain()const {return mSimDomain;}
	inline String getName()const{return mName;}

	virtual bool activate()=0;
	virtual bool deactivate()=0;
};

}

