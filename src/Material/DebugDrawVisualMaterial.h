/*
 * DebugDrawVisualMaterial.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once

#include "Material/VisualMaterial.h"

#include "Geometry/Geometry.h"

namespace Flewnit
{

class DebugDrawVisualMaterial
	: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	DebugDrawVisualMaterial( String name, const Vector4D& debugDrawColor,
			//must be VERTEX_BASED_POINT_CLOUD or VERTEX_BASED_LINES or VERTEX_BASED_TRIANGLES
			GeometryRepresentation geomRepToDraw = VERTEX_BASED_LINES  );

	virtual ~DebugDrawVisualMaterial();
private:
	float mDebugDrawColor;
};

}
