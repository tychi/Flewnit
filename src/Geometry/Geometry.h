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

	//folowing became obsolte:
	//if the Geometry is shared by several sim. domains, is has several Subobjects to backtrack:
	//geometry can be coupled that hard to a subobject, as if one wants to use a VBO for several
	//draws per frame, one will use instancing :D.
	//SubObject* mOwningSubObjects[__NUM_SIM_DOMAINS__];
	//friend class SubObject;
	//void setOwningSubObject(SimulationDomain sd, SubObject* so){mOwningSubObjects[sd]= so;}

public:

	Geometry( String name, GeometryRepresentation geoRep);
	virtual ~Geometry();

	GeometryRepresentation getGeometryRepresentation()const{return mGeometryRepresentation;}

	/*
	 * Render the geometry in the classic way, i.e. usually in the visual domain with OpenGL.
	 *
	 * @param 	currentStage to determine for the geometry what to to; If the backtrack
	 * 			to the sim. stage is really necessary for geometry, is not yet determined,
	 * 			but in doubt the more information, the better, as the several and generic
	 * 			simulation domains can become quite complex and one could at least use the backtrack
	 * 			for error checking.
	 * @param	currentUsingSuboject needed for getting associated MAterial an WorldObject (for transformation matrix)
	 * @param	numInstances if <=1, default non-instanced rendering with subobject query for matrix etc.;
	 * 			if >1, then rely on the fact that the method is called by an InstanceManager and don't care about matrices etc;
	 */
	virtual void draw(
			//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
			unsigned int numInstances=1,
			GeometryRepresentation desiredGeomRep = DEFAULT_GEOMETRY_REPRESENTATION) =0;


};


/**
 *  *  Class maintaining pointers to buffers containing geometric information (VBO and/or several ((non)interop) OpenCL Buffers);
 *  	These buffers will be treated differently, depending on the curren sim stage;
 */
class BufferBasedGeometry :
	public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	BufferBasedGeometry(String name, GeometryRepresentation geoRep);
	virtual ~BufferBasedGeometry();

	//the semantics of the buffer is readable from its bufferInfo
	//@param useInGLRendering controls if the buffer will be bound as a generic attribute buffer
	//to the VBO for usage in vertex/geometry shaders; if false, the buffer will only have
	//any use in OpenCL contexts; (Example: We need z-index etc in openCL, but in OpenGL
	//at most for debug drawing;)
	//The usageFlags param is not necessarily identical to buffi->getBufferInfo().usageFlags,
	//as a certain shading may not use all features of a buffer and buffers may be shared by different
	//geometry/WorldObjects and hence Materials;
	void setAttributeBuffer(BufferInterface* buffi, ContextTypeFlags usageFlags) throw(BufferException);


	//can return NULL pointer if buffer is not registered for the given semantics
	BufferInterface* getAttributeBuffer(BufferSemantics bs);


	virtual void draw(
				//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
				unsigned int numInstances,
				GeometryRepresentation desiredGeomRep) = 0;

	//TODO maybe some activation getter/setter to change GL rendering behaviour at runtime;

protected:


	//A "list" of pointers to relevant generic vertex Attribute OpenGL buffers;
	BufferInterface* mAttributeBuffers[__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__];
	//buffers can be set, but may be not used while openGL rendering
	//(as e.g. their values are only needed in openCL)
	//this array serves as indicator, which buffers should be part of the VBO-canon
	ContextTypeFlags mUsageFlags[__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__];


	//compare buffers for sizees, types, number of elements etc;
	virtual void validateBufferIntegrity()throw(BufferException)=0;

	void bindSafe();
	void unBindSave();
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

