/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Buffer.h"
#include "Util/Log/Log.h"



namespace Flewnit
{

Buffer::Buffer(
		String name,
		ContextTypeFlags usageContextFlags,
		BufferTypeFlags bufferTypeFlags,
		Type elementType,
		cl_GLint numElements,
		//normally, a vertex attribute buffer is seldom modified; but for the special case of particle simulation via ocl and point rendering via ogl,
		//the pasition and pressure etc. buffer will be completely updated every frame;
		bool contentsAreModified,
		//if data!= NULL, the buffers of the desired contexts are allocated and copied to;
		//the caller is responsible of the deletion of the data pointer;
		const void* data)
: BufferInterface(name,usageContextFlags),mContentsAreModified(contentsAreModified)
{
	mBufferInfo->bufferTypeFlags = bufferTypeFlags;
	mBufferInfo->elementType = elementType;
	mBufferInfo->numElements = numElements;

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



bool Buffer::allocMem(ContextTypeFlags typeFlags)throw(BufferException)
{
	int sizeInBytes= mBufferInfo->numElements * BufferHelper::elementSize(mBufferInfo->elementType);

	if( (typeFlags & HOST_CONTEXT_TYPE_FLAG) )
	{
		if(mBufferInfo->allocationGuards[HOST_CONTEXT_TYPE])
		{
			assert(mCPU_Handle != 0 && "something wnet terribly wrong with the info maintaining");
			LOG<< WARNING_LOG_LEVEL << "Buffer::allocMem: CPU memory already allocated;\n";
		}
		else
		{
			if( !(mBufferInfo->bufferTypeFlags & CPU_BUFFER_FLAG))
			{	throw(BufferException("Buffer::allocMem:usageContextFlags and bufferTypeFlags don't fit"));}

			mCPU_Handle = malloc(sizeInBytes);

			mBufferInfo->allocationGuards[HOST_CONTEXT_TYPE] = true;
		}
	}

	if(typeFlags & OPEN_GL_CONTEXT_TYPE_FLAG)
	{

	}
	else
	{
		if(typeFlags & OPEN_GL_CONTEXT_TYPE_FLAG)
		{
			//mComputeBufferHandle = cl::Buffer()
		}
	}




#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
		registerBufferAllocation(typeFlags,sizeInBytes);
#endif

}


bool Buffer::freeMem(ContextType type)
{

}

void Buffer::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{

}


void Buffer::bind(ContextType type)
{

}



}
