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
: mBufferInfo(name),
mRecentlyUpdatedBufferIndex(0),
mCurrentActiveBufferIndex(1)
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Creating PingPongBuffer named "<<name<<" ;\n";

	mPingPongBuffers[mRecentlyUpdatedBufferIndex] = ping;
	mPingPongBuffers[mCurrentActiveBufferIndex] = pong;

	assert( "pingpong buffers are identical/compatible" && (*ping) == (*pong));

	mBufferInfo = ping->getBufferInfo();
	//override pingpongsetting in mBufferInfo
	mBufferInfo.isPingPongBuffer = true;
}

PingPongBuffer::~PingPongBuffer()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Destroying PingPongBuffer named "<<mBufferInfo.name<<" ;\n";

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
	BufferInfo tmp  = mBufferInfo;
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


bool PingPongBuffer::isAllocated(ContextType type) const
{

	return mBufferInfo.allocationGuards[type];
}


bool PingPongBuffer::allocMem(ContextType type)
{
	checkPingPongError();
	mPingPongBuffers[0]->allocMem(type);
	mPingPongBuffers[0]->allocMem(type);
}


bool PingPongBuffer::freeMem(ContextType type)
{
	checkPingPongError();
	mPingPongBuffers[0]->freeMem(type);
	mPingPongBuffers[0]->freeMem(type);
}



void PingPongBuffer::bind(ContextType type)
{
	mPingPongBuffers[mCurrentActiveBufferIndex]->bind(type);
}


//void Buffer::unBind(){}

BufferTypeFlags PingPongBuffer::getBufferTypeFlags()const
{
	checkPingPongError();
	return mBufferInfo.bufferTypeFlags;
}


String PingPongBuffer::getName() const
{
	checkPingPongError();
	return mBufferInfo.name;
}



void PingPongBuffer::setData(void* data, ContextType type)
{
	checkPingPongError();
	mPingPongBuffers[0]->setData(data,type);
	mPingPongBuffers[0]->setData(data,type);
}



int  PingPongBuffer::getNumElements() const
{
	checkPingPongError();
	return mBufferInfo.numElements;
}


size_t  PingPongBuffer::getElementSize() const
{
	checkPingPongError();
	return BufferHelper::elementSize( mBufferInfo.elementType );
}


Type PingPongBuffer::getElementType() const
{
	checkPingPongError();
	return mBufferInfo.elementType;
}


//cl_GLenum PingPongBuffer::getElementInternalFormat() const
//{
//
//}



bool PingPongBuffer::isPingPongBuffer()const
{
	checkPingPongError();
	assert(mBufferInfo.isPingPongBuffer);
	return true;
}




const BufferInfo& PingPongBuffer::getBufferInfo() const
{
	checkPingPongError();
	return mBufferInfo;
}



}
