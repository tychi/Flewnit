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
}







void Buffer::bind(ContextType type)
{
	if(type == OPEN_GL_CONTEXT_TYPE)
	{
		CLMANAGER->acquireSharedBuffersForGraphics();
		GUARD(glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle););
		return;
	}
	if(type == OPEN_CL_CONTEXT_TYPE)
	{
		LOG<<WARNING_LOG_LEVEL<<"binding a buffer to an OpenCL context makes no big sense to me at the moment ;). Try just assuring the Buffer is aqcquired for the CL context and it is set as a kernel argument properly;\n";
		CLMANAGER->acquireSharedBuffersForCompute();
		return;
	}

	if(type == HOST_CONTEXT_TYPE)
	{
		assert("binding a buffer to the host context makes no sense to me at the moment ;)"&&0);
		return;
	}

	assert("should never end up here" && 0);

}

virtual void Buffer::generateGL()
{}
virtual void Buffer::generateCL();
virtual void Buffer::bindGL();
virtual void Buffer::bindCL();
virtual void Buffer::allocGL();
virtual void Buffer::allocCL();
virtual void Buffer::writeGL(const void* data);
virtual void Buffer::writeCL(const void* data);
virtual void Buffer::readGL(void* data);
virtual void Buffer::readCL(void* data);
virtual void Buffer::freeGL()
{GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));}
virtual void Buffer::freeCL()
{/*do nothing*/}



}
