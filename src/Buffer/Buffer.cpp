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


	if(mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG)
	{
		assert( "cpu buffer was allocated" && mCPU_Handle);
		free(mCPU_Handle);
		mCPU_Handle = 0;
	}

	if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		assert( "GL buffer was allocated" && mGraphicsBufferHandle);
		GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));
		mGraphicsBufferHandle = 0;
	}

	//CL stuff deletes itself;

#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	unregisterBufferAllocation(mBufferInfo.usageContexts, mBufferSizeInByte);
#endif
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

bool Buffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
{
	//TODO after alloc and setData() stuff :P
}



bool Buffer::allocMem()throw(BufferException)
{
	//assert that this routine is called only once per object:
	if(mCPU_Handle || mGraphicsBufferHandle || mComputeBufferHandle())
	{
		throw(BufferException("Buffer::allocMem(): some buffers already allocated"));
	}

	if( mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG )
	{
			mCPU_Handle = malloc(mBufferSizeInByte);
	}

	if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		//ok, there is a need for an openGL buffer; maybe it will be shared with openCL,
		//but that doesn't matter for the GL buffer creation :)
		if( mBufferInfo.glBufferType == NO_GL_BUFFER_TYPE	)
		{
			throw(BufferException("no gl buffer type specified, although a gl usage context was requested"));
		}

		GUARD(glGenBuffers(1, &mGraphicsBufferHandle));
		GUARD(glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle));

		GUARD(glBufferData(
						//which target?
						mGlBufferTargetEnum,
						// size of storage
						mBufferSizeInByte ,
						//data will be passed in setData();
						NULL,
						//draw static if not modded, dynamic otherwise ;)
						mContentsAreModifiedFrequently ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	}

	//ok, the GL stuff is allocated if it was requested; Now let's check for the "compute" world;
	if(mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG)
	{
		if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
		{
			//both CL and GL are requested, that means interop:
			GUARD(
				mComputeBufferHandle = cl::BufferGL(
					CLMANAGER->getCLContext(),
					//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
					CL_MEM_READ_WRITE,
					mGraphicsBufferHandle,
					//TODO check if adress of a reference is the same as the adress of a variable
					& CLMANAGER->getLastCLError()
					)
			);

			CLMANAGER->registerSharedBuffer(mComputeBufferHandle);
		}
		else
		{
			//a CL-only buffer is requested:
			GUARD(
					mComputeBufferHandle = cl::Buffer(
							CLMANAGER->getCLContext(),
							//TODO check performance and interface "set-ability" of CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY
							CL_MEM_READ_WRITE,
							mBufferSizeInByte,
							NULL,
							//TODO check if adress of a reference is the same as the adress of a variable
							& CLMANAGER->getLastCLError()
							)
			);
		}
	}


#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
		registerBufferAllocation(mBufferInfo.usageContexts,mBufferSizeInByte);
#endif

	return true;

}



void Buffer::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{
	//CPU
	if( where & HOST_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to cpu buffer requested, but this buffer has no CPU storage!"));}

		if(! mCPU_Handle)
		{throw(BufferException(" Buffer::setData: mCPU_Handle is NULL; some implementing of (calling)  allocMem() went terribly wrong"));}

		memcpy(mCPU_Handle,data, mBufferSizeInByte);

	}

	//GL
	if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to GL buffer requested, but this buffer has no GL storage!"));}

		//we can call this often, as a guard omits obsolete CL calls;
		CLMANAGER->acquireSharedBuffersForGraphics();
		bind(OPEN_GL_CONTEXT_TYPE);
		GUARD(
				glBufferSubData(mGlBufferTargetEnum,0,mBufferSizeInByte,data);
		);
	}
	else
	//CL; Handle this only if no GL copy was requested, as a shared buffer is sufficient to be set up
	//by one API ;)
	{
		if( where & OPEN_CL_CONTEXT_TYPE_FLAG )
		{
			if( ! (mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
			{throw(BufferException("data copy to CL buffer requested, but this buffer has no CL storage!"));}

			//it is not necessary to distinguish between a cl::Buffer and a cl::BufferGL here :).

			CLMANAGER->acquireSharedBuffersForCompute();
			GUARD(
					CLMANAGER->getCommandQueue().enqueueWriteBuffer(
							static_cast<cl::Buffer&>(mComputeBufferHandle),
							CLMANAGER->getBlockAfterEnqueue(),
							0,
							mBufferSizeInByte,
							data,
							0,
							& CLMANAGER->getLastEvent());
			);

		}
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
		LOG<<WARNING_LOG_LEVEL<<"binding a buffer to an OpenCL context makes no big sense to me at the moment ;). Try just assuring the Buffer is qcquired for the CL context and it is set as a kernel argument properly;\n";
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



}
