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




protected:

	//wrapper functions to GL and CL calls without any error checking,
	//i.e. semantic checks/flag delegation/verifiaction must be done before those calls;
	//those routines are introduced to reduce boilerplate code;
	virtual void generateGL();
	virtual void generateCL();
	virtual void generateCLGL();

	virtual void bindGL();
	virtual void allocGL();

	virtual void writeGL(const void* data);
	virtual void writeCL(const void* data);
	virtual void readGL(void* data);
	virtual void readCL(void* data);
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom);
	virtual void copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom);
	virtual void freeGL();
	virtual void freeCL();
//	virtual void* mapGLToHost();
//	virtual void* mapCLToHost();
//	virtual void unmapGL();
//	virtual void unmapCL();


};

}

