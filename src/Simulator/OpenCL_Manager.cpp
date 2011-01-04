/*
 * OpenCL_Manager.cpp
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#include "OpenCL_Manager.h"

#include "Util/Log/Log.h"

#if defined __APPLE__ || defined(MACOSX)
#else
    #if defined WIN32
    #else
        //needed for context sharing functions
        #include <GL/glx.h>
    #endif
#endif

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
	/*
	 * Code ideas from "Adventures with OpenCL, part2 from enja;
	 * */

	List<cl::Platform> platforms;
	GUARD( mLastCLError = cl::Platform::get(&platforms) );
	LOG<<INFO_LOG_LEVEL<<platforms.size()<<" OpenCL platforms found;\n";

	List<cl::Device> devices;
	if(useCPU)
	{
		LOG<<WARNING_LOG_LEVEL<<"Using CPU instead of GPU; Expect a heavy performance impact!\n";
		//code crashes already here; maybe it's an nvidia driver's problem...
		GUARD( mLastCLError = platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices) );
		LOG<<INFO_LOG_LEVEL<<devices.size()<<" OpenCL CPU devices found;\n";
		int deviceTypeEnum  = devices.front().getInfo<CL_DEVICE_TYPE>();
		LOG<<INFO_LOG_LEVEL<< "TEST: numeric value of CL_DEVICE_TYPE_CPU: " <<CL_DEVICE_TYPE_CPU
			<<"; numeric value of currently selected device: "<<deviceTypeEnum<<";\n";
		assert(deviceTypeEnum == CL_DEVICE_TYPE_CPU && devices.size()>0);
	}
	else
	{
		GUARD( mLastCLError = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices) );
		 LOG<<INFO_LOG_LEVEL<<devices.size()<<" OpenCL GPU devices found;\n";
		 int deviceTypeEnum  = devices.front().getInfo<CL_DEVICE_TYPE>();
		LOG<<INFO_LOG_LEVEL<< "TEST: numeric value of CL_DEVICE_TYPE_GPU: " <<CL_DEVICE_TYPE_GPU
				<<"; numeric value of currently selected device: "<<deviceTypeEnum<<";\n";
			assert(deviceTypeEnum == CL_DEVICE_TYPE_GPU && devices.size()>0);
	}

	mUsedDevice = devices.front();
	int deviceType = useCPU ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU;


    // Define OS-specific context properties and create the OpenCL context
    // We setup OpenGL context sharing slightly differently on each OS
    // this code comes mostly from NVIDIA's SDK examples
    // we could also check to see if the device supports GL sharing
    // but that is just searching through the properties
    // an example is avaible in the NVIDIA code
    #if defined (__APPLE__) || defined(MACOSX)
        CGLContextObj kCGLContext = CGLGetCurrentContext();
        CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
        cl_context_properties props[] =
        {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
            0
        };
        //Apple's implementation is weird, and the default values assumed by cl.hpp don't work
        //this works
        //cl_context cxGPUContext = clCreateContext(props, 0, 0, NULL, NULL, &err);
        //these dont
        //cl_context cxGPUContext = clCreateContext(props, 1,(cl_device_id*)&devices.front(), NULL, NULL, &err);
        //cl_context cxGPUContext = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
        //printf("error? %s\n", oclErrorString(err));
        try{
        	mCLContext = cl::Context(props);   //had to edit line 1448 of cl.hpp to add this constructor
        }
        catch (cl::Error er) {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
    #else
        #if defined WIN32 // Win32
            cl_context_properties props[] =
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                0
            };
            //cl_context cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &err);
            try{
            	mCLContext = cl::Context(deviceType, props);
            }
            catch (cl::Error er) {
                printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
            }
        #else
            cl_context_properties props[] =
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
                CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                0
            };
            //cl_context cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &err);
            try{
            	mCLContext = cl::Context(deviceType, props);
            }
            catch (cl::Error er) {
                printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
            }
        #endif
    #endif

    //create the command queue we will use to execute OpenCL commands
    try{
        mCommandQueue = cl::CommandQueue(mCLContext, mUsedDevice, 0, &mLastCLError);
    }
    catch (cl::Error er) {
        printf("ERROR: %s(%d)\n", er.what(), er.err());
    }


	return true;
}


//to be called by buffer implementations at the end of memory allocation;
void OpenCL_Manager::registerSharedBuffer(cl::Memory newSharedBuffer)
{
	mRegisteredCLGLSharedBuffers.push_back(newSharedBuffer);
	checkCLGLErrors();
}


void OpenCL_Manager::acquireSharedBuffersForCompute()
{
	//skip if not necessary;
	if(computeIsInControl()) return;

	//the stupidest and slowes way of synchronization:
	//TODO make this more efficient
	///\{
	//force to wait for all GL commands to complete
	GUARD(glFinish());
	//force to wait for all other CL commands to complete;
	GUARD(mCommandQueue.enqueueBarrier());
	///\}


	GUARD(
		mLastCLError = mCommandQueue.enqueueAcquireGLObjects(
			& mRegisteredCLGLSharedBuffers,
			//TODO maybe manage a set of events in order for more efficient synch;
			//at the moment, we
			0,
			& mLastEvent
			)
	);

	mCLhasAcquiredSharedObjects = true;


}

void OpenCL_Manager::acquireSharedBuffersForGraphics()
{
	//skip if not necessary;
	if(graphicsAreInControl()) return;

	//the stupidest and slowes way of synchronization:
	//TODO make this more efficient
	///\{
	//force to wait for all other CL commands to complete;
	//nb: no synch with GL seems necessary
	GUARD(mCommandQueue.enqueueBarrier());
	///\}

	GUARD(
		mLastCLError = mCommandQueue.enqueueReleaseGLObjects(
			& mRegisteredCLGLSharedBuffers,
			//TODO maybe manage a set of events in order for more efficient synch;
			//at the moment, we
			0,
			& mLastEvent
			)
	);

	mCLhasAcquiredSharedObjects = false;

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
