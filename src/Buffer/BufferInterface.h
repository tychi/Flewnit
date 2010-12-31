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

#include "Common/CL_GL_Common.h"


namespace Flewnit
{

//class BufferHandles
//{
//public:
//	void* mHostBuffer;
//	GLuint mGLBuffer;
//
//	cl::Buffer mCLBuffer;
//
//	cl::BufferRenderGL
//
//	cl::BufferGL mCLGLBuffer;
//
//};

/**
 * some partially redundant information about the buffer;
 */
class BufferInfo
{
public:
	String name;
	BufferTypeFlags bufferTypeFlags;
	ContextTypeFlags usageContexts;
	//default false
	bool isPingPongBuffer;
	//default false
	bool isSharedByCLAndGL;
	bool allocationGuards[__NUM_CONTEXT_TYPES__];
	Type elementType;
	cl_GLuint numElements;
	cl_GLuint dimensionality;
	Vector3Dui dimensionExtends;

	explicit BufferInfo(String name)
	: name(name),bufferTypeFlags(EMPTY_BUFFER_FLAG), usageContexts(NO_CONTEXT_TYPE_FLAG), isPingPongBuffer(false),isSharedByCLAndGL(false),
	  elementType(TYPE_UNDEF),numElements(0),dimensionality(1),dimensionExtends(Vector3Dui(0,0,0))
	{
		allocationGuards[HOST_CONTEXT_TYPE] = false;
		allocationGuards[OPEN_CL_CONTEXT_TYPE] = false;
		allocationGuards[OPEN_GL_CONTEXT_TYPE] = false;
	}

};



class BufferInterface
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	BufferInterface();
	virtual ~BufferInterface();


protected:


#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	//friend Profiler so that he can set the ID of the BasicObjects;
	friend class Profiler;

	void registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
	void unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
#endif



public:


	virtual bool isAllocated(ContextType type) const = 0;
	virtual bool allocMem(ContextType type) = 0;
	virtual bool freeMem(ContextType type) = 0;

	virtual void bind(ContextType type) = 0;
	//virtual void unBind()=0;

	virtual BufferTypeFlags getBufferTypeFlags()const =0;
	virtual String getName() const = 0;

	virtual void setData(void* data, ContextType type) = 0;

	//convenience functions to access bufferInfo data;
	virtual int  getNumElements() const = 0;
	virtual int  getElementSize() const = 0;
	virtual Type getElementType() const = 0;
	virtual cl_GLenum getElementInternalFormat() const = 0;
	virtual bool isPingPongBuffer() const = 0;

	//get the bufferinfo directly:
	virtual const BufferInfo& getBufferInfo() const =0;

protected:

	BufferInfo* mBufferInfo;

};

}

