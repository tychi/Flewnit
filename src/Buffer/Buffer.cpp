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
{
	glGenBuffers(1, &mGraphicsBufferHandle);
}
void Buffer::generateCL()
{
	mComputeBufferHandle = cl::Buffer(
			CLMANAGER->getCLContext(),
			//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
			CL_MEM_READ_WRITE,
			mBufferInfo.bufferSizeInByte,
			NULL,
			//TODO check if adress of a reference is the same as the adress of a variable
			& CLMANAGER->getLastCLError()
	);

}
void Buffer::generateCLGL()
{
	mComputeBufferHandle = cl::BufferGL(
			CLMANAGER->getCLContext(),
			//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
			CL_MEM_READ_WRITE,
			mGraphicsBufferHandle,
			//TODO check if adress of a reference is the same as the adress of a variable
			& CLMANAGER->getLastCLError()
	);
	CLMANAGER->registerSharedBuffer(mComputeBufferHandle);
}

void Buffer::bindGL()
{
	glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle);
}


void Buffer::allocGL()
{
	glBufferData(
		//which target?
		mGlBufferTargetEnum,
		// size of storage
		mBufferInfo.bufferSizeInByte,
		//data will be passed in setData();
		NULL,
		//draw static if not modded, dynamic otherwise ;)
		mContentsAreModifiedFrequently ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}


void Buffer::writeGL(const void* data)
{
	glBufferSubData(mGlBufferTargetEnum,0,mBufferInfo.bufferSizeInByte,data);
}
void Buffer::writeCL(const void* data)
{
	CLMANAGER->getCommandQueue().enqueueWriteBuffer(
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			CLMANAGER->getBlockAfterEnqueue(),
			0,
			mBufferInfo.bufferSizeInByte,
			data,
			0,
			& CLMANAGER->getLastEvent());
}

void Buffer::readGL(void* data)
{
	glGetBufferSubData(mGlBufferTargetEnum,0,mBufferInfo.bufferSizeInByte,data);
}
void Buffer::readCL(void* data)
{
	CLMANAGER->getCommandQueue().enqueueReadBuffer(
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			CLMANAGER->getBlockAfterEnqueue(),
			0,
			mBufferInfo.bufferSizeInByte,
			data,
			0,
			& CLMANAGER->getLastEvent());
}

void Buffer::copyGL(GraphicsBufferHandle bufferToCopyContentsTo)
{}
void Buffer::copyCL(ComputeBufferHandle bufferToCopyContentsTo)
{}
void Buffer::freeGL()
{
	GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));
}

void Buffer::freeCL()
{/*do nothing*/}

void Buffer::mapGLToHost(void* data)
{}
void Buffer::mapCLToHost(void* data)
{
//	CLMANAGER->getCommandQueue().enqueueMapBuffer(
//			static_cast<cl::Buffer&>(mComputeBufferHandle),
//			CLMANAGER->getBlockAfterEnqueue(),
//			x,
//			0,
//			mBufferInfo.bufferSizeInByte,
//			0,
//			& CLMANAGER->getLastEvent(),
//			& CLMANAGER->getLastCLError()
//			);
}
void Buffer::unmapGL()
{}
void Buffer::unmapCL()
{}





}
