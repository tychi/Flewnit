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
: BufferInterface(ping->getBufferInfo()),
mRecentlyUpdatedBufferIndex(0),
mCurrentActiveBufferIndex(1)
{
	mBufferInfo.isPingPongBuffer = true;

	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Creating PingPongBuffer named "<<mBufferInfo.name<<" ;\n";

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
}

PingPongBuffer::~PingPongBuffer()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Destroying PingPongBuffer named "<<mBufferInfo.name<<" ;\n";

	delete mPingPongBuffers[mRecentlyUpdatedBufferIndex];
	delete mPingPongBuffers[mCurrentActiveBufferIndex];

	mCPU_Handle=0;
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
	BufferInfo tmp(mBufferInfo);
	tmp.isPingPongBuffer=false;
	assert( "bufferinfo of pingPongBuffer object is in synch with those of the managed buffers"
			&& tmp == mPingPongBuffers[0]->getBufferInfo()
			&& tmp == mPingPongBuffers[1]->getBufferInfo() );



}



bool PingPongBuffer::operator==(const BufferInterface& rhs) const
{
	const PingPongBuffer* castedPtr = dynamic_cast<const PingPongBuffer*>(&rhs);

	if(!castedPtr) return false;

	//ok, it is a ping pong buffer; now compare the pings and pongs to each other (comparing twice is redundant, but ... :P)
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


	//perform copyings of all data stores;
	*(mPingPongBuffers[mRecentlyUpdatedBufferIndex]) = *(castedPtr->getCurrentActiveBuffer());
	*(mPingPongBuffers[mCurrentActiveBufferIndex]) = *(castedPtr->getRecentlyUpdatedBuffer());

	//getHandlesFromCurrentActiveBuffer();

	return *this;
}


//generators empty, as managed buffers generate and alloc themselves
virtual void PingPongBuffer::generateGL()
{}
virtual void PingPongBuffer::generateCL()
{}

virtual void PingPongBuffer::bindGL()
{mPingPongBuffers[mCurrentActiveBufferIndex]->bindGL();}
virtual void PingPongBuffer::bindCL()
{mPingPongBuffers[mCurrentActiveBufferIndex]->bindCL();}

//allocators empty, as managed buffers generate and alloc themselves
virtual void PingPongBuffer::allocGL()
{}
virtual void PingPongBuffer::allocCL()
{}

//write only to
virtual void PingPongBuffer::writeGL(const void* data)
{}
virtual void PingPongBuffer::writeCL(const void* data)
{}
virtual void PingPongBuffer::readGL(void* data)
{}
virtual void PingPongBuffer::readCL(void* data)
{}
virtual void PingPongBuffer::copyGL(GraphicsBufferHandle bufferToCopyContentsTo)
{}
virtual void PingPongBuffer::copyGL(ComputeBufferHandle bufferToCopyContentsTo)
{}
virtual void PingPongBuffer::freeGL()
{}
virtual void PingPongBuffer::freeCL()
{}
virtual void PingPongBuffer::mapGLToHost(void* data)
{}
virtual void PingPongBuffer::mapCLToHost(void* data)
{}
virtual void PingPongBuffer::unmapGL()
{}
virtual void PingPongBuffer::unmapCL()
{}






//bool PingPongBuffer::allocMem()throw(BufferException)
//{
//	checkPingPongError();
////	return
////		mPingPongBuffers[0]->allocMem(typeFlags) &&
////		mPingPongBuffers[1]->allocMem(typeFlags);
//	throw(BufferException("PingPongBuffer::allocMem() not allowed"));
//}

//void PingPongBuffer::setData(const void* data, ContextTypeFlags where)throw(BufferException)
//{
//	checkPingPongError();
//	mPingPongBuffers[0]->setData(data,where);
//	mPingPongBuffers[1]->setData(data,where);
//}
//
//bool PingPongBuffer::readBack()throw(BufferException)
//{
//	mPingPongBuffers[mCurrentActiveBufferIndex]->readBack();
//}

//bool PingPongBuffer::copyBetweenContexts(ContextType from,ContextType to)throw(BufferException)
//{
//	checkPingPongError();
//	return
//		mPingPongBuffers[0]->copyBetweenContexts(from, to) &&
//		mPingPongBuffers[1]->copyBetweenContexts(from, to);
//}




//void PingPongBuffer::bind(ContextType type)
//{
//	checkPingPongError();
//	mPingPongBuffers[mCurrentActiveBufferIndex]->bind(type);
//}



}
