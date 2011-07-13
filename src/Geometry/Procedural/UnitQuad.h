/*
 * UnitQuad.h
 *
 * Most simple Sqare in [(-1,-1,0)..(1,1,0)] for drawing a full screen quad;
 * Has only position attibute, nothing else; pixel position can be achieved from gl_FragCoord.
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */


#pragma once


#include "Geometry/VertexBasedGeometry.h"

namespace Flewnit
{

class UnitQuad
: public VertexBasedGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	UnitQuad(String name);
	virtual ~UnitQuad();
};

}


