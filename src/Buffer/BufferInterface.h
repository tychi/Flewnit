/*
 * BufferInterface.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief The base class of all buffers;
 *
 */

#pragma once

//TODO from/to typeid stuff

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/BasicObject.h"

#include "Buffer/BufferSharedDefinitions.h"




#include <exception>
#include <typeinfo>



namespace Flewnit
{





class BufferInterface
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	//planned usage must be determined in the beginning
	explicit BufferInterface(const BufferInfo& buffi);
	virtual ~BufferInterface();


	//check for campatibility: not the contents, but the types, dimensions, allocations etc are compared;
	virtual bool operator==(const BufferInterface& rhs) const = 0;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type, buffer type, same size, element type, dimensions etc;
	const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);

	void bind(ContextType type)throw(BufferException);

	//memory manipulating stuff:
	///\{
	// memory must be allocated before (done by constructors of derived classes), else exception;
	//convention: copy data into cpu buffer if cpu buffer is used and allocated;
	//throw exception if cpu context is specified in flags, but not used by the buffer object;
	//the context flags are provided to omit unnecessary copies, e.g. when fresh data is only needed by one "context"
	void setData(const void* data, ContextTypeFlags where)throw(BufferException);

	void readBack()throw(BufferException);

	void* mapBuffer()throw(BufferException)
	{
		//TODO implement when needed
		throw(BufferException("mapCPUAdressSpaceTo() not implemented yet"));
	}
	void unmapBuffer()throw(BufferException)
	{
		//TODO implement when needed
		throw(BufferException("mapCPUAdressSpaceTo() not implemented yet"));
	}
	///\}




	//convenience functions to access bufferInfo data;
	///\{
	String getName()const;

	ContextTypeFlags getContextTypeFlags()const;
	bool hasBufferInContext(ContextType type) const;
	bool isCLGLShared()const;

	int  getNumElements() const;
	size_t  getElementSize() const;
	Type getElementType() const;

	//get the bufferinfo directly:
	const BufferInfo& getBufferInfo() const;
	///\}

	//convenience caster methods:
	///\{
	bool isPingPongBuffer() const;
	PingPongBuffer& toPingPongBuffer() throw(BufferException);

	bool isDefaultBuffer()const;
	Buffer& toDefaultBuffer()throw(BufferException);

	bool isTexture() const;
	bool isTexture1D() const;
	Texture1D& toTexture1D() throw(BufferException);
	bool isTexture2D() const;
	Texture2D& toTexture2D() throw(BufferException);
	bool isTexture3D() const;
	Texture3D& toTexture3D() throw(BufferException);
//	bool isRenderBuffer() const;
//	RenderBuffer& toRenderBuffer() throw(BufferException);
	///\}


	//getters n setters for the buffer handles (might need to be exposed due to passing as kernel arguments etc)
	///\{
	const CPUBufferHandle getCPUBufferHandle()const throw(BufferException);
	GraphicsBufferHandle getGraphicsBufferHandle()const throw(BufferException);
	ComputeBufferHandle getComputeBufferHandle()const throw(BufferException);
	///\}
protected:

	//to be called by the constructor;
	bool allocMem()throw(BufferException);


	//PingPongBuffer calls following routines of his managed buffers; so let's make him friend
	friend class PingPongBuffer;
#	define FLEWNIT_PURE_VIRTUAL
#	include "BufferVirtualSignatures.h"
#	undef FLEWNIT_PURE_VIRTUAL

#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	//friend Profiler so that he can set the ID of the BasicObjects;
	friend class Profiler;

	void registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
	void unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
#endif

	BufferInfo* mBufferInfo;

	//NULL if no host pointer exists;
	CPUBufferHandle mCPU_Handle;
	//0 if no openGL buffer exists;
	GraphicsBufferHandle mGraphicsBufferHandle;
	//mComputeBufferHandle(), i.e "cl_mem cl::Memory::operator()" is NULL if no Buffer exists;
	//OpenCL distinguishes between a buffer and an image; I don't like this conceptional
	//separation, as they are both regions in memory; Thus, the interface in this framework
	//is different than in openCL: Buffer is the base class, Texture etc. derive from this,
	//non-image Buffers are derictly implemented and internally acessed by casting
	//the cl::Memory member to cl::Buffer/cl::BufferGL;
	//interop handle classes (e.g. cl::BufferGL, cl::Image2D, cl::Image2DGL etc.) will be handled by statically casting the cl::Memory mCLBuffer member;
	ComputeBufferHandle mComputeBufferHandle;


};

}

