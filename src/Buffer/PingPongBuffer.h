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

	BufferInfo mBufferInfo;
	BufferInterface* mPingPongBuffers[2];
	int mRecentlyUpdatedBufferIndex;
	int mCurrentActiveBufferIndex;


public:
	void toggleBuffers();
	BufferInterface* getRecentlyUpdatedBuffer()const;
	BufferInterface* getCurrentActiveBuffer()const;



	virtual bool operator==(const BufferInterface& rhs) const;

	virtual bool isAllocated(ContextType type) const;
	virtual bool allocMem(ContextType type);
	virtual bool freeMem(ContextType type) ;

	virtual void bind(ContextType type) ;
	//virtual void unBind()=0;

	virtual BufferTypeFlags getBufferTypeFlags()const ;
	virtual String getName() const ;

	virtual void setData(void* data, ContextType type);

	virtual int  getNumElements() const;
	virtual size_t  getElementSize() const;
	virtual Type getElementType() const;
	//virtual cl_GLenum getElementInternalFormat() const;

	virtual bool isPingPongBuffer()const;


	virtual const BufferInfo& getBufferInfo() const;

private:
	void checkPingPongError()const;

};

}

