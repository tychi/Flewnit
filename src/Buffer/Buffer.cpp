/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"
#include "Util/Log/Log.h"
#include "Simulator/OpenCL_Manager.h"



namespace Flewnit
{

Buffer::Buffer(
		const BufferInfo& buffi,
		//normally, a vertex attribute buffer is seldom modified; but for the special case of particle simulation via ocl and point rendering via ogl,
		//the pasition and pressure etc. buffer will be completely updated every frame;
		bool contentsAreModifiedFrequently,
		//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
		//the caller is responsible of the deletion of the data pointer;
		const void* data)
: BufferInterface(buffi),
  mContentsAreModifiedFrequently(contentsAreModifiedFrequently)
{
	switch(mBufferInfo.glBufferType)
	{
	case 	NO_GL_BUFFER_TYPE :
		mGlBufferTargetEnum = 0;
		break;
	case 	VERTEX_ATTRIBUTE_BUFFER_TYPE :
		mGlBufferTargetEnum= GL_ARRAY_BUFFER;
		break;
	case 	VERTEX_INDEX_BUFFER_TYPE :
		mGlBufferTargetEnum = GL_ELEMENT_ARRAY_BUFFER;
		break;
	case 	UNIFORM_BUFFER_TYPE :
		mGlBufferTargetEnum = GL_UNIFORM_BUFFER;
		break;
	default:
		assert( "no other GL buffer stuff implemented yet ;(" && 0);
	};

	mBufferSizeInByte = mBufferInfo.numElements * BufferHelper::elementSize(mBufferInfo.elementType);



	allocMem();

	if(data)
	{
		setData(data,mBufferInfo.usageContexts);
	}
}

Buffer::~Buffer()
{

}


bool Buffer::operator==(const BufferInterface& rhs) const
{
	//if everything is implemented and maintained correctly, the pure compraision of the bufferinfo
	//should be enough; but don't trust the programmer, not even yourself :P
	return (mBufferInfo == rhs.getBufferInfo()
			//is it really of same type?
			&& dynamic_cast<const Buffer*>(&rhs));

}

const BufferInterface& Buffer::operator=(const BufferInterface& rhs) throw(BufferException)
{
	if( (*this) == rhs )
	{
		//TODO after alloc and setData() stuff :P
	}
	else
	{
		throw(BufferException("Buffer::operator= : Buffers not compatible"));
	}

	return *this;
}






//------------------------------------------------------
//wrapper functions to GL and CL calls without any error checking,
//i.e. semantic checks/flag delegation/verifiaction must be done before those calls;
//those routines are introduced to reduce boilerplate code;
void Buffer::generateGL()
{}
void Buffer::generateCL()
{}
void Buffer::bindGL()
{GUARD(glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle););}
//empty on purpose
void Buffer::bindCL()
{}
void Buffer::allocGL()
{}
void Buffer::allocCL()
{}
void Buffer::writeGL(const void* data)
{}
void Buffer::writeCL(const void* data)
{}
void Buffer::readGL(void* data)
{}
void Buffer::readCL(void* data)
{}
void Buffer::copyGL(GraphicsBufferHandle bufferToCopyContentsTo)
{}
void Buffer::copyCL(ComputeBufferHandle bufferToCopyContentsTo)
{}
void Buffer::freeGL()
{GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));}
void Buffer::freeCL()
{/*do nothing*/}
void Buffer::mapGLToHost(void* data)
{}
void Buffer::mapCLToHost(void* data)
{}
void Buffer::unmapGL()
{}
void Buffer::unmapCL()
{}





}
