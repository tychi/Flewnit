/*
 * VertexBasedGeometry.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "VertexBasedGeometry.h"
#include "Simulator/OpenCL_Manager.h"
#include "Buffer/BufferInterface.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "Util/Log/Log.h"

namespace Flewnit
{

VertexBasedGeometry::VertexBasedGeometry(String name, GeometryRepresentation geoRep)
: BufferBasedGeometry(name, geoRep), mIndexBuffer(0)
{
	GUARD( glGenVertexArrays(1, &mGLVBO));
	bind();

	if(	(geoRep == VERTEX_BASED_TRIANGLE_PATCHES)||
		(geoRep == VERTEX_BASED_QUAD_PATCHES)||
		(geoRep == VERTEX_BASED_BEZIER_PATCHES)	)
	{
		setUpPatchRepresentationState();
	}
}

//VertexBasedGeometry::VertexBasedGeometry(String name, GLint verticesPerPatch)
//: BufferBasedGeometry(name, VERTEX_BASED_PATCHES), mIndexBuffer(0), mNumVerticesPerPatch(verticesPerPatch)
//{
//	GUARD( glGenVertexArrays(1, &mGLVBO));
//
//	setUpPatchRepresentationState();
//}

void VertexBasedGeometry::setUpPatchRepresentationState()
{
	if(WindowManager::getInstance().getAvailableOpenGLVersion().x < 4)
	{

		if(mGeometryRepresentation == VERTEX_BASED_TRIANGLE_PATCHES)
		{
			LOG<<WARNING_LOG_LEVEL<<"You requested a tessellated triangle geometry, but your active OpenGL version "
				    <<"doesn't support this; Hence, Tesselation will be deactivated;\n";
			mGeometryRepresentation = VERTEX_BASED_TRIANGLES;
		}
		else
		{
			LOG<<ERROR_LOG_LEVEL<<"You requested a tessellated non-triangle geometry representation,"
					<<" but your active OpenGL version "
				    <<"doesn't support this and there is no compatible fallback; aborting;\n";
			assert(0);
		}

		return;
	}

	bind();

	int numVerticesPerPatch =0;
	switch(getGeometryRepresentation())
	{
	case VERTEX_BASED_TRIANGLE_PATCHES:
		numVerticesPerPatch = 3;
	break;
	case VERTEX_BASED_QUAD_PATCHES:
		numVerticesPerPatch = 4;
		assert(0&&" sry VERTEX_BASED_QUAD_PATCHES are not supported yet");
	break;
	case VERTEX_BASED_BEZIER_PATCHES:
		numVerticesPerPatch = 16;
		assert(0&&" sry VERTEX_BASED_BEZIER_PATCHES are not supported yet");
	break;
	default:
		assert(0&&"no other patch representation supported yet ;(");
	break;
	}

	validateBufferIntegrity();
	GUARD(glPatchParameteri(GL_PATCH_VERTICES, numVerticesPerPatch));
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


	//Convention for the BufferElementInfo of a Buffer designated to be an attribute buffer:
		//if internal GPU data type is int or uint, it will always be handled as integer attributes,
		//unless the normalization flag is set; Note especcially that non-normalized int-to-float
		//conversions aren't supported this way; This is on purpose, as
		//	1.: I don't see any advantage in reading unnormalized integer values and convert
		//		them to float;
		//	2.: Control flow and usage flags to be tracked are less complex;
		//if one wants save memory, then the GL_HALF data type shall be used
		//(though I didn't test it in the glm library)
	GLenum elementTypeGL= GL_FLOAT;
	if(buffi->getBufferInfo().elementInfo.internalGPU_DataType != GPU_DATA_TYPE_FLOAT){
		if(buffi->getBufferInfo().elementInfo.internalGPU_DataType == GPU_DATA_TYPE_UINT){
			switch(buffi->getBufferInfo().elementInfo.bitsPerChannel)
			{
			case 8:  elementTypeGL= GL_UNSIGNED_BYTE; 	break;
			case 16: elementTypeGL= GL_UNSIGNED_SHORT;	break;
			case 32: elementTypeGL= GL_UNSIGNED_INT;	break;
			default: throw(BufferException("bad bits per channel")); break;
			}
		}else{ //must be signed int
			switch(buffi->getBufferInfo().elementInfo.bitsPerChannel)
			{
			case 8:  elementTypeGL= GL_BYTE; 	break;
			case 16: elementTypeGL= GL_SHORT;	break;
			case 32: elementTypeGL= GL_INT;		break;
			default: throw(BufferException("bad bits per channel")); break;
			}
		}
	} else //end "not float"
	{
		switch(buffi->getBufferInfo().elementInfo.bitsPerChannel)
		{
		case 8:  throw(BufferException("there is no 8 bit floating point type"));  	break;
		case 16: elementTypeGL= GL_HALF_FLOAT;	break;
		case 32: elementTypeGL= GL_FLOAT;		break;
		case 64: throw(BufferException("double precision floating point not supported (yet);"));		break;
		default: throw(BufferException("bad bits per channel")); break;
		}
	}



	if(
		(buffi->getBufferInfo().elementInfo.internalGPU_DataType==GPU_DATA_TYPE_FLOAT)
		||
		(buffi->getBufferInfo().elementInfo.normalizeIntegralValuesFlag)
	)
	{
		GUARD(
			glVertexAttribPointer(
				static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics),
				buffi->getBufferInfo().elementInfo.numChannels,
				elementTypeGL,
				buffi->getBufferInfo().elementInfo.normalizeIntegralValuesFlag,
				0,
				0
			)
		);
	}
	else //"real" integer stuff, both concerning storage and lookup
	{
		GUARD(
			glVertexAttribIPointer(
				static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics),
				buffi->getBufferInfo().elementInfo.numChannels,
				elementTypeGL,
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

	GLenum drawType= GL_POINTS; //most compatible value as default
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
	case VERTEX_BASED_TRIANGLE_PATCHES:
		drawType = GL_PATCHES;
		GUARD(glPatchParameteri(GL_PATCH_VERTICES, 3));
		assert(getGeometryRepresentation() == VERTEX_BASED_TRIANGLES ||
				getGeometryRepresentation() == VERTEX_BASED_TRIANGLE_PATCHES);
		break;
	case VERTEX_BASED_QUAD_PATCHES:
		assert(0&&"sry VERTEX_BASED_QUAD_PATCHES not supported yet");
		break;
	case VERTEX_BASED_BEZIER_PATCHES:
		assert(0&&"sry VERTEX_BASED_BEZIER_PATCHES not supported yet");
		break;
	default:
		drawType = GL_POINTS;
		assert(0 && "incompatible vertex based geometry draw type");
	break;
	}


	GUARD( glDrawElementsInstanced(drawType,mIndexBuffer->getBufferInfo().numElements,GL_UNSIGNED_INT,0,numInstances) );

	unbind();
}


void VertexBasedGeometry::validateBufferIntegrity()throw(BufferException)
{
	assert(
			getGeometryRepresentation()==VERTEX_BASED_POINT_CLOUD ||
			getGeometryRepresentation()==VERTEX_BASED_LINES ||
			getGeometryRepresentation()==VERTEX_BASED_LINE_STRIP ||
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLES ||
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLES_ADJACENCY ||
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLE_PATCHES
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
