/*
 * BufferInterface.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief The base class of all buffers;
 */

#pragma once

#include "BufferInterface.h"


namespace Flewnit
{





class Buffer
: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	Buffer(String name, cl_GLint numElements, cl_GLint elementSize);
	virtual ~Buffer();


protected:

	String mName;
	BufferTypeFlags mBufferTypeFlags;

	cl_GLint mNumElements;
	cl_GLint mEelementSize;

	//NULL if no host pointer exists;
	void* mHostPtr;
	//0 if no openGL buffer exists;
	GLuint mGLBuffer;
	//mCLBuffer() (operator()) is NULL if no Buffer exists;
	cl::Buffer mCLBuffer;

	//interop

public:
	virtual bool isAllocated(ContextType type) ;
	virtual bool allocMem(ContextType type, size_t sizeInByte);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;
	//virtual void unBind()=0;

	virtual BufferTypeFlags getBufferTypeFlags()const ;
	virtual String getName() const ;

	virtual void setData(void* data, ContextType type);

	virtual int  getNumElements() const;
	virtual int  getElementSize();
	virtual Type getElementType();

	virtual bool isPingPongBuffer(ContextType type);


	virtual BufferInfo* getBufferInfo() const;

protected:

	BufferInfo* mBufferInfo;

};

}

