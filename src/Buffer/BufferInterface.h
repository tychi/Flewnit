/*
 * BufferInterface.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief The base class of all buffers;
 */

#pragma once

//TODO from/to typeid stuff

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/BasicObject.h"

#include "Buffer/BufferSharedDefinitions.h"

#include "Buffer/BufferHelperUtils.h"

#include "Common/Math.h"

#include <exception>



namespace Flewnit
{


class BufferException : public std::exception
{
	String mDescription;
 public:
	BufferException(String description = "unspecified Buffer exception") throw()
	: mDescription(description)
	{ }

	virtual ~BufferException() throw(){}

	virtual const char* what() const throw()
	{
	    return mDescription.c_str();
	}
};


/**
 * some partially redundant information about the buffer;
 */
class BufferInfo
{
public:
	String name;
	BufferTypeFlags bufferTypeFlags;
	ContextTypeFlags usageContexts;
	bool isPingPongBuffer; //default false
	bool isTexture; //default false
	bool isRenderBuffer;//default false
	bool isSharedByCLAndGL; //default false
	bool allocationGuards[__NUM_CONTEXT_TYPES__]; //default{false,false,false};
	Type elementType; //default TYPE_UNDEF
	cl_GLuint numElements;
	cl_GLuint dimensionality; //interesting for textures: 1,2 or 3 dimensions;
	Vector3Dui dimensionExtends;


	explicit BufferInfo(String name);
	BufferInfo(const BufferInfo& rhs);
	virtual ~BufferInfo();
	bool operator==(const BufferInfo& rhs) const;
	const BufferInfo& operator=(const BufferInfo& rhs);

};



class BufferInterface
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	BufferInterface(String name);
	virtual ~BufferInterface();


	//check for campatibility: not the contents, but the types, dimensions, allocations etc are compared;
	virtual bool operator==(const BufferInterface& rhs) const = 0;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException) = 0;

	virtual void bind(ContextType type) = 0;

	//memory manipulating stuff:
	///\{
	bool isAllocated(ContextType type) const;
	virtual bool allocMem(ContextType type) throw(BufferException) = 0;
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)=0;
	virtual bool freeMem(ContextType type) = 0;
	///\}




	virtual BufferTypeFlags getBufferTypeFlags()const =0;
	virtual String getName() const = 0;

	virtual void setData(void* data, ContextType type) = 0;

	//convenience functions to access bufferInfo data;
	///\{
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
	bool isRenderBuffer() const;
	RenderBuffer& toRenderBuffer() throw(BufferException);
	///\}


	//getters n setters for the buffer handles (might need to be exposed due to passing as kernel arguments etc)
	///\{
	const CPUBufferHandle getCPUBufferHandle()const;
	GraphicsBufferHandle getGraphicsBufferHandle()const;
	ComputeBufferHandle getComputeBufferHandle()const;
	///\}
protected:


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

