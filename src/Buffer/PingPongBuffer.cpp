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
:
//tak BufferInfo from one of the Managed Buffers
BufferInterface(ping->getBufferInfo()),
mInactiveBufferIndex(0),
mActiveBufferIndex(1)
{
	//alloc new one, as we need its info for tracking purposes till the end,
	//when the managed buffers are already deleted;
	//mBufferInfo = new BufferInfo(ping->getBufferInfo());

	//override pingpong flag
	mBufferInfo->isPingPongBuffer = true;

	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Creating PingPongBuffer named "<<mBufferInfo->name<<" ;\n";

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


	mPingPongBuffers[mInactiveBufferIndex] = ping;
	mPingPongBuffers[mActiveBufferIndex] = pong;
}

PingPongBuffer::~PingPongBuffer()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"Destroying PingPongBuffer named "<<mBufferInfo->name<<" ;\n";

	delete mPingPongBuffers[mInactiveBufferIndex];
	delete mPingPongBuffers[mActiveBufferIndex];

	//omit a free() call by the bufferinterface destructor as members of this class were only references
	mCPU_Handle=0;
	//demonstrate that one has thought about gl-buffer deletiion(the Object orientation-managed CL buffers will free themselves)
	mGraphicsBufferHandle = 0;

}


void PingPongBuffer::toggleBuffers()
{
	mInactiveBufferIndex =  	(mInactiveBufferIndex +1) %2;
	mActiveBufferIndex =  	(mActiveBufferIndex +1) 	%2;
	getHandlesFromCurrentActiveBuffer();
}

void PingPongBuffer::getHandlesFromCurrentActiveBuffer()
{
	mCPU_Handle= mPingPongBuffers[mActiveBufferIndex]->getCPUBufferHandle();
	mGraphicsBufferHandle=  mPingPongBuffers[mActiveBufferIndex]->getGraphicsBufferHandle();
	mComputeBufferHandle=  mPingPongBuffers[mActiveBufferIndex]->getComputeBufferHandle();
}

BufferInterface* PingPongBuffer::getInactiveBuffer()const
{
	return mPingPongBuffers[mInactiveBufferIndex];
}

BufferInterface* PingPongBuffer::getActiveBuffer()const
{
	return mPingPongBuffers[mActiveBufferIndex];
}

void PingPongBuffer::checkPingPongError() const
{
	assert( "pingpong buffers still in synch" && *(mPingPongBuffers[0]) == *(mPingPongBuffers[1]));

	//haxx to make the bufferinfs equal :P
	BufferInfo tmp(*mBufferInfo);
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
	return ( 	*(mPingPongBuffers[mInactiveBufferIndex])
			==  *(castedPtr->getInactiveBuffer()) )
			&&
		   ( 	*(mPingPongBuffers[mActiveBufferIndex])
			==  *(castedPtr->getActiveBuffer()) )
	;
}

//const BufferInterface& PingPongBuffer::operator=(const BufferInterface& rhs) throw(BufferException)
//{
//	checkPingPongError();
//
//	//buffers must be not identical (pointers not equal)...
//	if( ! (this == &rhs  ) )
//	{
//		throw(BufferException("operator= not allowed for identical buffers;"));
//	}
//
//	//but compatible (objects "equal", i.e. same subclass and same buffer sizes//allocation schemes;)
//	if( ! (*this == rhs  ) )
//	{
//		throw(BufferException("Ping Pong buffers aren't compatible for assignment;"));
//	}
//
//
//	const PingPongBuffer* castedPtr = dynamic_cast<const PingPongBuffer*>(&rhs);
//	if(!castedPtr)
//	{
//		throw(BufferException("PingPongBuffer::operator= : rhs not of same subclass;"));
//	}
//
//
//	//perform copyings of all data stores;
//	*(mPingPongBuffers[mRecentlyUpdatedBufferIndex]) = *(castedPtr->getCurrentActiveBuffer());
//	*(mPingPongBuffers[mCurrentActiveBufferIndex]) = *(castedPtr->getRecentlyUpdatedBuffer());
//
//	//getHandlesFromCurrentActiveBuffer();
//
//	return *this;
//}


//generators empty, as managed buffers generate and alloc themselves
void PingPongBuffer::generateGL()throw(BufferException)
{}
void PingPongBuffer::generateCL()throw(BufferException)
{}
void PingPongBuffer::generateCLGL()throw(BufferException)
{}

//allocator empty, as managed buffers generate and alloc themselves
void PingPongBuffer::allocGL()throw(BufferException)
{}

//deleters empty as managed buffers delete their data store on destruction themselves
void PingPongBuffer::freeGL()throw(BufferException)
{}
void PingPongBuffer::freeCL()throw(BufferException)
{}

//following function implementation seems wrong
////alloc for the current active buffer:
//void PingPongBuffer::allocGL()throw(BufferException)
//{
//	mPingPongBuffers[mCurrentActiveBufferIndex]->allocGL();
//}


void PingPongBuffer::bindGL()throw(BufferException)
{mPingPongBuffers[mActiveBufferIndex]->bindGL();}



//write data to both buffers; if the programmaer want to write only one of the managed buffers,
//he will have to get them directly
void PingPongBuffer::writeGL(const void* data)throw(BufferException)
{
	mPingPongBuffers[mActiveBufferIndex]->writeGL(data);
	//mPingPongBuffers[1]->writeGL(data);
}
void PingPongBuffer::writeCL(const void* data)throw(BufferException)
{
	mPingPongBuffers[mActiveBufferIndex]->writeCL(data);
	//mPingPongBuffers[1]->writeCL(data);
}
void PingPongBuffer::readGL(void* data)throw(BufferException)
{
	mPingPongBuffers[mActiveBufferIndex]->readGL(data);
}
void PingPongBuffer::readCL(void* data)throw(BufferException)
{
	mPingPongBuffers[mActiveBufferIndex]->readCL(data);
}
void PingPongBuffer::copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	assert("PingPongBuffer::operator=() forbidden due to too much ambiguity; assign desired pingpong managed buffers directly!"&&0);
}
void PingPongBuffer::copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	assert("PingPongBuffer::operator=() forbidden due to too much ambiguity; assign desired pingpong managed buffers directly!"&&0);
}


//void PingPongBuffer::mapGLToHost(void* data)
//{
//	LOG<<WARNING_LOG_LEVEL<<"PingPongBuffer::mapGLToHost: are you sure to map both buffers of a ping pong buffer to host mem?\n";
//	mPingPongBuffers[0]->mapGLToHost(data);
//	mPingPongBuffers[1]->mapGLToHost(data);
//}
//void PingPongBuffer::mapCLToHost(void* data)
//{
//	LOG<<WARNING_LOG_LEVEL<<"PingPongBuffer::mapCLToHost: are you sure to map both buffers of a ping pong buffer to host mem?\n";
//	mPingPongBuffers[0]->mapCLToHost(data);
//	mPingPongBuffers[1]->mapCLToHost(data);
//}
//void PingPongBuffer::unmapGL()
//{
//	mPingPongBuffers[0]->unmapGL();
//	mPingPongBuffers[1]->unmapGL();
//}
//void PingPongBuffer::unmapCL()
//{
//	mPingPongBuffers[0]->unmapCL();
//	mPingPongBuffers[1]->unmapCL();
//}






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
