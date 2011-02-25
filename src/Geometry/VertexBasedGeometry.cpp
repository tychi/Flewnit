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

namespace Flewnit
{

VertexBasedGeometry::VertexBasedGeometry(String name, GeometryRepresentation geoRep)
: BufferBasedGeometry(name, geoRep), mIndexBuffer(0),
  //not needed
  mNumVerticesPerPatch(0)
{
	GUARD( glGenVertexArrays(1, &mGLVBO));
	bind();
}

VertexBasedGeometry::VertexBasedGeometry(String name, GLint verticesPerPatch)
: BufferBasedGeometry(name, VERTEX_BASED_PATCHES), mIndexBuffer(0), mNumVerticesPerPatch(verticesPerPatch)
{
	GUARD( glGenVertexArrays(1, &mGLVBO));

	setUpPatchRepresentationState();
}

void VertexBasedGeometry::setUpPatchRepresentationState()
{
	if(WindowManager::getInstance().getAvailableOpenGLVersion().x < 4)
	{
		//TODO
		assert(0 && "sorry, your opengl version is smaller than 4.0, and a fallback implemetation for "
				"tesselation geometry is not yet implemented; coming soon ;(");
	}

	bind();


	switch(getGeometryRepresentation())
	{
	case VERTEX_BASED_PATCHES:
	break;
	case VERTEX_BASED_LINES:
		mNumVerticesPerPatch = 2;
	break;
	case VERTEX_BASED_TRIANGLES:
		mNumVerticesPerPatch = 3;
	break;
	default:
		assert(0&&"optional patch drawing only possible with triangles and lines as original representation");
	break;
	}

	mGeometryRepresentation = VERTEX_BASED_PATCHES;
	validateBufferIntegrity();
	GUARD(glPatchParameteri(GL_PATCH_VERTICES, mNumVerticesPerPatch));
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
	case VERTEX_BASED_PATCHES:
		drawType = GL_PATCHES;
		//check for comaptibility with "base" representation and set patch vertices
		switch(getGeometryRepresentation())
		{
		case VERTEX_BASED_PATCHES:
		break;
		case VERTEX_BASED_LINES:
			GUARD(glPatchParameteri(GL_PATCH_VERTICES, 2));
		break;
		case VERTEX_BASED_TRIANGLES:
			GUARD(glPatchParameteri(GL_PATCH_VERTICES, 3));
		break;
		default:
			assert(0&&"optional patch drawing only possible with triangles and lines as original representation");
		break;
		}
		break;
	default:
		drawType = GL_POINTS;
		assert(0 && "incompatible vertex based geometry draw type");
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
			getGeometryRepresentation()==VERTEX_BASED_TRIANGLES_ADJACENCY ||
			getGeometryRepresentation()==VERTEX_BASED_PATCHES
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

	if(getGeometryRepresentation()== VERTEX_BASED_PATCHES)
	{
		assert( "only the following patch sizes are currently supported:" &&
			(
				(mNumVerticesPerPatch == 3)	||
				(mNumVerticesPerPatch == 4) ||
				(mNumVerticesPerPatch == 16)
			)

		);
	}



	if(mIndexBuffer)
	{
		assert((mIndexBuffer->getBufferInfo().elementType == TYPE_UINT32));
		assert(mIndexBuffer->getBufferInfo().glBufferType == VERTEX_INDEX_BUFFER_TYPE);

		if(getGeometryRepresentation()== VERTEX_BASED_PATCHES)
		{
			assert( "index count multiple of patch size" &&  (mIndexBuffer->getBufferInfo().numElements % mNumVerticesPerPatch  == 0 ));
		}
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
