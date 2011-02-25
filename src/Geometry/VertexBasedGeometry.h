/*
 * VertexBasedGeometry.h
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 *
 * The most common geometry Representation; Owns and maintains an OpenGL Vertex Buffer Object;
 * Is sharable with OpenCL; Is used for point, line and triangle rendering;
 */

#pragma once

#include "Geometry.h"

namespace Flewnit
{



class VertexBasedGeometry :
	public BufferBasedGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	VertexBasedGeometry(String name, GeometryRepresentation geoRep);
	//constructor for patch-based tesselation geometry
	VertexBasedGeometry(String name, GLint verticesPerPatch);
	virtual ~VertexBasedGeometry();

	//the semantics of the buffer is readable from its bufferInfo
	//non-pure virtual, as it must be overridden by VertexBasedGeometry to register
	//to the VBO
	//Convention for the BufferElementInfo of a Buffer designated to be an attribute buffer:
	//if internal GPU data type is int or uint, it will always be handled as integer attributes,
	//unless the normalization flag is set; Note especcially that non-normalized int-to-float
	//conversions aren't supported this way; This is on purpose, as
	//	1.: I don't see any advantage in reading unnormalized integer values and convert
	//		them to float;
	//	2.: Control flow and usage flags to be tracked are less complex;
	//if one wants save memory, then the GL_HALF data type shall be used
	//(though I didn't test it in the glm library)
	virtual void setAttributeBuffer(BufferInterface* buffi) throw(BufferException);

	void setIndexBuffer(BufferInterface* buffi) throw(BufferException);
	//returns NULL if index buffer doesn't exist;
	BufferInterface* getIndexBuffer();

	virtual void draw(
				//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
				unsigned int numInstances,
				GeometryRepresentation desiredGeomRep);

	//returns zero if geometry is not "patch driven"
	inline GLint getNumVerticesPerPatch()const {return mNumVerticesPerPatch;}

protected:

	//compare buffers for sizees, types, number of elements etc;
	virtual void validateBufferIntegrity()throw(BufferException);

	void setUpPatchRepresentationState();

private:

	//handle to the OpenGL Vertex Buffer Object;
	GLuint mGLVBO;

	BufferInterface* mIndexBuffer;

	GLint mNumVerticesPerPatch;

	void bind();
	void unbind();

//	void bindSafe();
//	void unBindSave();
};

}


