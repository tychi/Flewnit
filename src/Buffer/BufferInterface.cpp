/*
 * BufferInterface.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "BufferInterface.h"

#include "Common/Profiler.h"

#include "Common/Math.h"

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
{}

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
	//DON'T delete the handles; can cause conflicts with ping pong buffer, plus, the correct deletion instruction isn't known directly;
	//better assert that derived classes free the mem for themselves and set the appropriate pointers to zero;

	//some guard in order to check if the implementor of a derived class has thought about the release of the gl-object
	//(via glDeleteBuffers(), glDeleteTextures() or glDeleteRenderbuffers())
	assert("derived classes have to release the GL handle appropriately if the use it!" && mGraphicsBufferHandle==0);
	assert("derived classes have to release the CPU handle appropriately if the use it!" && mCPU_Handle==0);
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



bool BufferInterface::hasBufferInContext(ContextType type) const
{
	//return mBufferInfo->allocationGuards[type];
	return (mBufferInfo.usageContexts & FLEWNIT_FLAGIFY(type)) != 0;
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
