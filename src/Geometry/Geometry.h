/*
 * Geometry.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

namespace Flewnit
{

enum GeometryRepresentation
{
	TRIANGLE_MESH,
	POINT_CLOUD,
	VOXEL_GRID
};

class Geometry
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	GeometryRepresentation mGeometryRepresentation;
public:
	Geometry();
	virtual ~Geometry();

	virtual void render() =0;

};

}

