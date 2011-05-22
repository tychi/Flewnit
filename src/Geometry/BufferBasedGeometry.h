/*
 * BufferBasedGeometry.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "Geometry/Geometry.h"


namespace Flewnit {

/**
 *  *  Class maintaining pointers to buffers containing geometric information (VBO and/or several ((non)interop) OpenCL Buffers);
 *  	These buffers will be treated differently, depending on the current sim stage;
 */
class BufferBasedGeometry :
	public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	BufferBasedGeometry(String name, GeometryRepresentation geoRep);
	virtual ~BufferBasedGeometry();

	//the semantics of the buffer is readable from its bufferInfo
	//non-pure virtual, as it must be overridden by VertexBasedGeometry to register
	//to the VBO;
	//The BufferSemantics param is used for "unsetting" functionality: pass a null pointer and the
	//desired semantics to "unset"; When passing a non-null pointerm the second param is ignored,
	//because the semantics can be read directly from the buffer itself.
	virtual void setAttributeBuffer(BufferInterface* buffi, BufferSemantics bs = INVALID_SEMANTICS) throw(BufferException);




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
	//ContextTypeFlags mUsageFlags[__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__];


	//compare buffers for sizees, types, number of elements etc;
	virtual void validateBufferIntegrity()throw(BufferException)=0;

};

}


