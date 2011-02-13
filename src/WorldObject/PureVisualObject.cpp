/*
 * PureVisualObject.cpp
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#include "PureVisualObject.h"


namespace Flewnit
{

PureVisualObject::PureVisualObject(String name,
		const AmendedTransform& localtransform)
:
		WorldObject( name, VISUAL_OBJECT,localtransform)
{

}

PureVisualObject::~PureVisualObject()
{

}

}
