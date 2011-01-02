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

PingPongBuffer::PingPongBuffer(String name,BufferInterface* ping, BufferInterface* pong )
: BufferInterface(name),
mRecentlyUpdatedBufferIndex(0),
mCurrentActiveBufferIndex(1)
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Creating PingPongBuffer named "<<name<<" ;\n";

	mPingPongBuffers[mRecentlyUpdatedBufferIndex] = ping;
	mPingPongBuffers[mCurrentActiveBufferIndex] = pong;

	assert( "pingpong buffers are identical/compatible" && (*ping) == (*pong));


	(*mBufferInfo) = ping->getBufferInfo();
	//override pingpongsetting in mBufferInfo
	mBufferInfo->isPingPongBuffer = true;
}

PingPongBuffer::~PingPongBuffer()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Destroying PingPongBuffer named "<<mBufferInfo->name<<" ;\n";

	delete mPingPongBuffers[mRecentlyUpdatedBufferIndex];
	delete mPingPongBuffers[mCurrentActiveBufferIndex];
}


void PingPongBuffer::toggleBuffers()
{
	mRecentlyUpdatedBufferIndex =  	(mRecentlyUpdatedBufferIndex +1) %2;
	mCurrentActiveBufferIndex =  	(mCurrentActiveBufferIndex +1) %2;
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

	//ok, it is a pig pong buffer; now compar the pings and pongs to each other (comparing twice is redundant, but ... :P)
	return ( 	*(mPingPongBuffers[mRecentlyUpdatedBufferIndex])
			==  *(castedPtr->getRecentlyUpdatedBuffer()) )
			&&
		   ( 	*(mPingPongBuffers[mCurrentActiveBufferIndex])
			==  *(castedPtr->getCurrentActiveBuffer()) )
	;
}

const BufferInterface& PingPongBuffer::operator=(const BufferInterface& rhs) throw(BufferException)
{
	//TODO
}



bool PingPongBuffer::allocMem(ContextType type)
{
	checkPingPongError();
	return
		mPingPongBuffers[0]->allocMem(type) &&
		mPingPongBuffers[1]->allocMem(type);
}

void PingPongBuffer::setData(void* data, ContextType type)
{
	checkPingPongError();
	mPingPongBuffers[0]->setData(data,type);
	mPingPongBuffers[0]->setData(data,type);
}

bool PingPongBuffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
{
	//TODO
}



bool PingPongBuffer::freeMem(ContextType type)
{
	checkPingPongError();
	return
		mPingPongBuffers[0]->freeMem(type) &&
		mPingPongBuffers[1]->freeMem(type);
}



void PingPongBuffer::bind(ContextType type)
{
	mPingPongBuffers[mCurrentActiveBufferIndex]->bind(type);
}



}
