/*
 * ParallelComputeManager.cpp
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#include "ParallelComputeManager.h"

#include "UserInterface/WindowManager/WindowManager.h"
#include "Util/Log/Log.h"

#include "boost/foreach.hpp"


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



ParallelComputeDeviceInfo::ParallelComputeDeviceInfo(cl::Device* device)
: device(device)
{
	GUARD(device->getInfo<String>( CL_DEVICE_NAME, &deviceName));
	GUARD(device->getInfo<String>( CL_DEVICE_VENDOR, &deviceVendor));
	GUARD(device->getInfo<String>( CL_DRIVER_VERSION, &driverVersionString));
	GUARD(device->getInfo<String>( CL_DEVICE_VERSION, &openCLVersionString));

	GUARD(device->getInfo<cl_device_type>( CL_DEVICE_TYPE, &deviceType));

	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_MAX_COMPUTE_UNITS, &maxComputeUnits));
//HAXX!111
	maxComputeUnits=2;


	GUARD(device->getInfo<size_t>( CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize));
	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_MAX_CLOCK_FREQUENCY, &maxClockFrequencyMHz));
	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_ADDRESS_BITS, &numAdressBits));

	GUARD(device->getInfo<cl_ulong>( CL_DEVICE_MAX_MEM_ALLOC_SIZE, &maxBufferSizeByte));
	GUARD(device->getInfo<cl_ulong>( CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemorySizeByte));
	GUARD(device->getInfo<cl_device_local_mem_type>( CL_DEVICE_LOCAL_MEM_TYPE, &localMemoryType));
	GUARD(device->getInfo<cl_ulong>( CL_DEVICE_LOCAL_MEM_SIZE, &localMemorySizeByte));
	GUARD(device->getInfo<cl_ulong>( CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &maxConstantBufferSizeByte));
	GUARD(device->getInfo<cl_ulong>( CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &globalCacheSizeByte));
	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, &globalCacheLineSizeByte));
	GUARD(device->getInfo<cl_command_queue_properties>( CL_DEVICE_QUEUE_PROPERTIES, &queueProperties));


	// CL_DEVICE_EXTENSIONS
	bool nv_device_attibute_query_ExtensionSupported = false;
	String extensionsString="";
	GUARD(device->getInfo<String>( CL_DEVICE_EXTENSIONS, &extensionsString));
	//clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
	if (extensionsString != "")
	{
		size_t szOldPos = 0;
		size_t szSpacePos = extensionsString.find(' ', szOldPos); // extensions string is space delimited
		while (szSpacePos != extensionsString.npos) {
			String subString = extensionsString.substr(szOldPos, szSpacePos- szOldPos);
			if (strcmp("cl_nv_device_attribute_query", subString.c_str()) == 0)
				nv_device_attibute_query_ExtensionSupported = true;
			extensionStrings.push_back(subString);
			do {
				szOldPos = szSpacePos + 1;
				szSpacePos = extensionsString.find(' ', szOldPos);
			} while (szSpacePos == szOldPos);
		}
	}

	if (nv_device_attibute_query_ExtensionSupported)
	{queryNvidiaSpecificDeviceInfo();}
	else{
		nvidiaDeviceComputeCapability = Vector2Dui(0,0);
		numTotalNvidiaCudaCores = 0;
		registersPerNvidiaMultiProcessor=0;
	}
}


// Beginning of GPU Architecture definitions
void ParallelComputeDeviceInfo::queryNvidiaSpecificDeviceInfo()
{
	//###################################################################################
		//the cl_ext.h for OpenCL 1.0 doesn't have the vendor specific identifiers;
	//In order to minimize the risk of corruption/loss of synch with standard headers,
	//the NVIDIA specifirc identifieres have been coied from the OpenCL 1.1 gl_ext.h

	/******************************************
	 * cl_nv_device_attribute_query extension *
	 ******************************************/
	/* cl_nv_device_attribute_query extension - no extension #define since it has no functions */
	#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
	#define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV       0x4000
	#endif
	#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV
	#define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV       0x4001
	#endif
	#ifndef CL_DEVICE_REGISTERS_PER_BLOCK_NV
	#define CL_DEVICE_REGISTERS_PER_BLOCK_NV            0x4002
	#endif
	#ifndef CL_DEVICE_WARP_SIZE_NV
	#define CL_DEVICE_WARP_SIZE_NV                      0x4003
	#endif
	#ifndef	CL_DEVICE_GPU_OVERLAP_NV
	#define CL_DEVICE_GPU_OVERLAP_NV                    0x4004
	#endif
	#ifndef CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV
	#define CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV            0x4005
	#endif
	#ifndef CL_DEVICE_INTEGRATED_MEMORY_NV
	#define CL_DEVICE_INTEGRATED_MEMORY_NV              0x4006
	#endif
	//###################################################################################

	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, &(nvidiaDeviceComputeCapability.x)));
	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, &(nvidiaDeviceComputeCapability.y)));

	struct SMPtoNumCudaCoreMap
	{
		cl_GLuint computeCapability;
		cl_GLuint numCores;
	};
	SMPtoNumCudaCoreMap numCoresPerSMParray[] =
	{ { 0x10,  8 },
	  { 0x11,  8 },
	  { 0x12,  8 },
	  { 0x13,  8 },
	  { 0x20, 32 },
	  { 0x21, 48 },
	  {   -1, -1 }
	};
	numTotalNvidiaCudaCores= -1;
	for(int i = 0;numCoresPerSMParray[i].computeCapability != cl_GLuint(-1);i++){
		if (numCoresPerSMParray[i].computeCapability == ((nvidiaDeviceComputeCapability.x << 4) + nvidiaDeviceComputeCapability.y) )
		{	numTotalNvidiaCudaCores = numCoresPerSMParray[i].numCores;
			break;
		}
	}
	if(numTotalNvidiaCudaCores == cl_GLuint(-1)){
		LOG<<ERROR_LOG_LEVEL<<"Unknown Nvidia Device Compute capability: "<<nvidiaDeviceComputeCapability.x<<"."<<nvidiaDeviceComputeCapability.y<<";\n";
		assert(0);
	}
	numTotalNvidiaCudaCores *= maxComputeUnits;

	GUARD(device->getInfo<cl_GLuint>( CL_DEVICE_REGISTERS_PER_BLOCK_NV, &registersPerNvidiaMultiProcessor));
}
// end of GPU Architecture definitions


void ParallelComputeDeviceInfo::print()
{
	LOG<<INFO_LOG_LEVEL<<"deviceName:\t\t\t"<< deviceName  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"deviceVendor:\t\t\t"<< deviceVendor  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"driverVersionString:\t\t"<< driverVersionString  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"openCLVersionString:\t\t"<< openCLVersionString  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"maxComputeUnits:\t\t\t"<< maxComputeUnits  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"maxWorkGroupSize:\t\t\t"<< maxWorkGroupSize  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"maxClockFrequencyMHz:\t\t"<< maxClockFrequencyMHz  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"numAdressBits:\t\t\t"<< numAdressBits  <<"\n";

	LOG<<INFO_LOG_LEVEL<<"maxBufferSizeByte:\t\t"<< maxBufferSizeByte  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"globalMemorySizeByte:\t\t\t"<< globalMemorySizeByte  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"localMemoryType:\t\t\t"<< ((localMemoryType==CL_LOCAL)?"CL_LOCAL":"CL_GLOBAL")  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"localMemorySizeByte:\t\t"<< localMemorySizeByte  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"maxConstantBufferSizeByte:\t\t"<< maxConstantBufferSizeByte  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"globalCacheSizeByte:\t\t"<< globalCacheSizeByte  <<"\n";
	LOG<<INFO_LOG_LEVEL<<"globalCacheLineSizeByte:\t\t"<< globalCacheLineSizeByte  <<"\n";

	if (queueProperties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE)
		LOG << INFO_LOG_LEVEL << "CL_DEVICE_QUEUE_PROPERTIES:\t"<< "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE" << "\n";
	if (queueProperties & CL_QUEUE_PROFILING_ENABLE)
		LOG << INFO_LOG_LEVEL << "CL_DEVICE_QUEUE_PROPERTIES:\t"<< "CL_QUEUE_PROFILING_ENABLE" << "\n";

	LOG << INFO_LOG_LEVEL << "supported OpenCL Extensions:\n";
	BOOST_FOREACH(String extString, extensionStrings)
	{
		LOG << INFO_LOG_LEVEL << "\t\t" << extString << "\n";
	}

	LOG << INFO_LOG_LEVEL << "Nvidia compute capability:\t\t\t"<< nvidiaDeviceComputeCapability.x << "." << nvidiaDeviceComputeCapability.y << "\n";
	LOG << INFO_LOG_LEVEL << "numTotalNvidiaCudaCores:\t\t\t"<< numTotalNvidiaCudaCores << "\n";
	LOG << INFO_LOG_LEVEL << "registersPerNvidiaMultiProcessor:\t\t"<< registersPerNvidiaMultiProcessor << "\n";
}



//===================================================================================

ParallelComputeManager::ParallelComputeManager( bool useCPU)
:
	//let's be careful first, as there may be serious synch overhead with events etc.
	//when doing asynchronous calls to the GPU
	   mLastCLError(CL_SUCCESS),
	   mLastGLError(GL_NO_ERROR),
		mBlockAfterEnqueue(CL_TRUE),
	    mCLhasAcquiredSharedObjects(false)
{init(useCPU);}

ParallelComputeManager::~ParallelComputeManager()
{
	delete mParallelComputeDeviceInfo;
	// nothing else to clean up, as no pointer members exist, and the cl-c++-binding makes the cleanup for itself;
}


bool ParallelComputeManager::init(bool useCPU)
{
	//we need a valid openGL context
	assert(WindowManager::getInstance().openGLContextIsCreated());

	/*
	 * Code ideas from "Adventures with OpenCL", part2 from enja;
	 *
	 * */

	std::vector<cl::Platform> platforms;
	GUARD( mLastCLError = cl::Platform::get(&platforms) );
	LOG<<INFO_LOG_LEVEL<<platforms.size()<<" OpenCL platforms found;\n";

	std::vector<cl::Device> devices;
	if(useCPU)
	{
		LOG<<WARNING_LOG_LEVEL<<"Using CPU instead of GPU; Expect a heavy performance impact and other implementation issues like crashes! ;(\n";
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

	mParallelComputeDeviceInfo = new ParallelComputeDeviceInfo( &mUsedDevice );
	mParallelComputeDeviceInfo->print();


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
                assert(0);
            }
        #endif
    #endif

    //create the command queue we will use to execute OpenCL commands
    try{
        mCommandQueue = cl::CommandQueue(mCLContext, mUsedDevice, 0, &mLastCLError);
    }
    catch (cl::Error er) {
        printf("ERROR: %s(%d)\n", er.what(), er.err());
        assert(0);
    }


	return true;
}


//to be called by buffer implementations at the end of memory allocation;
void ParallelComputeManager::registerSharedBuffer(cl::Memory newSharedBuffer)
{
	mRegisteredCLGLSharedBuffers.push_back(newSharedBuffer);
	checkCLGLErrors();
}


void ParallelComputeManager::acquireSharedBuffersForCompute()
{
	//skip if not necessary;
	if(computeIsInControl()) return;

	//the stupidest and slowes way of synchronization:
	//TODO make this more efficient
	///\{
	//force to wait for all GL commands to complete
	barrierGraphics();
	//force to wait for all other CL commands to complete;
	//barrierCompute();
	///\}


	GUARD(
		mLastCLError = mCommandQueue.enqueueAcquireGLObjects(
			& mRegisteredCLGLSharedBuffers,
			//TODO maybe manage a set of events in order for more efficient synch;
			//at the moment, we
			0
			//,& mLastEvent
			)
	);

	mCLhasAcquiredSharedObjects = true;


}

void ParallelComputeManager::acquireSharedBuffersForGraphics()
{
	//skip if not necessary;
	if(graphicsAreInControl()) return;

	//the stupidest and slowes way of synchronization:
	//TODO make this more efficient
	//maybe TODO: in case of an inexcplicable bug, try glFinish() here
	///\{
	//force to wait for all other CL commands to complete;
	barrierCompute();
	//nb: no synch with GL seems necessary, as during compute stuff (at first) no GL stuff should be active anyway;
	//in suspicion of driver bugs, let's glFinish() anyway at first; TODO remove when stable;
	//barrierGraphics();
	///\}

	GUARD(
		mLastCLError = mCommandQueue.enqueueReleaseGLObjects(
			& mRegisteredCLGLSharedBuffers,
			//TODO maybe manage a set of events in order for more efficient synch;
			//at the moment, we
			0
			//,& mLastEvent
			)
	);

	mCLhasAcquiredSharedObjects = false;

}


void ParallelComputeManager::barrierGraphics()
{
	GUARD(glFinish());
}
void ParallelComputeManager::barrierCompute()
{
	GUARD(mCommandQueue.enqueueBarrier());
}


cl::Context& ParallelComputeManager::getCLContext()
{
	return mCLContext;
}
cl::CommandQueue& ParallelComputeManager::getCommandQueue()
{
	return mCommandQueue;
}

cl::Device& ParallelComputeManager::getUsedDevice()
{
	return mUsedDevice;
}

//cl::Event& ParallelComputeManager::getLastEvent()
//{
//	return mLastEvent;
//}

cl::Event* ParallelComputeManager::getLastEventPtr()
{
	return &mLastEvent;
}


void ParallelComputeManager::checkCLGLErrors()
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
const char* ParallelComputeManager::oclErrorString(cl_int error)
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

const char* ParallelComputeManager::oglErrorString(GLenum error)
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
