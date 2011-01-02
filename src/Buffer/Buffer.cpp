/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"



namespace Flewnit
{

Buffer::Buffer(String name, Type elementType, cl_GLint numElements )
: BufferInterface(name)
{

	// TODO Auto-generated constructor stub

}

Buffer::~Buffer()
{
	// TODO Auto-generated destructor stub
}


bool Buffer::operator==(const BufferInterface& rhs) const
{

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

void Buffer::setData(void* data, ContextType type)
{

}


void Buffer::bind(ContextType type)
{

}



}
