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
	GUARD(freeGL());
	mGraphicsBufferHandle = 0;
}


bool Buffer::operator==(const BufferInterface& rhs) const
{
	//if everything is implemented and maintained correctly, the pure compraision of the bufferinfo
	//should be enough; but don't trust the programmer, not even yourself :P
	return (//is it really of same type?
			rhs.isDefaultBuffer() &&
			mBufferInfo == rhs.getBufferInfo() );

}

const BufferInterface& Buffer::operator=(const BufferInterface& rhs) throw(BufferException)
{
	if( (*this) == rhs )
	{
		if((mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG) !=0)
		{
			memcpy(mCPU_Handle,rhs.getCPUBufferHandle(),mBufferInfo.bufferSizeInByte);
		}

		//GL
		if(
			( hasBufferInContext(OPEN_GL_CONTEXT_TYPE) && CLMANAGER->graphicsAreInControl() )
			||
			! (hasBufferInContext(OPEN_CL_CONTEXT_TYPE))
		)
		{
			//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
			//TODO uncomment when stable work is assured
			//if(isCLGLShared())
			{
				CLMANAGER->acquireSharedBuffersForGraphics();
			}
			//do a barrier in by all means to assure buffer integrity;
			CLMANAGER->barrierGraphics();

			GUARD(copyGLFrom(rhs.getGraphicsBufferHandle()));
			//return, as a shared buffer does need only copy via one context;
			return *this;
		}

		//CL
		if(
			( hasBufferInContext(OPEN_CL_CONTEXT_TYPE) && CLMANAGER->computeIsInControl() )
			||
			! (hasBufferInContext(OPEN_GL_CONTEXT_TYPE))
		)
		{
			//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
			//TODO uncomment when stable work is assured
			//if(isCLGLShared())
			{
				CLMANAGER->acquireSharedBuffersForCompute();
			}

			//do a barrier in by all means to assure buffer integrity;
			CLMANAGER->barrierCompute();
			GUARD(copyCLFrom(rhs.getComputeBufferHandle()));
			return *this;
		}

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

void Buffer::copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)
{

}
void Buffer::copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)
{
	CLMANAGER->getCommandQueue().enqueueCopyBuffer(
			static_cast<cl::Buffer&>(bufferToCopyContentsFrom),
			static_cast<cl::Buffer&>(mComputeBufferHandle),
			0,
			0,
			mBufferInfo.bufferSizeInByte,
			0,
			& CLMANAGER->getLastEvent()
			);
}


void Buffer::freeGL()
{
	GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));
}

void Buffer::freeCL()
{/*do nothing*/}




//void Buffer::mapGLToHost(void* data)
//{}
//void Buffer::mapCLToHost(void* data)
//{
////	CLMANAGER->getCommandQueue().enqueueMapBuffer(
////			static_cast<cl::Buffer&>(mComputeBufferHandle),
////			CLMANAGER->getBlockAfterEnqueue(),
////			x,
////			0,
////			mBufferInfo.bufferSizeInByte,
////			0,
////			& CLMANAGER->getLastEvent(),
////			& CLMANAGER->getLastCLError()
////			);
//}
//void Buffer::unmapGL()
//{}
//void Buffer::unmapCL()
//{}





}
