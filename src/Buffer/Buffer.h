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

	Buffer(String name, ContextTypeFlags usageContextFlags, BufferTypeFlags bufferTypes, Type elementType, cl_GLint numElements,
			//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
			//the caller is responsible of the deletion of the data pointer;
			const void* data = NULL);
	virtual ~Buffer();



public:
	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);


	virtual bool allocMem(ContextType type)throw(BufferException);
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException);
	virtual void setData(void* data, ContextTypeFlags where)throw(BufferException);
	virtual bool freeMem(ContextType type) ;

	//bind the currently active managed buffer:
	virtual void bind(ContextType type) ;

	//copyMethods to create a new buffer from a texture or vice versa via copying
	//TODO when necessary


};

}

