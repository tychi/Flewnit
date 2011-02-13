/*
 * Box.h
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 *
 * It is questionable if an own class is appropriate, but at least this will serve as
 * the first renderable object for first testing of the structure of the system,
 *  without the necessity of any asset loader ;)
 */

#include "PureVisualObject.h"

#pragma once

namespace Flewnit
{

class Box
: public PureVisualObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	Box(String name, const AmendedTransform& localTransform, const Vector3D& halfextends = Vector3D(2.0f, 1.0f,0.33f));
	virtual ~Box();
};

}
