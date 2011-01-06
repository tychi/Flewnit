/*
 * BufferInterface.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "BufferInterface.h"

#include "Common/Profiler.h"

#include "Common/Math.h"

#include "Util/Log/Log.h"
#include "Simulator/OpenCL_Manager.h"

//for casting:
///\{
#include "PingPongBuffer.h"
#include "Buffer.h"

#include "Texture.h"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "RenderBuffer.h"
///\}


namespace Flewnit
{




BufferInfo::BufferInfo(String name,
		ContextTypeFlags usageContexts,
		BufferSemantics bufferSemantics,
		Type elementType,
		cl_GLuint numElements,
		GLBufferType glBufferType,
		ContextType mappedToCPUContext)
	: name(name),
	  usageContexts(usageContexts),
	  bufferSemantics(bufferSemantics),
	  elementType(elementType),
	  numElements(numElements),
	  glBufferType(glBufferType),
	  isPingPongBuffer(false),
	  mappedToCPUContext(mappedToCPUContext)
{
	bufferSizeInByte = numElements * BufferHelper::elementSize(elementType);
}

BufferInfo::BufferInfo(const BufferInfo& rhs)
{
	(*this) = rhs;
}

BufferInfo::~BufferInfo()
{

}

bool BufferInfo::operator==(const BufferInfo& rhs) const
{
		return
				//NOT name equality, this doesn't matter, in the contrary, the names should be unique for convenient referancation in the data base!
				name==rhs.name	&&
				usageContexts==rhs.usageContexts &&
				bufferSemantics==rhs.bufferSemantics &&
				elementType==rhs.elementType &&
				numElements==rhs.numElements &&
				glBufferType==rhs.glBufferType &&
				isPingPongBuffer==rhs.isPingPongBuffer &&
				mappedToCPUContext==rhs.mappedToCPUContext
				;
}

const BufferInfo& BufferInfo::operator=(const BufferInfo& rhs)
{
	name=rhs.name; //TODO check if name copying has side effects;
	usageContexts=rhs.usageContexts;
	bufferSemantics=rhs.bufferSemantics;
	elementType=rhs.elementType;
	numElements=rhs.numElements;
	glBufferType=rhs.glBufferType;
	isPingPongBuffer=rhs.isPingPongBuffer;
	mappedToCPUContext=rhs.mappedToCPUContext;

	return *this;
}


//----------------------------------------------------------------

TextureInfo::TextureInfo(
		cl_GLuint dimensionality,
		Vector3Dui dimensionExtends,

		GLenum textureTarget,
		GLint imageInternalChannelLayout,
		GLenum imageInternalDataType,

		GLint numMultiSamples,
		bool isMipMapped
		)
: dimensionality(dimensionality),
  dimensionExtends(dimensionExtends),
  textureTarget(textureTarget),
  imageInternalChannelLayout(imageInternalChannelLayout),
  imageInternalDataType(imageInternalDataType),
  numMultiSamples(numMultiSamples),
  isMipMapped(isMipMapped)
{}

TextureInfo::TextureInfo(const TextureInfo& rhs)
{
	(*this) = rhs;
}

TextureInfo::~TextureInfo()
{}


bool TextureInfo::operator==(const TextureInfo& rhs) const
{
	return
		dimensionality==rhs.dimensionality &&
		glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
//		dimensionExtends.x==rhs.dimensionExtends.x &&
//		dimensionExtends.y==rhs.dimensionExtends.y &&
//		dimensionExtends.z==rhs.dimensionExtends.z &&
		textureTarget==rhs.textureTarget &&
		imageInternalChannelLayout==rhs.imageInternalChannelLayout &&
		imageInternalDataType==rhs.imageInternalDataType &&
		numMultiSamples==rhs.numMultiSamples &&
		isMipMapped==rhs.isMipMapped
		;
}

const TextureInfo& TextureInfo::operator=(const TextureInfo& rhs)
{
	dimensionality=rhs.dimensionality;
	dimensionExtends=rhs.dimensionExtends ;
	textureTarget=rhs.textureTarget ;
	imageInternalChannelLayout=rhs.imageInternalChannelLayout;
	imageInternalDataType=rhs.imageInternalDataType;
	numMultiSamples=rhs.numMultiSamples;
	isMipMapped=rhs.isMipMapped;

	return *this;
}



//----------------------------------------------------------------


BufferInterface::BufferInterface(const BufferInfo& buffi)
:mBufferInfo(buffi), mCPU_Handle(0), mGraphicsBufferHandle(0)
{
	//the compute-handle manages its initialization for itself due to the cl-c++-bindings :)
}

BufferInterface::~BufferInterface()
{
	if(mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG)
	{
		if(mCPU_Handle)
			free(mCPU_Handle);
	}

	if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		assert("due to virtual function call are the derived classes responsible for deletion of their GL buffers"
				&& (mGraphicsBufferHandle == 0));
	}

	//CL stuff deletes itself;

#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	unregisterBufferAllocation(mBufferInfo.usageContexts, mBufferInfo.bufferSizeInByte);
#endif

}


void BufferInterface::bind(ContextType type)throw(BufferException)
{
	if(type == OPEN_GL_CONTEXT_TYPE)
	{
		if( (mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG) == 0)
		{throw(BufferException("BufferInterface::bind: GL binding requested, but this Buffer has no GL context;"));}
		CLMANAGER->acquireSharedBuffersForGraphics();
		GUARD(bindGL());
		return;
	}
	if(type == OPEN_CL_CONTEXT_TYPE)
	{
		if(  (mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG) == 0)
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

	assert("should never end down here" && 0);

}


bool BufferInterface::allocMem()throw(BufferException)
{
	//assert that this routine is called only once per object:
	if(mCPU_Handle || mGraphicsBufferHandle || mComputeBufferHandle())
	{
		throw(BufferException("Buffer::allocMem(): some buffers already allocated"));
	}

	if( mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG )
	{
			mCPU_Handle = malloc(mBufferInfo.bufferSizeInByte);
	}

	if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		//ok, there is a need for an openGL buffer; maybe it will be shared with openCL,
		//but that doesn't matter for the GL buffer creation :)
		if( isDefaultBuffer() && (mBufferInfo.glBufferType == NO_GL_BUFFER_TYPE)	)
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
	if(mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG)
	{
		if(mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
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
		registerBufferAllocation(mBufferInfo.usageContexts,mBufferInfo.bufferSizeInByte);
#endif

	return true;

}


void BufferInterface::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{
	//CPU
	if( where & HOST_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to cpu buffer requested, but this buffer has no CPU storage!"));}

		if(! mCPU_Handle)
		{throw(BufferException(" Buffer::setData: mCPU_Handle is NULL; some implementing of (calling)  allocMem() went terribly wrong"));}

		memcpy(mCPU_Handle,data, mBufferInfo.bufferSizeInByte);
	}

	//GL
	if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG))
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
			if( ! (mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
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

void BufferInterface::readBack()throw(BufferException)
{
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

		GUARD(readGL(mCPU_Handle));
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

		GUARD(readCL(mCPU_Handle));
		return;
	}

	throw(BufferException("BufferInterface::readBack(): need at least one GL or GL usage context in Buffer"));
}





//get the bufferinfo directly:
const BufferInfo& BufferInterface::getBufferInfo() const
{
	return mBufferInfo;
}


//convenience functions to access bufferInfo data;

ContextTypeFlags BufferInterface::getContextTypeFlags()const
{
	return mBufferInfo.usageContexts;
}

bool BufferInterface::hasBufferInContext(ContextType type) const
{
	//return mBufferInfo->allocationGuards[type];
	return (mBufferInfo.usageContexts & FLEWNIT_FLAGIFY(type)) != 0;
}

bool BufferInterface::isCLGLShared()const
{
	return
			((mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG) != 0)
			&&
			((mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG) != 0);
}



String BufferInterface::getName() const
{
	return mBufferInfo.name;
}




int  BufferInterface::getNumElements() const
{
	return mBufferInfo.numElements;
}


size_t  BufferInterface::getElementSize() const
{
	return BufferHelper::elementSize( mBufferInfo.elementType );
}


Type BufferInterface::getElementType() const
{
	return mBufferInfo.elementType;
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
	return mBufferInfo.isPingPongBuffer;
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

bool BufferInterface::isRenderBuffer() const
{
	return 	dynamic_cast<const RenderBuffer*>(this) ? true : false;
}

RenderBuffer& BufferInterface::toRenderBuffer() throw(BufferException)
{
	RenderBuffer* toCastPtr = dynamic_cast<RenderBuffer*> (this);
	if (toCastPtr)
		return *toCastPtr;
	else
		throw(BufferException("Bad cast to RenderBuffer"));
}

const CPUBufferHandle BufferInterface::getCPUBufferHandle()const{return mCPU_Handle;}
GraphicsBufferHandle BufferInterface::getGraphicsBufferHandle()const{return mGraphicsBufferHandle;}
ComputeBufferHandle BufferInterface::getComputeBufferHandle()const{return mComputeBufferHandle;}




}
