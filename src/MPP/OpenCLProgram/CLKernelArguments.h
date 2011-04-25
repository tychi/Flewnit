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
#include "Simulator/SimulatorMetaInfo.h"



namespace Flewnit
{


class CLKernelWorkLoadParams
	: 	public BasicObject
{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLKernelWorkLoadParams(
			cl_GLuint numTotalWorkItems,
			cl_GLuint numWorkItemsPerWorkGroup
		);
		virtual ~CLKernelWorkLoadParams(){}

	private:
		friend class CLKernel;

		//there may be some __attribute__((reqd_work_group_size(...)))
		//definitions in the kernel; check that this doesn't conflict with the passed values;
		//also check common stuff like that the mNumWorkItemsPerWorkGroup is a power of two;
		void validateAgainst(CLKernel* kernel)throw(SimulatorException);

		cl_GLuint mNumTotalWorkItems;
		cl_GLuint mNumWorkItemsPerWorkGroup;

};


//-------------------------------------------------------------------------------------------


class CLKernelArgumentBase
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	virtual ~CLKernelArgumentBase( ){}

protected:
	friend class CLKernelArguments;

	CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr);

	void passArgToKernel(cl_uint argIndex, CLKernel* clKernel);


	String mArgName;
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
		//CLKernelArgumentBase::mArgValuePtr directly points to mValue; nothing to re-point ;)
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

	inline BufferInterface* get()const{return mBufferInterface;}
	void set(BufferInterface* buffi );

protected:
	BufferInterface* mBufferInterface;
};



class CLKernelArguments
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	//make sure the kernels' signature fits exactly the type, number and order of values
	//to be passed; Unfortunately, the OpenCL API has no way to query parameter names or types
	//of kernel functions, hence there is a huge danger of malfunction without explicit
	//error generation; be especially careful when creating these argument lists!
	//ALWAYS keep it in synch with the openCL code!!!11
	CLKernelArguments(const std::vector< CLKernelArgumentBase* > & argVec);
	virtual ~CLKernelArguments();

	//throw exception is arg with name doesn't exist, i < mArgVec.size,
	//if a bad cast occured;
	CLBufferKernelArgument* getBufferArg(unsigned int i) throw(BufferException);
	CLBufferKernelArgument* getBufferArg(String argName) throw(BufferException);

	template<typename T>
	CLValueKernelArgument<T>* getValueArg(unsigned int i) throw(BufferException);
	template<typename T>
	CLValueKernelArgument<T>* getValueArg(String argName) throw(BufferException);


private:

	friend class CLKernel;

	//at least verify the argument count ;( other info related to a kernel argument list
	//is not available yet :((
	void validateAgainst(CLKernel* kernel)throw(SimulatorException);
	void passArgsToKernel(CLKernel* clKernel);

	CLKernelArgumentBase* getArg(unsigned int i) throw(BufferException);
	CLKernelArgumentBase* getArg(String argName) throw(BufferException);

	std::vector< CLKernelArgumentBase* > mArgVec;
	std::map<String, CLKernelArgumentBase* > mArgMap;

};


template<typename T>
CLValueKernelArgument<T>* CLKernelArguments::getValueArg(unsigned int i) throw(BufferException)
{
	CLValueKernelArgument<T>* casted = dynamic_cast< CLValueKernelArgument<T>* >(getArg(i));
	if(!casted)
	{
		throw(BufferException(
				String("CLKernelArguments::getValueArg(unsigned int i): bad cast!\n")
		));
	}

	return casted;
}

template<typename T>
CLValueKernelArgument<T>* CLKernelArguments::getValueArg(String argName) throw(BufferException)
{
	CLValueKernelArgument<T>* casted = dynamic_cast< CLValueKernelArgument<T>* >(getArg(argName));
	if(!casted)
	{
		throw(BufferException(
				String("CLKernelArguments::getValueArg(String argName): bad cast!\n")
		));
	}

	return casted;
}


}
