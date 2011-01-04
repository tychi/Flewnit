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




class Texture
: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	Texture(String name,
			ContextTypeFlags usageContextFlags,
			GLint imageInternalChannelLayout, //GL_RGBA or stuff
			GLenum imageInternalDataType,	//GL_FLOAT or  GL_UNSIGNED_BYTE, or some stuff like  GL_UNSIGNED_SHORT_5_6_5
			Type elementType,				//must fit  imageInternalDataType and imageInternalChannelLayout
			cl_GLint numElements );
	virtual ~Texture();



public:
	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);



	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)=0;
	virtual void setData(const void* data, ContextTypeFlags where)throw(BufferException)=0;


	//bind the currently active managed buffer:
	virtual void bind(ContextType type) =0 ;

	//copyMethods to create a new buffer from a texture or vice versa via copying
	//TODO when necessary

protected:
	virtual bool allocMem()throw(BufferException) = 0;


};

}

