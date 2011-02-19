/*
 * BoxGeometry.h
 *
 *  Created on: Feb 19, 2011
 *      Author: tychi
 */


#pragma once


#include "Geometry/VertexBasedGeometry.h"

namespace Flewnit
{

class BoxGeometry
: public VertexBasedGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	BoxGeometry();
	virtual ~BoxGeometry();
};

}


