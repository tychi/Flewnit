/*
 * OpenCL_Manager.cpp
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#include "OpenCL_Manager.h"

#include "Util/Log/Log.h"

namespace Flewnit
{

//OpenCL_Manager::OpenCL_Manager( bool useCPU )
//{
//	// TODO Auto-generated constructor stub
//
//}

OpenCL_Manager::~OpenCL_Manager()
{
	// TODO Auto-generated destructor stub
}


bool OpenCL_Manager::init(bool useCPU)
{
	//TODO
	LOG<<WARNING_LOG_LEVEL<<" OpenCL Stuff not yet implemented;\n";
	return true;
}


cl::Context& OpenCL_Manager::getCLContext()
{
	return mCLContext;
}
cl::CommandQueue& OpenCL_Manager::getCommandQueue()
{
	return mCommandQueue;
}

cl::Device& OpenCL_Manager::getUsedDevice()
{
	return mUsedDevice;
}
cl::Event& OpenCL_Manager::getLastEvent()
{
	return mLastEvent;
}

}
