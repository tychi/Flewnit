/*
 * VertexBasedGeometry.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "VertexBasedGeometry.h"
#include "Simulator/ParallelComputeManager.h"
#include "Buffer/BufferInterface.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "Util/Log/Log.h"
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit
{

VertexBasedGeometry::VertexBasedGeometry(String name, GeometryRepresentation geoRep)
: BufferBasedGeometry(name, geoRep), mIndexBuffer(0),
  	  mOffsetStart(0), mIndexCount(0),
  	  mOldVBOBinding(0), mBindSaveCallCounter(0),
  	  mAnyAttribBufferIsPingPong(false)
{
	GUARD( glGenVertexArrays(1, &mGLVBO));

	bindSafe();

	if(	(geoRep == VERTEX_BASED_TRIANGLE_PATCHES)||
		(geoRep == VERTEX_BASED_QUAD_PATCHES)||
		(geoRep == VERTEX_BASED_BEZIER_PATCHES)	)
	{
		setUpPatchRepresentationState();
	}

	unBindSave();
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
	bindSafe();

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

	unBindSave();
}


VertexBasedGeometry::~VertexBasedGeometry()
{
	bindSafe();
	for(int i=0; i< __NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__;i++)
	{
		if(mAttributeBuffers[i])
		{
			GUARD(glDisableVertexAttribArray(static_cast<GLuint>(mAttributeBuffers[i]->getBufferInfo().bufferSemantics)));
		}
	}
	unBindSave();

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

void  VertexBasedGeometry::bindSafe()
{
	//GLint currentVBOBinding;
	//wtf, the GL_VERTEX_ARRAY_BINDING cap is documented nowhere!
	//let's hope this works
	//GUARD( glGetIntegerv(GL_VERTEX_ARRAY_BINDING , &currentVBOBinding) );

	mBindSaveCallCounter++;

	//if( ( (GLuint)(currentVBOBinding) ) != mGLVBO )
	if(mBindSaveCallCounter == 1)
	{
		//mOldVBOBinding = currentVBOBinding;
		GUARD( glGetIntegerv(GL_VERTEX_ARRAY_BINDING , &mOldVBOBinding) );
		GUARD( glBindVertexArray(mGLVBO) );
	}
}
void  VertexBasedGeometry::unBindSave()
{
	mBindSaveCallCounter--;

	//only restore old binding if unbindSave() has been calls as often as bindSave();
	if(  mBindSaveCallCounter == 0 )
	{
		GUARD( glBindVertexArray( (GLuint)(mOldVBOBinding) ) );
	}
	//GUARD( glBindVertexArray(0) );
}




void VertexBasedGeometry::setAttributeBuffer(BufferInterface* buffi, BufferSemantics bs) throw(BufferException)
{
	bindSafe();

	BufferBasedGeometry::setAttributeBuffer(buffi);

	//validateBufferIntegrity();

	if(buffi)
	{
		mAnyAttribBufferIsPingPong = (mAnyAttribBufferIsPingPong || buffi->isPingPongBuffer() );

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
		if(buffi->getBufferInfo().elementInfo.internalGPU_DataType != GPU_DATA_TYPE_FLOAT)
		{
			if(buffi->getBufferInfo().elementInfo.internalGPU_DataType == GPU_DATA_TYPE_UINT)
			{
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
					0, //no stride, tightly packed
					0 //no offset to currently bound GL_ARRAY_BUFFER
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
					0, //no stride, tightly packed
					0 //no offset to currently bound GL_ARRAY_BUFFER
				)
			);
		}


		GUARD (glEnableVertexAttribArray(static_cast<GLuint> (buffi->getBufferInfo().bufferSemantics)) );

	}
	else
	{
		//unset attribute semantics
		GUARD ( glDisableVertexAttribArray( bs ) );
	}

	unBindSave();
}

void VertexBasedGeometry::setIndexBuffer(
	BufferInterface* buffi,
	//values for shared usage of index buffers, like for different fluids
	//indexCount = 0 indicates that no offset shall be used, so that the whole
	//attribute buffer shall be taken
	unsigned int offsetStart,
	unsigned int indexCount
	) throw(BufferException)
{
	bindSafe();

	mOffsetStart = offsetStart;
	mIndexCount = indexCount;

	mIndexBuffer = buffi;


	if(buffi)
	{
		buffi->bind(OPEN_GL_CONTEXT_TYPE);
	}
	else
	{
		//unbind index buffer
		GUARD(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
	}

	validateBufferIntegrity();

	unBindSave();

}


//returns NULL if index buffer doesn't exist;
BufferInterface* VertexBasedGeometry::getIndexBuffer()
{
	return mIndexBuffer;
}


void VertexBasedGeometry::rebindPingPongBuffers()
{
	bindSafe();

	if(mAnyAttribBufferIsPingPong)
	{
		for(unsigned int i=0; i< __NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__;i++)
		{
			if( mAttributeBuffers[i] && mAttributeBuffers[i]->isPingPongBuffer())
			{
				//re-set stuff if contents have been toggled an odd number of times
				//since the last draw call ;(
				setAttributeBuffer(mAttributeBuffers[i]);
			}
		}
	}

	if(mIndexBuffer && mIndexBuffer->isPingPongBuffer())
	{
		//re-set stuff if contents have been toggled an odd number of times
		//since the last draw call ;(
		mIndexBuffer->bind(OPEN_GL_CONTEXT_TYPE);
	}

	unBindSave();
}

void VertexBasedGeometry::draw(
			//SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
			unsigned int numInstances,
			GeometryRepresentation desiredGeomRep)

{
	bindSafe();

	rebindPingPongBuffers();


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
		assert( getGeometryRepresentation() == VERTEX_BASED_TRIANGLES ||
				getGeometryRepresentation() == VERTEX_BASED_TRIANGLE_PATCHES);

		if(ShaderManager::getInstance().tesselationIsEnabled())
		{
			drawType = GL_PATCHES;
			GUARD(glPatchParameteri(GL_PATCH_VERTICES, 3));
		}
		else
		{
			drawType = GL_TRIANGLES;
		}
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


	if(mIndexBuffer)
	{
		//use index buffer, i.e. the glDrawElementsXY() functions

		if(mIndexCount == 0)
		{
			GUARD(
				glDrawElementsInstanced(
						drawType,
						mIndexBuffer->getBufferInfo().numElements,
						GL_UNSIGNED_INT,
						0,
						numInstances
				)
			);
		}
		else
		{
			//new version to support index buffer sharing (e.g. for multiple fluids etc)
			GUARD(
				glDrawElementsInstancedBaseVertex(
					drawType,
					mIndexCount,
					GL_UNSIGNED_INT,
					0,
					numInstances,
					mOffsetStart
				)
			);
		}
	}
	else
	{
		//don't use index buffer, i.e. use the glDrawArraysXY() functions
		if(mIndexCount == 0)
		{
			//draw in whole range, from zero to element count of position buffer
			GUARD(
				glDrawArraysInstanced(
					drawType,
					0,
					mAttributeBuffers[POSITION_SEMANTICS]->getBufferInfo().numElements,
					numInstances)
			);
		}
		else
		{
			//draw in restricted range
			GUARD(
				glDrawArraysInstanced(
					drawType,
					mOffsetStart,
					mIndexCount,
					numInstances)
			);
		}

	}

	unBindSave();

}


void VertexBasedGeometry::validateBufferIntegrity()throw(BufferException)
{
	if(mIndexBuffer)
	{
		assert(	mOffsetStart + mIndexCount <= mIndexBuffer->getBufferInfo().numElements);
	}

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


			if(
				( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == POSITION_SEMANTICS )
				||
				( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == VELOCITY_SEMANTICS )
				||
				( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == FORCE_SEMANTICS )
			)
			{
				assert("must be four component" && ((mAttributeBuffers[i]->getBufferInfo().elementInfo.numChannels ) == 4));

				assert( "indirectly testing for 32 bit size of single data element" &&
					( (mAttributeBuffers[i]->getBufferInfo().bufferSizeInByte /
					  (mAttributeBuffers[i]->getBufferInfo().numElements)       % (4*sizeof(float))) == 0));

				assert(
					(
							(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4F) ||
							(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4I32) ||
							(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_VEC4UI32)
					)
				&& "due to design mistake currently nothing else allowed" //<--may 2011 : can't remember what design error that should be ;(
				);
			}
			else
			{
				if(
					( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == DENSITY_SEMANTICS )
					||
					( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == Z_INDEX_SEMANTICS )
					||
					( mAttributeBuffers[i]->getBufferInfo().bufferSemantics == MASS_SEMANTICS )
				)
				{
					assert("must be single component" && ((mAttributeBuffers[i]->getBufferInfo().elementInfo.numChannels ) == 1));

					assert( "indirectly testing for 32 bit size of single data element" &&
						( (mAttributeBuffers[i]->getBufferInfo().bufferSizeInByte /
						  (mAttributeBuffers[i]->getBufferInfo().numElements)  % (sizeof(unsigned int))) == 0));

					assert(
						(
								(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_FLOAT) ||
								(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_INT32) ||
								(mAttributeBuffers[i]->getBufferInfo().elementType == TYPE_UINT32)
						)
					);
				}
			}


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
