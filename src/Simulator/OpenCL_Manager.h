/*
 * OpenCL_Manager.h
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"


//forward declarations might be too error prone (to redefine all the typedefs etc.; Forwarding the gl/cl-stuff can be done in the future as a refactoring step);
#include "Common/CL_GL_Common.h"

#ifdef _DEBUG
	//Macro to permanently check for errors in debug mode
#	define GUARD(expression) \
	expression; \
	OpenCL_Manager::getInstancePtr()->checkCLGLErrors()
#else
#	define GUARD(expression) expression
#endif

//shortcut macro for the OpenCL Manager, as it if often needed;
#define CLMANAGER OpenCL_Manager::getInstancePtr()

namespace Flewnit
{

class OpenCL_Manager
	: 	public Singleton<OpenCL_Manager>,
		public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	//a cl::Platform member needed?

    cl::Device mUsedDevice;

    cl::Context mCLContext;
    cl::CommandQueue mCommandQueue;

    //debugging variables
    cl_int mLastCLError;
    GLenum mLastGLError;
    cl::Event mLastEvent;

    cl_bool mBlockAfterEnqueue;



    bool init(bool useCPU =false);

    const char* oclErrorString(cl_int error);
    const char* oglErrorString(GLenum error);

public:
	OpenCL_Manager( bool useCPU = false)
	:
		//let's be careful first, as there may be serious synch overhead with events etc.
		//when doing asynchronous calls to the GPU
		mBlockAfterEnqueue(CL_TRUE)
	{init(useCPU);}

	virtual ~OpenCL_Manager();

	void checkCLGLErrors();

	//modifiable references
	cl::Context& getCLContext();
	cl::CommandQueue& getCommandQueue();
	cl::Device& getUsedDevice();
	cl::Event& getLastEvent();

	void setBlockAfterEnqueue(cl_bool val){mBlockAfterEnqueue = val;}
	cl_bool getBlockAfterEnqueue()const{return mBlockAfterEnqueue;}

    inline cl_int& getLastCLError(){return mLastCLError;}
    inline GLenum getLastGLError(){return mLastGLError;}



};

}


