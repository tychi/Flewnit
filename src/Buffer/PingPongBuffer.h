/*
 * PingPongBuffer.h
 *
 * Convention throughout the whole framework:
 * The buffer that has been least recently written is the active buffer;
 * This means, that BEFORE a new write to a ping pong buffer, the programmer is responsible to do the toggle
 * right befor the write, so that after the write, the active buffer is automatically the most recently
 * written one again;
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
	int mInactiveBufferIndex;
	int mActiveBufferIndex;


public:
	//returns pointer to itself for convenience;
	PingPongBuffer* toggleBuffers();
	BufferInterface* getActiveBuffer()const;
	BufferInterface* getInactiveBuffer()const;



	virtual bool operator==(const BufferInterface& rhs) const;
	//copy contents of the one buffer to the other, but only if they are of the same leaf type;
	//virtual const BufferInterface& operator=(const BufferInterface& rhs) throw(BufferException);



	//virtual void setData(const void* data, ContextTypeFlags where)throw(BufferException);
	//read back form the currently active buffer
	//virtual void readBack()throw(BufferException);


	//virtual void bind(ContextType type) ;
	//virtual void unBind()=0;


protected:

#	include "BufferVirtualSignatures.h"


//	//wrapper functions to GL and CL calls without any error checking,
//	//i.e. semantic checks/flag delegation/verifiaction must be done before those calls;
//	//those routines are introduced to reduce boilerplate code;
//	virtual void generateGL();
//	virtual void generateCL();
//
//	virtual void bindGL();
//	virtual void allocGL();
//
//	virtual void writeGL(const void* data);
//	virtual void writeCL(const void* data);
//	virtual void readGL(void* data);
//	virtual void readCL(void* data);
//	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom);
//	virtual void copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom);
//	virtual void freeGL();
//	virtual void freeCL();
////	virtual void* mapGLToHost();
////	virtual void* mapCLToHost();
////	virtual void unmapGL();
////	virtual void unmapCL();



private:
	void checkPingPongError()const;
	void getHandlesFromCurrentActiveBuffer();

};

}

