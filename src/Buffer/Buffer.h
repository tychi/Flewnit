/*
 * Buffer.h
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
	//OpenCL differences between a buffer and an image; I don't like this conceptional
	//separation, as they are both regions in memory; Thus, the interface in this framework
	//is different than in openCL: Buffer is the base class, Texture etc. derive from this,
	//non-image Buffers are derictly implemented and internally acessed by casting
	//the cl::Memory member to cl::Buffer/cl::BufferGL;
	//interop handle classes (e.g. cl::BufferGL etc.) will be handled by statically casting the cl::Memory mCLBuffer member;
	cl::Memory mCLBuffer;

public:
	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);


	virtual bool allocMem(ContextType type);
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException);
	virtual void setData(void* data, ContextType type);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;


};

}

