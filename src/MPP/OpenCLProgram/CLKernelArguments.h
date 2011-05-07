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
	//: 	public BasicObject
{
		//FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLKernelWorkLoadParams(
			//if work load params aren't know a priori, default params help maskin validation
			cl_GLuint numTotalWorkItems = 0,
			cl_GLuint numWorkItemsPerWorkGroup =0
		);
		virtual ~CLKernelWorkLoadParams(){}

		const CLKernelWorkLoadParams& operator=( const CLKernelWorkLoadParams& rhs)
		{mNumTotalWorkItems= rhs.mNumTotalWorkItems; mNumWorkItemsPerWorkGroup= rhs.mNumWorkItemsPerWorkGroup; return *this;}

	protected:
		friend class CLKernel;
		friend class CLKernelArgumentBase;

		//there may be some __attribute__((reqd_work_group_size(...)))
		//definitions in the kernel; check that this doesn't conflict with the passed values;
		//also check common stuff like that the mNumWorkItemsPerWorkGroup is a power of two;
		void validateAgainst(CLKernel* kernel)const throw(SimulatorException);

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

	//note if no argValuePtr is passed, this may be in indicator that this argument list is for
	//generic" programs with frequently changing arguments;
	//The user hat to set the unititialized arguments manually, else an exception is thrown
	CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr = 0);

	//if mArgValuePtr is still 0, then throw exception
	virtual void passArgToKernel(cl_uint argIndex, CLKernel* clKernel)throw(BufferException);


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

		T getValue()const{return mValue;}
		//CLKernelArgumentBase::mArgValuePtr directly points to mValue; nothing to re-point ;)
		void setValue(T val){ mValue = val; }
protected:

		T mValue;
};



class CLBufferKernelArgument
 : public CLKernelArgumentBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//param ifPingPongBufferUsedInactiveOne delegates if in case a ping pong buffer is passed,
	//explicitly the INactive buffer component shall be bound as kernel arg!
	CLBufferKernelArgument(String argName, BufferInterface* buffi,  bool ifPingPongBufferUseInactiveOne = false );
	virtual ~CLBufferKernelArgument( ){}

	inline BufferInterface* get()const{return mBufferInterface;}
	//CLKernelArgumentBase::mArgValuePtr has to be altered;
	void set(BufferInterface* buffi, bool ifPingPongBufferUseInactiveOne = false );

protected:
	//override for buffer argument to handle that always the respective active/inactive ping pong component,
	//if the buffer is a ping pong buffer;
	virtual void passArgToKernel(cl_uint argIndex, CLKernel* clKernel)throw(BufferException);

	BufferInterface* mBufferInterface;
	bool mIfPingPongBufferUseInactiveOne;

	//With the CL C++ API and its "const *& operator()() const" wtf stuff,
	//I get segfaults or "& needs lvalue" compiler errors;
	//Hence, i just save a copy to the unwrapped cl_mem handle; this variable
	//cannot be destroyed as temporary and connot have any const*const&const()const-fuckup;
	cl_mem mCurrentCLMemoryHandle;
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
	void validateAgainst(CLKernel* kernel)const throw(SimulatorException);
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
