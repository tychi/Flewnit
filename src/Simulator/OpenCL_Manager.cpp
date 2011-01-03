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
	List<cl::Platform> platforms;
	GUARD( mLastCLError = cl::Platform::get(&platforms) );
	LOG<<INFO_LOG_LEVEL<<platforms.size()<<" OpenCL platforms found;\n";

	List<cl::Device> devices;
	if(useCPU)
	{
		GUARD( mLastCLError = platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices) );
		LOG<<INFO_LOG_LEVEL<<devices.size()<<" OpenCL CPU devices found;\n";
		int deviceTypeEnum  = devices.front().getInfo<CL_DEVICE_TYPE>();
		LOG<<INFO_LOG_LEVEL<< "TEST: numeric value of CL_DEVICE_TYPE_CPU: " <<CL_DEVICE_TYPE_CPU
			<<"; numeric value of currently selected device: "<<deviceTypeEnum<<";\n";
		assert(deviceTypeEnum == CL_DEVICE_TYPE_CPU);
	}
	else
	{
		GUARD( mLastCLError = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices) );
		 LOG<<INFO_LOG_LEVEL<<devices.size()<<" OpenCL GPU devices found;\n";
		 int deviceTypeEnum  = devices.front().getInfo<CL_DEVICE_TYPE>();
		LOG<<INFO_LOG_LEVEL<< "TEST: numeric value of CL_DEVICE_TYPE_GPU: " <<CL_DEVICE_TYPE_GPU
				<<"; numeric value of currently selected device: "<<deviceTypeEnum<<";\n";
			assert(deviceTypeEnum == CL_DEVICE_TYPE_GPU);
	}



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


void OpenCL_Manager::checkCLGLErrors()
{
	mLastGLError = glGetError();
	if (mLastGLError  != GL_NO_ERROR )
	{
		LOG<<ERROR_LOG_LEVEL<<" OpenGL Error: "<< oglErrorString(mLastGLError) <<";\n";
	}

	//mLastCLError is set by the app, getting a non const reference to the member;
	//in case of an error, an exception should have already been thrown;
	//but in case of a catch, let's do some "pretty printing" anyway ;( :
	if(mLastCLError != CL_SUCCESS)
	{
		LOG<<ERROR_LOG_LEVEL<<" OpenGL Error: "<< oglErrorString(mLastCLError) <<";\n";
	}

	if (mLastGLError  != GL_NO_ERROR )
	{
		assert("GL ERROR"&&0);
	}
	if(mLastCLError != CL_SUCCESS)
	{
		assert("CL ERROR"&&0);
	}
}


// Helper function to get error string
// *********************************************************************
const char* OpenCL_Manager::oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

const char* OpenCL_Manager::oglErrorString(GLenum error)
{

	String errorString("GL_NO_ERROR");

	if (error != GL_NO_ERROR) {

		switch (error) {
		case GL_INVALID_OPERATION:
			errorString = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			errorString = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorString = "GL_INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			errorString = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		default:
			errorString = "Unknown GL error";
			break;
		}
	}
	return errorString.c_str();
}


}
