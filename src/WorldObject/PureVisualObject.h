/*
 * PureVisualObject.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 *
 *  class for every objects which hav no other representation but the visual one
 */


#include "WorldObject/WorldObject.h"

#pragma once

namespace Flewnit
{

class PureVisualObject
: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	PureVisualObject(String name, const AmendedTransform& localtransform = AmendedTransform());

	virtual ~PureVisualObject();
};

}
