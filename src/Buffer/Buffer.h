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

	Buffer(String name, Type elementType, cl_GLint numElements );
	virtual ~Buffer();


protected:

	//NULL if no host pointer exists;
	void* mHostPtr;
	//0 if no openGL buffer exists;
	GLuint mGLBuffer;
	//mCLBuffer() (operator()) is NULL if no Buffer exists;
	cl::Buffer mCLBuffer;

	//interop handle classes (e.g. cl::BufferGL etc.) will be used in specialized derived buffer classes
	BufferInfo mBufferInfo;

public:
	virtual bool isAllocated(ContextType type) const;
	virtual bool allocMem(ContextType type);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;
	//virtual void unBind()=0;

	virtual BufferTypeFlags getBufferTypeFlags()const ;
	virtual String getName() const ;

	virtual void setData(void* data, ContextType type);

	virtual int  getNumElements() const;
	virtual int  getElementSize() const;
	virtual Type getElementType() const;
	virtual cl_GLenum getElementInternalFormat() const;

	virtual bool isPingPongBuffer()const;


	virtual const BufferInfo& getBufferInfo() const;

};

}

