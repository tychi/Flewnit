/*
 * Geometry.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *  Class mainaining a pointer to buffers containing geometric information (VBO or severel OpenCL Buffers);
 *  These buffers will be treated differently, depending on the curren sim stage;
 *
 *  As a geom. object can be used by severel sim domains, it  has no specific domain and so is not derived from SimulationObject;
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorForwards.h"

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
	String mName;
public:
	Geometry(GeometryRepresentation geoRep, String name);
	virtual ~Geometry();

	GeometryRepresentation getGeometryRepresentation()const{return mGeometryRepresentation;}
	String getName()const{return mName;}

	virtual void render() =0;

};

}

