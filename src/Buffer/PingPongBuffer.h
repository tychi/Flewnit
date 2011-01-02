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

	PingPongBuffer(String name,BufferInterface* ping, BufferInterface* pong );
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



	virtual bool allocMem(ContextType type);
	virtual void setData(void* data, ContextType type);
	virtual bool copyBetweenContexts(ContextType from,ContextType to)throw(BufferException);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;
	//virtual void unBind()=0;






private:
	void checkPingPongError()const;

};

}

