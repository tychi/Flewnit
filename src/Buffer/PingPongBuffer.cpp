/*
 * Buffer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "PingPongBuffer.h"

#include "Util/Log/Log.h"



namespace Flewnit
{

PingPongBuffer::PingPongBuffer(String name,BufferInterface* ping, BufferInterface* pong ) throw(BufferException)
: BufferInterface(name,ping->getBufferInfo().usageContexts),
mRecentlyUpdatedBufferIndex(0),
mCurrentActiveBufferIndex(1)
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Creating PingPongBuffer named "<<name<<" ;\n";

	if( !( ping || pong ) )
	{
		throw(BufferException("pingpong buffers must be non-NULL"));
	}

	if(dynamic_cast<PingPongBuffer*>(ping) || dynamic_cast<PingPongBuffer*>(pong))
	{
		throw(BufferException("a ping pong buffer may not manage other ping pong buffers!"));
	}
	if( !( (*ping) == (*pong) ) )
	{
		throw(BufferException("pingpong buffers are not identical/compatible"));
	}


	mPingPongBuffers[mRecentlyUpdatedBufferIndex] = ping;
	mPingPongBuffers[mCurrentActiveBufferIndex] = pong;


	(*mBufferInfo) = ping->getBufferInfo();
	//override pingpongsetting in mBufferInfo
	mBufferInfo->isPingPongBuffer = true;


}

PingPongBuffer::~PingPongBuffer()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Destroying PingPongBuffer named "<<mBufferInfo->name<<" ;\n";

	delete mPingPongBuffers[mRecentlyUpdatedBufferIndex];
	delete mPingPongBuffers[mCurrentActiveBufferIndex];

	//demonstrate that one has thought about gl-buffer deletiion(the managed buffers will delete themselves)
	mGraphicsBufferHandle = 0;
}


void PingPongBuffer::toggleBuffers()
{
	mRecentlyUpdatedBufferIndex =  	(mRecentlyUpdatedBufferIndex +1) %2;
	mCurrentActiveBufferIndex =  	(mCurrentActiveBufferIndex +1) 	%2;
	getHandlesFromCurrentActiveBuffer();
}

void PingPongBuffer::getHandlesFromCurrentActiveBuffer()
{
	mCPU_Handle= mPingPongBuffers[mCurrentActiveBufferIndex]->getCPUBufferHandle();
	mGraphicsBufferHandle=  mPingPongBuffers[mCurrentActiveBufferIndex]->getGraphicsBufferHandle();
	mComputeBufferHandle=  mPingPongBuffers[mCurrentActiveBufferIndex]->getComputeBufferHandle();
}

BufferInterface* PingPongBuffer::getRecentlyUpdatedBuffer()const
{
	return mPingPongBuffers[mRecentlyUpdatedBufferIndex];
}

BufferInterface* PingPongBuffer::getCurrentActiveBuffer()const
{
	return mPingPongBuffers[mCurrentActiveBufferIndex];
}

void PingPongBuffer::checkPingPongError() const
{
	assert( "pingpong buffers still in synch" && *(mPingPongBuffers[0]) == *(mPingPongBuffers[1]));

	//haxx to make the bufferinfs equal :P
	BufferInfo tmp  = *mBufferInfo;
	tmp.isPingPongBuffer=false;
	assert( "bufferinfo of pingPongBuffer object is in synch with those of the managed buffers"
			&& tmp == mPingPongBuffers[0]->getBufferInfo() );



}



bool PingPongBuffer::operator==(const BufferInterface& rhs) const
{
	const PingPongBuffer* castedPtr = dynamic_cast<const PingPongBuffer*>(&rhs);

	if(!castedPtr) return false;

	//ok, it is a ping pong buffer; now compar the pings and pongs to each other (comparing twice is redundant, but ... :P)
	return ( 	*(mPingPongBuffers[mRecentlyUpdatedBufferIndex])
			==  *(castedPtr->getRecentlyUpdatedBuffer()) )
			&&
		   ( 	*(mPingPongBuffers[mCurrentActiveBufferIndex])
			==  *(castedPtr->getCurrentActiveBuffer()) )
	;
}

const BufferInterface& PingPongBuffer::operator=(const BufferInterface& rhs) throw(BufferException)
{
	checkPingPongError();

	//buffers must be not identical (pointers not equal)...
	if( ! (this == &rhs  ) )
	{
		throw(BufferException("operator= not allowed for identical buffers;"));
	}

	//but compatible (objects "equal", i.e. same subclass and same buffer sizes//allocation schemes;)
	if( ! (*this == rhs  ) )
	{
		throw(BufferException("Ping Pong buffers aren't compatible for assignment;"));
	}


	const PingPongBuffer* castedPtr = dynamic_cast<const PingPongBuffer*>(&rhs);
	if(!castedPtr)
	{
		throw(BufferException("PingPongBuffer::operator= : rhs not of same subclass;"));
	}

//	delete mPingPongBuffers[mRecentlyUpdatedBufferIndex];
//	delete mPingPongBuffers[mCurrentActiveBufferIndex];
//
//	mPingPongBuffers[mRecentlyUpdatedBufferIndex] = castedPtr->getCurrentActiveBuffer();
//	mPingPongBuffers[mCurrentActiveBufferIndex] = castedPtr->getRecentlyUpdatedBuffer();

	*(mPingPongBuffers[mRecentlyUpdatedBufferIndex]) = *(castedPtr->getCurrentActiveBuffer());
	*(mPingPongBuffers[mCurrentActiveBufferIndex]) = *(castedPtr->getRecentlyUpdatedBuffer());

	getHandlesFromCurrentActiveBuffer();

	return *this;
}



bool PingPongBuffer::allocMem(ContextTypeFlags typeFlags)throw(BufferException)
{
	checkPingPongError();
	return
		mPingPongBuffers[0]->allocMem(typeFlags) &&
		mPingPongBuffers[1]->allocMem(typeFlags);
}

void PingPongBuffer::setData(const void* data, ContextTypeFlags where)throw(BufferException)
{
	checkPingPongError();
	mPingPongBuffers[0]->setData(data,where);
	mPingPongBuffers[1]->setData(data,where);
}

bool PingPongBuffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
{
	checkPingPongError();
	return
		mPingPongBuffers[0]->copyBetweenContexts(from, to) &&
		mPingPongBuffers[1]->copyBetweenContexts(from, to);
}




void PingPongBuffer::bind(ContextType type)
{
	checkPingPongError();
	mPingPongBuffers[mCurrentActiveBufferIndex]->bind(type);
}



}
