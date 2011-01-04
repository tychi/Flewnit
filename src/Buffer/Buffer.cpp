/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"
#include "Util/Log/Log.h"
#include "Simulator/OpenCL_Manager.h"



namespace Flewnit
{

Buffer::Buffer(
		String name,
		ContextTypeFlags usageContextFlags,
		Type elementType,
		cl_GLint numElements,
		//may only be != NO_GL_BUFFER_TYPE if usageContextFlags&OPEN_GL_CONTEXT_TYPE_FLAG != 0
		GLBufferType glBufferType = NO_GL_BUFFER_TYPE,
		//normally, a vertex attribute buffer is seldom modified; but for the special case of particle simulation via ocl and point rendering via ogl,
		//the pasition and pressure etc. buffer will be completely updated every frame;
		bool contentsAreModifiedFrequently,
		//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
		//the caller is responsible of the deletion of the data pointer;
		const void* data)
: BufferInterface(name,usageContextFlags),
  mContentsAreModifiedFrequently(contentsAreModifiedFrequently),
  //invalid value until mem alloc
  mGlBufferTargetEnum(0)
{
	mBufferInfo->bufferTypeFlags = bufferTypeFlags;
	mBufferInfo->elementType = elementType;
	mBufferInfo->numElements = numElements;

	mBufferSizeInByte = numElements * BufferHelper::elementSize(elementType);

	if( bufferTypeFlags &
		(TEXTURE_1D_BUFFER_FLAG | TEXTURE_2D_BUFFER_FLAG |TEXTURE_3D_BUFFER_FLAG | RENDER_BUFFER_FLAG)
	)
	{
		throw(BufferException("Buffer created with texture flags! use texture classes!"));
	}

	if(data)
	{
		allocMem(usageContextFlags);
		setData(data,usageContextFlags);
	}


}

Buffer::~Buffer()
{
	GUARD(glDeleteBuffers(1, &mGraphicsBufferHandle));
	unregisterBufferAllocation(mBufferInfo->usageContexts, mBufferSizeInByte);
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
	if( (*this) == rhs )
	{
		//TODO after alloc and setData() stuff :P
	}
	else
	{
		throw(BufferException("Buffer::operator= : Buffers not compatible"));
	}
}

bool Buffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
{
	//TODO after alloc and setData() stuff :P
}



bool Buffer::allocMem()throw(BufferException)
{
	if( (typeFlags & HOST_CONTEXT_TYPE_FLAG) )
	{
		if(mBufferInfo->allocationGuards[HOST_CONTEXT_TYPE])
		{
			assert(mCPU_Handle != 0 && "something went terribly wrong with the info maintaining");
			LOG<< WARNING_LOG_LEVEL << "Buffer::allocMem: CPU memory already allocated;\n";
		}
		else
		{
			if( !( mBufferInfo->bufferTypeFlags & CPU_BUFFER_FLAG) || !( mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG))
			{	throw(BufferException("Buffer::allocMem:usageContextFlags and bufferTypeFlags don't fit"));}

			mCPU_Handle = malloc(mBufferSizeInByte);

			mBufferInfo->allocationGuards[HOST_CONTEXT_TYPE] = true;
		}
	}

	if(typeFlags & OPEN_GL_CONTEXT_TYPE_FLAG)
	{
		//ok, there is a need for an openGL buffer; maybe it will be shared with openCL,
		//but that doesn't matter for the GL buffer creation :)
		if(mBufferInfo->allocationGuards[OPEN_GL_CONTEXT_TYPE])
		{
			assert(mGraphicsBufferHandle != 0 && "something went terribly wrong with the info maintaining");
			LOG<< WARNING_LOG_LEVEL << "Buffer::allocMem: OpenGL Buffer Storage already allocated;\n";
		}
		else
		{
			if( //first case: none of the valid flags for this subclass is specified:
				!	(mBufferInfo->bufferTypeFlags &
						(	VERTEX_ATTRIBUTE_BUFFER_FLAG | VERTEX_INDEX_BUFFER_FLAG | UNIFORM_BUFFER_FLAG	)
					)
				||
				//second case: at least one of the forbidden flags is specified:
				 	( 	mBufferInfo->bufferTypeFlags &
				 		(TEXTURE_1D_BUFFER_FLAG	| TEXTURE_2D_BUFFER_FLAG | TEXTURE_3D_BUFFER_FLAG |	RENDER_BUFFER_FLAG)

				 	)
				 ||
				 //third case: there was no GL stuff desired once;
				 !	( 	mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG)
			)
			{	throw(BufferException("Buffer::allocMem:usageContextFlags and bufferTypeFlags don't fit"));}

			//no let's check if there is more than one valid flag specified:
			int counter = 0;
			if(mBufferInfo->bufferTypeFlags & VERTEX_ATTRIBUTE_BUFFER_FLAG )
			{mGlBufferTargetEnum = GL_ARRAY_BUFFER; counter++;}
			if(mBufferInfo->bufferTypeFlags & VERTEX_INDEX_BUFFER_FLAG )
			{mGlBufferTargetEnum = GL_ELEMENT_ARRAY_BUFFER; counter++;}
			if(mBufferInfo->bufferTypeFlags & UNIFORM_BUFFER_FLAG )
			{mGlBufferTargetEnum = GL_UNIFORM_BUFFER; counter++;}
			if(counter != 1)
			{throw(BufferException(" Buffer::allocMem: more than one valid flag specified"));}

			GUARD(glGenBuffers(1, &mGraphicsBufferHandle));
			GUARD(glBindBuffer(mGlBufferTargetEnum, mGraphicsBufferHandle));

			GUARD(glBufferData(
					//which target?
					mGlBufferTargetEnum,
					// size of storage
					mBufferSizeInByte ,
					//data will be passed in setData();
					NULL,
					//draw static if not modded, dynamic otherwise ;)
					mContentsAreModifiedFrequently ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

			mBufferInfo->allocationGuards[OPEN_GL_CONTEXT_TYPE] = true;
		}

		//-----------------------------------------------------------------------------------

		if(typeFlags & OPEN_CL_CONTEXT_TYPE_FLAG)
		{
			//ok, there is a need for both worlds of GPU computing;
			//let's create an OpenCL buffer from the GL one :)
			if(mBufferInfo->allocationGuards[OPEN_CL_CONTEXT_TYPE])
			{
				assert(mComputeBufferHandle() != 0 && "something went terribly wrong with the info maintaining");
				LOG<< WARNING_LOG_LEVEL << "Buffer::allocMem: OpenCL Buffer Storage already allocated;\n";
			}
			else
			{
				if( !( mBufferInfo->bufferTypeFlags & OPEN_CL_BUFFER_FLAG) || !( mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
				{	throw(BufferException("Buffer::allocMem:usageContextFlags and bufferTypeFlags don't fit"));}
				//mComputeBufferHandle ==
			}
		}
	}
	else
	{
		if(typeFlags & OPEN_GL_CONTEXT_TYPE_FLAG)
		{
			//we seem to  only need a "pure" openCL buffer; but there is the possibility
		}
	}




#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
		registerBufferAllocation(typeFlags,mBufferSizeInByte);
#endif

}



void Buffer::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{

}


void Buffer::bind(ContextType type)
{

}



}
