/*
 * ParallelComputeManager.h
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"


//forward declarations might be too error prone (to redefine all the typedefs etc.; Forwarding the gl/cl-stuff can be done in the future as a refactoring step);
#include "Common/CL_GL_Common.h"
#include "Common/Math.h"

#ifdef _DEBUG
	//Macro to permanently check for errors in debug mode
#	define GUARD(expression) \
	expression; \
	ParallelComputeManager::getInstancePtr()->checkCLGLErrors()

//macro only to be used by the RenderTarget class internally
#	define GUARD_FRAMEBUFFER(expression) \
		expression; \
		checkFrameBufferErrors(); \
		ParallelComputeManager::getInstancePtr()->checkCLGLErrors()
#else
#	define GUARD(expression) expression
#	define GUARD_FRAMEBUFFER(expression) expression
#endif

//shortcut macro for the OpenCL Manager, as it if often needed;
#define PARA_COMP_MANAGER ParallelComputeManager::getInstancePtr()

namespace Flewnit
{

/*
 * Device infos are stored conveniently in this class in order to calculate appropriate
 * KernelWorkParameters dependent from available Hardware resources and technology;
 * */
class ParallelComputeDeviceInfo
{
public:

	explicit ParallelComputeDeviceInfo(cl::Device* device);
	~ParallelComputeDeviceInfo(){}

	void queryNvidiaSpecificDeviceInfo();
	void print();

	cl::Device* device;

	//the comments show the appropriate OpenCL idendtifieres and the
	//values for a Geforce GT 435M mid range notebook NVIDIA GPU
	String deviceName; //CL_DEVICE_NAME: 			GeForce GT 435M
	String deviceVendor; //CL_DEVICE_VENDOR: 			NVIDIA Corporation
	String driverVersionString; //CL_DRIVER_VERSION: 			270.29
	String openCLVersionString; //CL_DEVICE_VERSION: 			OpenCL 1.0 CUDA

	cl_device_type deviceType; //CL_DEVICE_TYPE:			CL_DEVICE_TYPE_GPU

	cl_GLuint maxComputeUnits; //CL_DEVICE_MAX_COMPUTE_UNITS:		2
	size_t maxWorkGroupSize; //CL_DEVICE_MAX_WORK_GROUP_SIZE:	1024

	cl_GLuint maxClockFrequencyMHz;  // CL_DEVICE_MAX_CLOCK_FREQUENCY:	1300 MHz
	cl_GLuint numAdressBits; //L_DEVICE_ADDRESS_BITS:		32

	cl_ulong maxBufferSizeByte; //  CL_DEVICE_MAX_MEM_ALLOC_SIZE:		255 MByte
	cl_ulong globalMemorySizeByte; //  CL_DEVICE_GLOBAL_MEM_SIZE:		1023 MByte
	cl_device_local_mem_type localMemoryType; //  CL_DEVICE_LOCAL_MEM_TYPE:		CL_LOCAL
	cl_ulong localMemorySizeByte; //  CL_DEVICE_LOCAL_MEM_SIZE:		48 KByte
	cl_ulong  maxConstantBufferSizeByte; //CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:	64 KByte
	cl_ulong globalCacheSizeByte; //CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
	cl_GLuint globalCacheLineSizeByte; //CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE

	cl_command_queue_properties queueProperties; //  CL_DEVICE_QUEUE_PROPERTIES : CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE

	/*  CL_DEVICE_EXTENSIONS:			cl_khr_byte_addressable_store
						cl_khr_icd
						cl_khr_gl_sharing
						cl_nv_compiler_options
						cl_nv_device_attribute_query
						cl_nv_pragma_unroll
						cl_khr_global_int32_base_atomics
						cl_khr_global_int32_extended_atomics
						cl_khr_local_int32_base_atomics
						cl_khr_local_int32_extended_atomics
						cl_khr_fp64
	 */
	std::vector<String> extensionStrings;
	//String extensionsString;

	Vector2Dui nvidiaDeviceComputeCapability; //CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV
	cl_GLuint numTotalNvidiaCudaCores; // NUMBER OF CUDA CORES:			96
	cl_GLuint registersPerNvidiaMultiProcessor; //  CL_DEVICE_REGISTERS_PER_BLOCK_NV:	32768

	//-----------------------------------------------------------
	//rest of the values aren't _that_ interesting at the moment, but this may change
	//with future hardware/drivers and/or other hardware vendors (Intel/AMD; This framework is only tested in NVIDIA hardware with compute ability 1.3  and 2.1)

	//CL_DEVICE_WARP_SIZE_NV:		32
	//CL_DEVICE_GPU_OVERLAP_NV:		CL_TRUE
	//CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV:	CL_TRUE
	//CL_DEVICE_INTEGRATED_MEMORY_NV:	CL_FALSE
	//CL_DEVICE_PREFERRED_VECTOR_WIDTH_<t>	CHAR 1, SHORT 1, INT 1, LONG 1, FLOAT 1, DOUBLE 1

	//CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:	3
	//cl_GLuint maxWorkItemsPerWorkGroupDimension[3]; //  CL_DEVICE_MAX_WORK_ITEM_SIZES:	1024 / 1024 / 64
  	//CL_DEVICE_ERROR_CORRECTION_SUPPORT:	no
	// CL_DEVICE_IMAGE_SUPPORT:		1
	// CL_DEVICE_MAX_READ_IMAGE_ARGS:	128
	// CL_DEVICE_MAX_WRITE_IMAGE_ARGS:	8
	//	CL_DEVICE_SINGLE_FP_CONFIG:		denorms INF-quietNaNs round-to-nearest round-to-zero round-to-inf fma

	//	  CL_DEVICE_IMAGE <dim>	2D_MAX_WIDTH	 4096
	//							2D_MAX_HEIGHT	 32768
	//							3D_MAX_WIDTH	 2048
	//							3D_MAX_HEIGHT	 2048
	//							3D_MAX_DEPTH	 2048
};




class ParallelComputeManager
	: 	public Singleton<ParallelComputeManager>,
		public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	//a cl::Platform member needed?

    cl::Device mUsedDevice;
    ParallelComputeDeviceInfo* mParallelComputeDeviceInfo;

    cl::Context mCLContext;
    cl::CommandQueue mCommandQueue;

    //debugging variables
    cl_int mLastCLError;
    GLenum mLastGLError;
    cl::Event mLastEvent;

    cl_bool mBlockAfterEnqueue;

    //the Manager class tracks all shared buffers, as they shall be acquired
    //at once when passing control from GL to CL; this hopefully improves performance;
    std::vector<cl::Memory> mRegisteredCLGLSharedBuffers;
    bool mCLhasAcquiredSharedObjects;


    bool init(bool useCPU =false);

    const char* oclErrorString(cl_int error);
    const char* oglErrorString(GLenum error);

public:
	ParallelComputeManager( bool useCPU = false)
	:
		//let's be careful first, as there may be serious synch overhead with events etc.
		//when doing asynchronous calls to the GPU
		mBlockAfterEnqueue(CL_TRUE)
	{init(useCPU);}

	virtual ~ParallelComputeManager();

	void checkCLGLErrors();

	const ParallelComputeDeviceInfo& getParallelComputeDeviceInfo()const{return *mParallelComputeDeviceInfo;}

	//modifiable references
	cl::Context& getCLContext();
	cl::CommandQueue& getCommandQueue();
	cl::Device& getUsedDevice();
	//cl::Event& getLastEvent();
	cl::Event* getLastEventPtr();

	//to be called by buffer implementations at the end of memory allocation;
	void registerSharedBuffer(cl::Memory newSharedBuffer);

	//checker for non-conflicting interop:
    inline bool computeIsInControl()const{return  mCLhasAcquiredSharedObjects;}
    inline bool graphicsAreInControl()const{return !mCLhasAcquiredSharedObjects;}

    //aquire/ release all shared buffers to/from the compute world ;)
    void acquireSharedBuffersForCompute();
    void acquireSharedBuffersForGraphics();

    void barrierGraphics();
    void barrierCompute();


	void setBlockAfterEnqueue(cl_bool val){mBlockAfterEnqueue = val;}
	cl_bool getBlockAfterEnqueue()const{return mBlockAfterEnqueue;}

	//accessor for propagating the last error to the framework for further processing (error checking ;()
    //inline cl_int& getLastCLErrorReference(){return mLastCLError;}
    inline cl_int* getLastCLErrorPtr(){return &mLastCLError;}

//    inline cl_int& getLastCLError(){return mLastCLError;}
//    inline GLenum getLastGLError(){return mLastGLError;}

};

}


