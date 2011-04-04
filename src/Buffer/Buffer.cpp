/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"
#include "Util/Log/Log.h"
#include "Simulator/ParallelComputeManager.h"



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
	 //mBufferInfo=new BufferInfo(buffi);

	switch(mBufferInfo->glBufferType)
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
		setData(data,mBufferInfo->usageContexts);
	}
}

Buffer::~Buffer()
{

	//	mComputeBufferHandle.release();
	//	mComputeBufferHandle()=0;

	//gl stuff is deleted BEFORE CL stuff, that migth couse problems;
	//but when call mComputeBufferHandle.release() exdplicitely before,
	//then there will be an error when the destructor of the cl::mem object
	//is called during BufferInterface destruction;
	//so, no amtter what we do, there might be an error, unless we make the member a pointer;
	//but that doesn't seem appropriate to me (yet);
	//TODO check out what happens during buffer deletion;
	GUARD(freeGL());
	mGraphicsBufferHandle = 0;
}


bool Buffer::operator==(const BufferInterface& rhs) const
{
	//if everything is implemented and maintained correctly, the pure compraision of the bufferinfo
	//should be enough; but don't trust the programmer, not even yourself :P
	return (//is it really of same type?
			rhs.isDefaultBuffer() &&
			*mBufferInfo == rhs.getBufferInfo() );

}






//------------------------------------------------------
//wrapper functions to GL and CL calls without any error checking,
//i.e. semantic checks/flag delegation/verifiaction must be done before those calls;
//those routines are introduced to reduce boilerplate code;
void Buffer::generateGL()throw(BufferException)
{
	glGenBuffers(1, &mGraphicsBufferHandle);
}
void Buffer::generateCL()throw(BufferException)
{
	mComputeBufferHandle = cl::Buffer(
			PARA_COMP_MANAGER->getCLContext(),
			//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
			CL_MEM_READ_WRITE,
			mBufferInfo->bufferSizeInByte,
			NULL,
			//TODO check if adress of a reference is the same as the adress of a variable
			& PARA_COMP_MANAGER->getLastCLError()
	);

}
void Buffer::generateCLGL()throw(BufferException)
{
	mComputeBufferHandle = cl::BufferGL(
			PARA_COMP_MANAGER->getCLContext(),
			//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
			CL_MEM_READ_WRITE,
			mGraphicsBufferHandle,
			//TODO check if adress of a reference is the same as the adress of a variable
			& PARA_COMP_MANAGER->getLastCLError()
	);
	PARA_COMP_MANAGER->registerSharedBuffer(mComputeBufferHandle);
}

void Buffer::bindGL()throw(BufferException)
{
	glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle);
}


void Buffer::allocGL()throw(BufferException)
{
	glBufferData(
		//which target?
		mGlBufferTargetEnum,
		// size of storage
		mBufferInfo->bufferSizeInByte,
		//data will be passed in setData();
		NULL,
		//draw static if not modded, dynamic otherwise ;)
		mContentsAreModifiedFrequently ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}


void Buffer::writeGL(const void* data)throw(BufferException)
{
	glBufferSubData(mGlBufferTargetEnum,0,mBufferInfo->bufferSizeInByte,data);
}
void Buffer::writeCL(const void* data)throw(BufferException)
{
	PARA_COMP_MANAGER->getCommandQueue().enqueueWriteBuffer(
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			PARA_COMP_MANAGER->getBlockAfterEnqueue(),
			0,
			mBufferInfo->bufferSizeInByte,
			data,
			0,
			& PARA_COMP_MANAGER->getLastEvent());
}

void Buffer::readGL(void* data)throw(BufferException)
{
	glGetBufferSubData(mGlBufferTargetEnum,0,mBufferInfo->bufferSizeInByte,data);
}
void Buffer::readCL(void* data)throw(BufferException)
{
	PARA_COMP_MANAGER->getCommandQueue().enqueueReadBuffer(
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			PARA_COMP_MANAGER->getBlockAfterEnqueue(),
			0,
			mBufferInfo->bufferSizeInByte,
			data,
			0,
			& PARA_COMP_MANAGER->getLastEvent());
}

void Buffer::copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	//bind other buffer as read target
	GUARD(glBindBuffer(mGlBufferTargetEnum, bufferToCopyContentsFrom););
	//bind own buffer as write target;
	GUARD(glBindBuffer(GL_COPY_WRITE_BUFFER, mGraphicsBufferHandle););

	GUARD(glCopyBufferSubData(
			mGlBufferTargetEnum, //operator==() asserts that buffer types are equal, so mGlBufferTargetEnum == rhs.mGlBufferTargetEnum
			GL_COPY_WRITE_BUFFER,
			0,
			0,
			mBufferInfo->bufferSizeInByte);
	);

}
void Buffer::copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	PARA_COMP_MANAGER->getCommandQueue().enqueueCopyBuffer(
			static_cast<cl::Buffer&>(bufferToCopyContentsFrom),
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			0,
			0,
			mBufferInfo->bufferSizeInByte,
			0,
			& PARA_COMP_MANAGER->getLastEvent()
			);
}


void Buffer::freeGL()throw(BufferException)
{
	GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));
}

void Buffer::freeCL()throw(BufferException)
{/*do nothing*/}




//void Buffer::mapGLToHost(void* data)
//{}
//void Buffer::mapCLToHost(void* data)
//{
////	PARA_COMP_MANAGER->getCommandQueue().enqueueMapBuffer(
////			static_cast<cl::Buffer&>(mComputeBufferHandle),
////			PARA_COMP_MANAGER->getBlockAfterEnqueue(),
////			x,
////			0,
////			mBufferInfo->bufferSizeInByte,
////			0,
////			& PARA_COMP_MANAGER->getLastEvent(),
////			& PARA_COMP_MANAGER->getLastCLError()
////			);
//}
//void Buffer::unmapGL()
//{}
//void Buffer::unmapCL()
//{}





}
