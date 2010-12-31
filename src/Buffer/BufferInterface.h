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

class BufferInfo
{
	//TODO implement
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


	virtual bool isAllocated(ContextType type) = 0;
	virtual bool allocMem(ContextType type, size_t sizeInByte) = 0;
	virtual bool freeMem(ContextType type) = 0;

	virtual void bind(ContextType type) = 0;
	//virtual void unBind()=0;

	virtual BufferTypeFlags getBufferTypeFlags()const =0;
	virtual String getName() const = 0;

	virtual void setData(void* data, ContextType type) = 0;

	virtual int  getNumElements() const = 0;
	virtual int  getElementSize() = 0;
	virtual Type getElementType() = 0;

	virtual bool isPingPongBuffer(ContextType type) = 0;


	virtual BufferInfo* getBufferInfo() const =0;

protected:

	BufferInfo* mBufferInfo;

};

}

