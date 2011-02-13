/*
 * VertexBasedGeometry.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "VertexBasedGeometry.h"
#include "Simulator/OpenCL_Manager.h"
#include "Buffer/BufferInterface.h"

namespace Flewnit
{

VertexBasedGeometry::VertexBasedGeometry(String name, GeometryRepresentation geoRep)
: BufferBasedGeometry(name, geoRep), mIndexBuffer(0)
{
	GUARD( glGenVertexArrays(1, &mGLVBO));
	bind();
}

VertexBasedGeometry::~VertexBasedGeometry()
{
	bind();
	for(int i=0; i< __NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__;i++)
	{
		if(mAttributeBuffers[i])
		{
			GUARD(glDisableVertexAttribArray(static_cast<GLuint>(mAttributeBuffers[i]->getBufferInfo().bufferSemantics)));
		}
	}
	unbind();

	GUARD(glDeleteVertexArrays(1,&mGLVBO));
}


void  VertexBasedGeometry::bind()
{
	GUARD( glBindVertexArray(mGLVBO) );
}
void  VertexBasedGeometry::unbind()
{
	GUARD( glBindVertexArray(0) );
}


void VertexBasedGeometry::setAttributeBuffer(BufferInterface* buffi) throw(BufferException)
{
	bind();
	BufferBasedGeometry::setAttributeBuffer(buffi);
	validateBufferIntegrity();

	buffi->bind(OPEN_GL_CONTEXT_TYPE);

	GLenum elementTypeGL;
	if(buffi->getBufferInfo().elementType == TYPE_VEC4F)
	{
		elementTypeGL = GL_FLOAT;

		GUARD(
			glVertexAttribPointer(
				static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics),
				4, //design error ;( bad hardcode
				elementTypeGL, //design error ;( bad hardcode
				false, //design error ;( bad hardcode
				0,
				0
			)
		);
	}
	else
	{
		if(buffi->getBufferInfo().elementType == TYPE_VEC4UI32)
		{
			elementTypeGL = GL_INT;
		}
		else
		{
			elementTypeGL = GL_UNSIGNED_INT;
		}

		GUARD(
			glVertexAttribIPointer(
				static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics),
				4, //design error ;( bad hardcode
				elementTypeGL, //design error ;( bad hardcode
				0,
				0
			)
		);
	}



	GUARD (glEnableVertexAttribArray(static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics)) );

	unbind();
}

void VertexBasedGeometry::setIndexBuffer(BufferInterface* buffi) throw(BufferException)
{
	mIndexBuffer = buffi;
	validateBufferIntegrity();

	bind();
	buffi->bind(OPEN_GL_CONTEXT_TYPE);



}


//returns NULL if index buffer doesn't exist;
BufferInterface* VertexBasedGeometry::getIndexBuffer()
{
	return mIndexBuffer;
}

void VertexBasedGeometry::draw(
			//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
			unsigned int numInstances,
			GeometryRepresentation desiredGeomRep)

{
	bind();


	GeometryRepresentation currentGeomRep;
	if(desiredGeomRep == DEFAULT_GEOMETRY_REPRESENTATION)
		{currentGeomRep = getGeometryRepresentation();}
	else
		{currentGeomRep = desiredGeomRep;}

	GLenum drawType;
	switch(currentGeomRep)
	{
	case VERTEX_BASED_POINT_CLOUD:
		drawType = GL_POINTS;
		break;
	case VERTEX_BASED_LINES:
		drawType=GL_LINES;
	break;
	case VERTEX_BASED_LINE_STRIP:
		drawType = GL_LINE_STRIP;
	break;

	case VERTEX_BASED_TRIANGLES:
		drawType = GL_TRIANGLES;
	break;
	case VERTEX_BASED_TRIANGLES_ADJACENCY:
		drawType = GL_TRIANGLES_ADJACENCY;
	break;
	default:
		drawType = GL_POINTS;
		assert(0 && "incompatible geometry draw type");
	break;
	}

	//TODO set to instanced when rendering works
	//GUARD( glDrawElementsInstanced(drawType,mIndexBuffer->getBufferInfo().numElements,GL_UNSIGNED_INT,0,numInstances) );
	GUARD( glDrawElements(drawType,mIndexBuffer->getBufferInfo().numElements,GL_UNSIGNED_INT,0) );

	unbind();
}


void VertexBasedGeometry::validateBufferIntegrity()throw(BufferException)
{
	assert(
			getGeometryRepresentation()==VERTEX_BASED_POINT_CLOUD ||
			getGeometryRepresentation()==VERTEX_BASED_LINES ||
			getGeometryRepresentation()==VERTEX_BASED_LINE_STRIP ||
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLES ||
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLES_ADJACENCY
	);

	//made a design mistake: texelinfo make also sense in bufferinfo (then should be called "ComponentInfo"),
	//in order to set the attribute buffers accordingly; TODO refactor;

	//before refactoring, assert float, unsigned int, int, all 32 bit and fourcomponent;
	cl_GLuint numElements = 0;
	for(unsigned int i=0; i< __NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__;i++)
	{
		if(mAttributeBuffers[i])
		{
			if(numElements==0)
			{
				numElements= mAttributeBuffers[i]->getBufferInfo().numElements;
			}
			else
			{
				assert("number of elements all the same"&&
						(numElements == mAttributeBuffers[i]->getBufferInfo().numElements));
			}
			assert("must be four component" && ((mAttributeBuffers[i]->getBufferInfo().numElements % 4) == 0));
			assert( "indirectly testing for 32 bit size of single data element" &&
				( (mAttributeBuffers[i]->getBufferInfo().bufferSizeInByte /
				  (mAttributeBuffers[i]->getBufferInfo().numElements)       % (4*sizeof(float))) == 0));
			assert(
				(
						(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4F) ||
						(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4I32) ||
						(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4UI32)
				)
			&& "due to design mistake currently nothing else allowed" );
		}
	}
	if(mIndexBuffer)
	{
		assert((mIndexBuffer->getBufferInfo().elementType == TYPE_UINT32));
		assert(mIndexBuffer->getBufferInfo().glBufferType == VERTEX_INDEX_BUFFER_TYPE);
	}
}



//
//
//void VertexBasedGeometry::bindSafe()
//{
//	bind();
//	//TODO transfer from rendertarget code
//
////	glGetIntegerv(GL_ARRAY_BUFFER_BINDING,
////					&mOld);
////	glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
////					&mOldDrawBufferBinding);
////
////	if(
////		( mIsReadFrameBuffer && ( (GLuint)mOldReadBufferBinding != mFBO ) )
////		||
////		(!mIsReadFrameBuffer && ( (GLuint)mOldDrawBufferBinding != mFBO ) )
////	)
////	{
////		bind(mIsReadFrameBuffer);
////	}
//}
//
//void VertexBasedGeometry::unBindSave()
//{
//	unbind();
//	//TODO transfer from rendertarget code
////	if(
////		( mIsReadFrameBuffer && ( (GLuint)mOldReadBufferBinding != mFBO ) )
////	)
////	{
////		glBindFramebuffer(GL_READ_FRAMEBUFFER,
////				mOldReadBufferBinding);
////	}
//
//}




}
