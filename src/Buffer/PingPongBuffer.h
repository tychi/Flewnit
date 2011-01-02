/*
 * PingPongBuffer.h
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


class PingPongBuffer
: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	PingPongBuffer(String name,BufferInterface* ping, BufferInterface* pong ) throw(BufferException);
	virtual ~PingPongBuffer();

protected:

	BufferInterface* mPingPongBuffers[2];
	int mRecentlyUpdatedBufferIndex;
	int mCurrentActiveBufferIndex;


public:
	void toggleBuffers();
	BufferInterface* getRecentlyUpdatedBuffer()const;
	BufferInterface* getCurrentActiveBuffer()const;



	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);


	//if you are calling the following routines on the pingpong-buffer, the every operation will be performed
	//on both of the managed buffers ("ping" and "pong"); to do stuff just on one of the both managed buffers,
	//get them and call those routines driectly on them
	virtual bool allocMem(ContextType type)throw(BufferException);
	virtual void setData(void* data, ContextType type);
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;
	//virtual void unBind()=0;






private:
	void checkPingPongError()const;
	void getHandlesFromCurrentActiveBuffer();

};

}

