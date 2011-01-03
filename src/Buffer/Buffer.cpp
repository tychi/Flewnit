/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"



namespace Flewnit
{

Buffer::Buffer(
		String name,
		ContextTypeFlags usageContextFlags,
		BufferTypeFlags bufferTypes,
		Type elementType,
		cl_GLint numElements,
		//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
		//the caller is responsible of the deletion of the data pointer;
		const void* data)
: BufferInterface(name,usageContextFlags)
{

	// TODO Auto-generated constructor stub

}

Buffer::~Buffer()
{
	// TODO Auto-generated destructor stub
}


bool Buffer::operator==(const BufferInterface& rhs) const
{
	//if everything is implemented and maintained correctly, the pure compraision of the bufferinfo
	//should be enough; but don't trust the programmer, not even yourself :P
	return (*mBufferInfo == rhs.getBufferInfo()
			//is it really of same type?
			&& dynamic_cast<const Buffer*>(&rhs));

}

const BufferInterface& Buffer::operator=(const BufferInterface& rhs) throw(BufferException)
{

}

bool Buffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
{

}



bool Buffer::allocMem(ContextType type)throw(BufferException)
{

}


bool Buffer::freeMem(ContextType type)
{

}

void Buffer::setData(void* data, ContextTypeFlags where)throw(BufferException)
{

}


void Buffer::bind(ContextType type)
{

}



}
