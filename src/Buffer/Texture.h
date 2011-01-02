/*
 * Texture.h
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

class TextureInfo:
	public BufferInfo
{
	cl_GLenum minFi
};


class Texture
: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	Texture(String name, Type elementType, cl_GLenum elementInternalFormat, cl_GLint numElements );
	virtual ~Texture();



public:
	virtual bool operator==(const BufferInterface& rhs) const =0;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException)=0;


	virtual bool allocMem(ContextType type)throw(BufferException) =0;
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)=0;
	virtual void setData(void* data, ContextType type)=0;
	virtual bool freeMem(ContextType type) ;

	//bind the currently active managed buffer:
	virtual void bind(ContextType type) =0 ;

	//copyMethods to create a new buffer from a texture or vice versa via copying
	//TODO when necessary


};

}

