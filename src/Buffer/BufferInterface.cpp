/*
 * BufferInterface.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "BufferInterface.h"

#include "Buffer/BufferHelperUtils.h"

#include "Common/Profiler.h"

#include "Common/Math.h"

#include "Util/Log/Log.h"
#include "Simulator/OpenCL_Manager.h"

//for casting:
///\{
#include "PingPongBuffer.h"
#include "Buffer.h"

#include "Texture.h"
#include "RenderBuffer.h"
///\}

#include "Simulator/SimulationResourceManager.h"


namespace Flewnit
{




BufferInterface::BufferInterface(const BufferInfo& buffi)
:mBufferInfo(0), mCPU_Handle(0), mGraphicsBufferHandle(0)
{
	if(dynamic_cast<const TextureInfo*>(&buffi) )
	{
		mBufferInfo= new TextureInfo( dynamic_cast<const TextureInfo&>(buffi) );
	}
	else
	{
		mBufferInfo = new BufferInfo(buffi);
	}
	//the compute-handle manages its initialization for itself due to the cl-c++-bindings :)

	SimulationResourceManager::getInstance().registerBufferInterface(this);
}

BufferInterface::~BufferInterface()
{
	if(mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG)
	{
		if(mCPU_Handle)
			free(mCPU_Handle);
	}

	if(mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		assert("due to virtual function call are the derived classes responsible for deletion of their GL buffers"
				&& (mGraphicsBufferHandle == 0));
	}

	//CL stuff deletes itself;

#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	//only track memory for non-pingpongs, as pingpongs only manage, but don't "own" own data store;
	if(! mBufferInfo->isPingPongBuffer)
	{
		unregisterBufferAllocation(mBufferInfo->usageContexts, mBufferInfo->bufferSizeInByte);
	}
#endif

	delete mBufferInfo;

}


const BufferInterface& BufferInterface::operator=(const BufferInterface& rhs) throw(BufferException)
{
	//the buffers must match exactly in all their meta-info in order to be securely copied
	//OpenGL and OpenCL are less restrictve than me, but in this case, I trade flexibility
	//for simplicity and robustness;
	if( (*this) == rhs )
	{
		if((mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG) !=0)
		{
			memcpy(mCPU_Handle,rhs.getCPUBufferHandle(),mBufferInfo->bufferSizeInByte);
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



void BufferInterface::bind(ContextType type)throw(BufferException)
{
	if(type == OPEN_GL_CONTEXT_TYPE)
	{
		if( (mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG) == 0)
		{throw(BufferException("BufferInterface::bind: GL binding requested, but this Buffer has no GL context;"));}
		CLMANAGER->acquireSharedBuffersForGraphics();
		GUARD(bindGL());
		return;
	}
	if(type == OPEN_CL_CONTEXT_TYPE)
	{
		if(  (mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG) == 0)
				{throw(BufferException("BufferInterface::bind: CL binding requested, but this Buffer has no CL context;"));}
		LOG<<WARNING_LOG_LEVEL<<"binding a buffer to an OpenCL context makes no big sense to me at the moment ;). Try just assuring "<<
				"the Buffer is acquired for the CL context and it is set as a kernel argument properly;\n";
		CLMANAGER->acquireSharedBuffersForCompute();
		//GUARD(bindCL()); <-- bullshat :P
		return;
	}

	if(type == HOST_CONTEXT_TYPE)
	{
		assert("binding a buffer to the host context makes no sense to me at the moment ;)"&&0);
		return;
	}

	assert("should never end dowglBlitFramebuffern here" && 0);

}


bool BufferInterface::allocMem()throw(BufferException)
{
	//assert that this routine is called only once per object:
	if(mCPU_Handle || mGraphicsBufferHandle || mComputeBufferHandle())
	{
		throw(BufferException("Buffer::allocMem(): some buffers already allocated"));
	}

	if( mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG )
	{
		mCPU_Handle = malloc(mBufferInfo->bufferSizeInByte);
	}

	if(mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		//ok, there is a need for an openGL buffer; maybe it will be shared with openCL,
		//but that doesn't matter for the GL buffer creation :)
		if( isDefaultBuffer() && (mBufferInfo->glBufferType == NO_GL_BUFFER_TYPE)	)
		{
			throw(BufferException("no gl buffer type specified for a non-texture or non-renderbuffer Buffer, although a gl usage context was requested"));
		}
		//no special treatment for texture types, as we use native GL-#defines

		GUARD(generateGL());
		//"direct" call of "bindGL()" here isn't dangerous, as the buffer is not shared (yet),
		//as it has just been created;
		GUARD(bindGL());
		GUARD(allocGL());
	}

	//ok, the GL stuff is allocated if it was requested; Now let's check for the "compute" world;
	if(mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG)
	{
		if(mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
		{
			//both CL and GL are requested, that means interop:
			//neither bind nor alloc necessary, just generating:
			GUARD(generateCLGL());

		}
		else
		{
			//a CL-only buffer is requested:
			//in OpenCL, alloc is done at the same time of generation; so, no allocCL() routine must be called
			GUARD(generateCL());
			//GUARD(allocCL()); <--bullshaat ;)
		}
	}


#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	//only track memory for non-pingpongs, as pingpongs only manage, but don't "own" own data store;
	if(! mBufferInfo->isPingPongBuffer)
	{
		registerBufferAllocation(mBufferInfo->usageContexts,mBufferInfo->bufferSizeInByte);
	}
#endif

	return true;

}




void BufferInterface::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{
	//CPU
	if( where & HOST_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to cpu buffer requested, but this buffer has no CPU storage!"));}

		if(! mCPU_Handle)
		{throw(BufferException(" Buffer::setData: mCPU_Handle is NULL; some implementing of (calling)  allocMem() went terribly wrong"));}

		memcpy(mCPU_Handle,data, mBufferInfo->bufferSizeInByte);
	}

	//GL
	if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to GL buffer requested, but this buffer has no GL storage!"));}


		//omit possible cl-release call where it is'nt necessary;
		//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
		//TODO uncomment when stable work is assured
		//if(isCLGLShared())
		{

			CLMANAGER->acquireSharedBuffersForGraphics();
		}

		bind(OPEN_GL_CONTEXT_TYPE);

		GUARD(writeGL(data));
	}
	else
	//CL; Handle this only if no GL copy was requested, as a shared buffer is sufficient to be set up
	//by one API ;)
	{
		if( where & OPEN_CL_CONTEXT_TYPE_FLAG )
		{
			if( ! (mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
			{throw(BufferException("data copy to CL buffer requested, but this buffer has no CL storage!"));}

			//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
			//TODO uncomment when stable work is assured
			//if(isCLGLShared())
			{
				CLMANAGER->acquireSharedBuffersForCompute();
			}

			//it is not necessary to distinguish between a cl::Buffer and a cl::BufferGL here :).
			GUARD(writeCL(data));

		}
	}
}



//---------------------------------------------------------------------------------------------------------
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)

void BufferInterface::registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte)
{
	Profiler::getInstance().registerBufferAllocation(contextTypeFlags,sizeInByte);
}

void BufferInterface::unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte)
{
	Profiler::getInstance().unregisterBufferAllocation(contextTypeFlags,sizeInByte);
}

#endif
//---------------------------------------------------------------------------------------------------------


//if both CL and GL are enabled, then the buffer is shared and the implementation
//will decide, which api will be used for the write;
void BufferInterface::copyFromHostToGPU()throw(BufferException)
{
	assert( "CPU buffer must exist for copy from host to GPU " && mCPU_Handle);

	transferData(true);
}


void BufferInterface::readBack()throw(BufferException)
{
	assert( "CPU buffer must exist for readback" && mCPU_Handle);

	transferData(false);

//	if(
//		( hasBufferInContext(OPEN_GL_CONTEXT_TYPE) && CLMANAGER->graphicsAreInControl() )
//		||
//		! (hasBufferInContext(OPEN_CL_CONTEXT_TYPE))
//	)
//	{
//		//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
//		//TODO uncomment when stable work is assured
//		//if(isCLGLShared())
//		{
//			CLMANAGER->acquireSharedBuffersForGraphics();
//		}
//
//		GUARD(readGL(mCPU_Handle));
//		return;
//	}
//
//	if(
//		( hasBufferInContext(OPEN_CL_CONTEXT_TYPE) && CLMANAGER->computeIsInControl() )
//		||
//		! (hasBufferInContext(OPEN_GL_CONTEXT_TYPE))
//	)
//	{
//		//commented out the guard in case of driver bugs fu**ing up when doing too mush time-shared CL-GL-stuff
//		//TODO uncomment when stable work is assured
//		//if(isCLGLShared())
//		{
//			CLMANAGER->acquireSharedBuffersForCompute();
//		}
//
//		GUARD(readCL(mCPU_Handle));
//		return;
//	}
//
//	throw(BufferException("BufferInterface::readBack(): need at least one GL or GL usage context in Buffer"));
}

void BufferInterface::transferData(bool fromSystemToDevice)throw(BufferException)
{
	assert( "CPU buffer must exist for transfer between host and device" && mCPU_Handle);

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

		GUARD(bindGL());
		if(fromSystemToDevice)
		{
			GUARD(writeGL(mCPU_Handle));
		}
		else
		{
			GUARD(readGL(mCPU_Handle));
		}
		return;
	}

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

		if(fromSystemToDevice)
		{
			GUARD(writeCL(mCPU_Handle));
		}
		else
		{
			GUARD(readCL(mCPU_Handle));
		}
		return;
	}

	throw(BufferException("BufferInterface::readBack(): need at least one GL or GL usage context in Buffer"));
}



//get the bufferinfo directly:
const BufferInfo& BufferInterface::getBufferInfo() const
{
	return *mBufferInfo;
}


//convenience functions to access bufferInfo data;

ContextTypeFlags BufferInterface::getContextTypeFlags()const
{
	return mBufferInfo->usageContexts;
}

bool BufferInterface::hasBufferInContext(ContextType type) const
{
	//return mBufferInfo->allocationGuards[type];
	return (mBufferInfo->usageContexts & FLEWNIT_FLAGIFY(type)) != 0;
}

bool BufferInterface::isCLGLShared()const
{
	return
			((mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG) != 0)
			&&
			((mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG) != 0);
}



String BufferInterface::getName() const
{
	return mBufferInfo->name;
}




int  BufferInterface::getNumElements() const
{
	return mBufferInfo->numElements;
}


size_t  BufferInterface::getElementSize() const
{
	return BufferHelper::elementSize( mBufferInfo->elementType );
}


Type BufferInterface::getElementType() const
{
	return mBufferInfo->elementType;
}

bool BufferInterface::isDefaultBuffer()const
{
	return 	dynamic_cast<const Buffer*>(this) ? true : false;
}

Buffer& BufferInterface::toDefaultBuffer()throw(BufferException)
{
	Buffer* toCastPtr = dynamic_cast<Buffer*>(this);
	if(toCastPtr) return *toCastPtr;
	else throw(BufferException("Bad cast to default Buffer"));
}


//convenience caster methods:
bool BufferInterface::isPingPongBuffer()const
{
	return mBufferInfo->isPingPongBuffer;
}

PingPongBuffer& BufferInterface::toPingPongBuffer() throw(BufferException)
{
	PingPongBuffer* toCastPtr = dynamic_cast<PingPongBuffer*>(this);
	if(toCastPtr) return *toCastPtr;
	else throw(BufferException("Bad cast to PingPongBuffer"));
}


bool BufferInterface::isTexture() const
{
	return 	dynamic_cast<const Texture*>(this) ? true : false;
}
bool BufferInterface::isTexture1D() const
{
	return 	dynamic_cast<const Texture1D*>(this) ? true : false;
}

Texture1D& BufferInterface::toTexture1D() throw(BufferException)
{
	Texture1D* toCastPtr = dynamic_cast<Texture1D*>(this);
	if(toCastPtr) return *toCastPtr;
	else throw(BufferException("Bad cast to Texture1D"));
}

bool BufferInterface::isTexture2D() const
{
	return 	dynamic_cast<const Texture2D*>(this) ? true : false;
}

Texture2D& BufferInterface::toTexture2D() throw(BufferException)
{
	Texture2D* toCastPtr = dynamic_cast<Texture2D*> (this);
	if (toCastPtr)
		return *toCastPtr;
	else
		throw(BufferException("Bad cast to Texture2D"));
}

bool BufferInterface::isTexture3D() const
{
	return 	dynamic_cast<const Texture3D*>(this) ? true : false;
}
Texture3D& BufferInterface::toTexture3D() throw(BufferException)
{
	Texture3D* toCastPtr = dynamic_cast<Texture3D*> (this);
	if (toCastPtr)
		return *toCastPtr;
	else
		throw(BufferException("Bad cast to Texture3D"));
}

//bool BufferInterface::isRenderBuffer() const
//{
//	return 	dynamic_cast<const RenderBuffer*>(this) ? true : false;
//}
//
//RenderBuffer& BufferInterface::toRenderBuffer() throw(BufferException)
//{
//	RenderBuffer* toCastPtr = dynamic_cast<RenderBuffer*> (this);
//	if (toCastPtr)
//		return *toCastPtr;
//	else
//		throw(BufferException("Bad cast to RenderBuffer"));
//}

CPUBufferHandle BufferInterface::getCPUBufferHandle()const throw(BufferException)
{
	if((mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG) == 0)
		throw(BufferException("BufferInterface::getCPUBufferHandle: buffer has no CPU attachment"));

	return mCPU_Handle;
}
GraphicsBufferHandle BufferInterface::getGraphicsBufferHandle()const  throw(BufferException)
{
	if((mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG) == 0)
			throw(BufferException("BufferInterface::getGraphicsBufferHandle: buffer has no GL attachment"));
	return mGraphicsBufferHandle;
}
ComputeBufferHandle BufferInterface::getComputeBufferHandle()const  throw(BufferException)
{
	if((mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG) == 0)
			throw(BufferException("BufferInterface::getComputeBufferHandle: buffer has no CL attachment"));

	return mComputeBufferHandle;
}




}
