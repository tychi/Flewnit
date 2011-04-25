/*
 * CLKernelArguments.h
 *
 * Helper classes to be able to reference values by their name intead of their index;
 * Every Kernel stores an instance of CLKernelArguments, and before any kernel invocation,
 * the member vector is iterated and set via clSetKernelArg(..);
 * If arguments are subject to change between frames, The user can set an argument directly
 * by referencig it by its name, without being bothered with its index;
 *
 *  Created on: Apr 24, 2011
 *      Author: tychi
 */

#pragma once


#include "Common/BasicObject.h"
#include "Buffer/BufferSharedDefinitions.h"



namespace Flewnit
{



class CLKernelArgumentBase
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	virtual ~CLKernelArgumentBase( ){}

protected:
	friend class CLKernelArguments;

	CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr);

	void passArgToKernel(CLKernel* clKernel);


	String mArgName;
	cl_uint mArgIndex; //set by CLKernelArguments;
	size_t mArgSize;
	void* mArgValuePtr;

};

template<typename T>
class CLValueKernelArgument
		: public CLKernelArgumentBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
		CLValueKernelArgument(String argName, T argValue)
		: CLKernelArgumentBase( argName, sizeof(T), &mValue ),
		   mValue(argValue)
		{}
		virtual ~CLValueKernelArgument( ){}

		inline T getValue()const{return mValue;}
		inline void setValue(T val)const{mValue = val;}
protected:
		T mValue;
};



class CLBufferKernelArgument
 : public CLKernelArgumentBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	CLBufferKernelArgument(String argName, BufferInterface* buffi);
	virtual ~CLBufferKernelArgument( ){}

	inline BufferInterface* getBufferInterface()const{return mBufferInterface;}
	inline void setBufferInterface(BufferInterface* buffi ){mBufferInterface = buffi;}
protected:
	BufferInterface* mBufferInterface;
};



class CLKernelArguments
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	CLKernelArguments(const std::vector< CLKernelArgumentBase* > & argVec);
	virtual ~CLKernelArguments( );

	CLKernelArgumentBase* getArg(unsigned int i) throw(BufferException);
	CLKernelArgumentBase* getArg(String argName) throw(BufferException);

private:

	std::vector< CLKernelArgumentBase* > mArgVec;
	std::map<String, CLKernelArgumentBase* > mArgMap;

};

}
