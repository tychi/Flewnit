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
: mBufferInfo(name)
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

bool Buffer::isAllocated(ContextType type) const
{

}


bool Buffer::allocMem(ContextType type)
{

}


bool Buffer::freeMem(ContextType type)
{

}



void Buffer::bind(ContextType type)
{

}


//void Buffer::unBind(){}

BufferTypeFlags Buffer::getBufferTypeFlags()const
{

}


String Buffer::getName() const
{

}



void Buffer::setData(void* data, ContextType type)
{

}



int  Buffer::getNumElements() const
{

}


size_t  Buffer::getElementSize() const
{

}


Type Buffer::getElementType() const
{

}


//cl_GLenum Buffer::getElementInternalFormat() const
//{
//
//}



bool Buffer::isPingPongBuffer()const
{

}




const BufferInfo& Buffer::getBufferInfo() const
{

}



}
