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
	BoxGeometry(
			String name,
			const Vector3D& halfextends,
			bool addTangents,
			//set default draw mode to patches (for tesselation) instead of triangles
			bool patchRepresentation = false);

	virtual ~BoxGeometry();

private:

};

}


