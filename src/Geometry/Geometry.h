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


#include "Simulator/SimulatorForwards.h"
#include "../Simulator/SimulationObject.h"

namespace Flewnit
{

/*
 * Representation enums for both Drawing and Simulation; It is rather a brainstorming but a complete
 * list
 */
enum GeometryRepresentation
{
	//used as default parameter for the render() function
	DEFAULT_GEOMETRY_REPRESENTATION,

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

class Geometry
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	GeometryRepresentation mGeometryRepresentation;
	SubObject* mOwningSubObject;

public:
	Geometry( String name, SimulationDomain sd, GeometryRepresentation geoRep, SubObject* owningSO);
	virtual ~Geometry();

	GeometryRepresentation getGeometryRepresentation()const{return mGeometryRepresentation;}

	virtual void render(SimulationPipelineStage* currentStage, GeometryRepresentation geomRep = DEFAULT_GEOMETRY_REPRESENTATION) =0;


};


class InstancedGeometry : public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	//TODO IN KIEL FUCKING INSTANCE MANAGER
	//is NULL if subobject is not instanced
	InstanceManager* mInstanceManager;
	// is FLEWNIT_INVALID_ID if subobject is not instanced
	ID mInstanceID;

};

class VertexBasedGeometry : public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;


private:

	/*
	 * A "list" of pointers to relevant generic vertex Attribute OpenGL buffers;
	 * They are requested to be created by the
	 * SimulationResourceManager, all with GL-interop, so that the Lighting Simulator can
	 * access all values it needs for the rendering of the fluid; This way, the sophistication
	 * of the rendering can be decoupled from the mechanics simulation;
	 *
 	 Allowed Semantics are:

		POSITION_SEMANTICS,
		INDEX_SEMANTICS,

		VELOCITY_SEMANTICS,
		MASS_SEMANTICS,
		DENSITY_SEMANTICS,
		PRESSURE_SEMANTICS,
		FORCE_SEMANTICS,

		Z_INDEX_SEMANTICS,
	*/
	Map<BufferSemantics, BufferInterface*> mFluidSimulationBuffers;
};

}

