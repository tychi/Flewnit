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

	//indicator do define GL_STATIC_DRAW or GL_DYNAMIC_DRAW for GL buffers;
	bool mContentsAreModifiedFrequently;

	GLenum mGlBufferTargetEnum; // GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER or GL_UNIFORM_BUFFER; will be set according to glBufferType;

public:

	Buffer( const BufferInfo& buffi,
			//normally, a vertex attribute buffer is seldom modified; but for the special case of particle simulation via ocl and point rendering via ogl,
			//the pasition and pressure etc. buffer will be completely updated every frame;
			bool contentsAreModifiedFrequently = false ,
			//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
			//the caller is responsible of the deletion of the data pointer;
			const void* data = NULL);
	virtual ~Buffer();


	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);


	//virtual void setData(const void* data, ContextTypeFlags where)throw(BufferException);


	//bind the currently active managed buffer:
	//virtual void bind(ContextType type) ;

	//copyMethods to create a new buffer from a texture or vice versa via copying
	//TODO when necessary

protected:
	virtual bool allocMem()throw(BufferException);

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
	virtual void copyGL(GraphicsBufferHandle bufferToCopyContentsTo);
	virtual void copyCL(ComputeBufferHandle bufferToCopyContentsTo);
	virtual void freeGL();
	virtual void freeCL();
	virtual void mapGLToHost(void* data);
	virtual void mapCLToHost(void* data);
	virtual void unmapGL();
	virtual void unmapCL();

};

}

