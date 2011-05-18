/*
 * Geometry.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *	Several different Geometry representations;
 *
 *  As a geom. object can be used by several sim domains, it  has no specific domain and so its Simulation domain is GENERIC_SIM_DOMAIN;
 */

#pragma once


#include "Simulator/SimulatorForwards.h"

#include "../Simulator/SimulationObject.h"

#include "Buffer/BufferSharedDefinitions.h"

namespace Flewnit
{

/*
 * Representation enums for both Drawing and Simulation; It is rather a brainstorming than a complete
 * list
 */
enum GeometryRepresentation
{
	//used as default parameter for the render() function
	DEFAULT_GEOMETRY_REPRESENTATION,
	INSTANCED_GEOMETRY_REPESENTATION, //indicator that its just a reference to "real" geometry

	//--------------------------------------
	//following "good buffer-representable" representations:
	//e.g. for particles
	VERTEX_BASED_POINT_CLOUD,
	//maybe for hair simulation? ...
	VERTEX_BASED_LINES,
	VERTEX_BASED_LINE_STRIP,
	//default triangle stuff
	VERTEX_BASED_TRIANGLES,
	VERTEX_BASED_TRIANGLES_ADJACENCY,

	VERTEX_BASED_TRIANGLE_PATCHES,
	VERTEX_BASED_QUAD_PATCHES,
	VERTEX_BASED_BEZIER_PATCHES,

	VERTEX_BASED_CONVEX_HULL,

	//following two would be easy to abstract in CUDA, but not in OpenCL (yet) :C ;
	//e.g. representation for Uniform Grid acc. structure;
	VOXEL_GRID_BUFFER_BASED,
	//e.g. usage for smoke simulation and visualizing
	VOXEL_GRID_TEXTURE3D_BASED,
	//----------------------------------------------------

	//debug draw with only a few vertices via heavily instanced line draw;
	//for debug drawing the uniform grid
	VOXEL_GRID_AXIS_ALIGNED_IMPLICIT,

	PRIMITIVE_BASED_BOX,
	PRIMITIVE_BASED_SPHERE,
	PRIMITIVE_BASED_COMPOUND,
	//.. more collision shape stuff for rigid bodies..

	//Bezier Surfaces, NURBS etc..
	PARAMETRIC_CURVE


};




class Geometry
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;


public:

	Geometry( String name, GeometryRepresentation geoRep);
	virtual ~Geometry();

	GeometryRepresentation getGeometryRepresentation()const{return mGeometryRepresentation;}

	/*
	 * Render the geometry in the classic way, i.e. usually in the visual domain with OpenGL.
	 * @param	numInstances if <=1, default non-instanced rendering with subobject query for matrix etc.;
	 * 			if >1, then rely on the fact that the method is called by an InstanceManager and don't care about matrices etc;
	 * @param	desiredGeomRep currently unused, maybe oboloet; TODO re-think about this param, possibly delete

	 */
	virtual void draw(
			//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
			unsigned int numInstances=1,
			GeometryRepresentation desiredGeomRep = DEFAULT_GEOMETRY_REPRESENTATION ) = 0;

protected:
	GeometryRepresentation mGeometryRepresentation;

};





}

//became obsolete TODO delete when sure it won't be needed
////decouple draw mode from geom. representation, as some rep.s can be drawn in different ways, e.g. triangle meshes as points, wireframe
//enum DrawMode
//{
//	DEFAULT_DRAW_MODE,
//
//	POINTS_STATIC_SIZE,
//	//desired draw mode for initial liquid particle rendering
//	POINTS_DYNAMIC_SIZE,
//
//	TRIANGLES,
//	TRIANGLES_ADJACENCY,
//
//	WIRE_FRAME
//
//};

