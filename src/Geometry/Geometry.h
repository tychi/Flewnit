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




class Geometry
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	GeometryRepresentation mGeometryRepresentation;
	//if the Geometry is shared by several sim. domains, is has several Subobjects to backtrack:
	SubObject* mOwningSubObjects[__NUM_SIM_DOMAINS__];

	friend class SubObject;
	void setOwningSubObject(SimulationDomain sd, SubObject* so){mOwningSubObjects[sd]= so;}

public:
	Geometry( String name, SimulationDomain sd, GeometryRepresentation geoRep);
	virtual ~Geometry();

	GeometryRepresentation getGeometryRepresentation()const{return mGeometryRepresentation;}

	virtual void render(SimulationPipelineStage* currentStage,
			GeometryRepresentation geomRep = DEFAULT_GEOMETRY_REPRESENTATION) =0;


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


/*
 * The most common geometry Representation; Owns and maintains an OpenGL Vertex Buffer Object;
 * Is shareable with OpenCL; Is used for point, line and triangle rendering;
 */
class VertexBasedGeometry : public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	VertexBasedGeometry()

	//the semantics of the buffer is readable from its bufferInfo
	//@param useInGLRendering controls if the buffer will be bound as a generic attribute buffer
	//to the VBO for usage in vertex/geometry shaders; if false, the buffer will only have
	//any use in OpenCL contexts; (Example: We need z-index etc in openCL, but in OpenGL
	//at most for debug drawing;)
	void setAttributeBuffer(BufferInterface* buffi, bool useInGLRendering) throw(BufferException);

	void setIndexBuffer(BufferInterface* buffi) throw(BufferException);

	//TODO maybe some activation getter/setter to change GL rendering behaviour at runtime;

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

	//handle to the OpenGL Vertex Buffer Object;
	GLuint mGLVBO;
	bool mIsSharedWithOpenCL;

	BufferInterface* mAttributeBuffers[__NUM_VALID_VERTEX_ATTRIBUTE_SEMANTICS__];
	//buffers can be set, but may be not used while openGL rendering
	//(as e.g. their values are only needed in openCL)
	bool mGLactiveGuards[__NUM_VALID_VERTEX_ATTRIBUTE_SEMANTICS__];

	BufferInterface* mIndexBuffer;

	//compare buffers for sizees, types, number of elements etc;
	void validateBufferIntegrity()throw(BufferException);
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

