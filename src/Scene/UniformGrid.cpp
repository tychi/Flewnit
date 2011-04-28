/*
 * UniformGrid.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "UniformGrid.h"

#include "Simulator/SimulationResourceManager.h"

#include "Simulator/ParallelComputeManager.h"
#include "Buffer/Buffer.h"
#include "MPP/OpenCLProgram/CLProgram.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"
#include "Util/HelperFunctions.h"



namespace Flewnit
{


UniformGridBufferSet::UniformGridBufferSet(String name, unsigned int numCellsPerDimension)
:
	//generic simulation domain, becaus a uniform grid has many purposes :P
	SimulationObject(name, GENERIC_SIM_DOMAIN),

	mStartIndices(
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			name + String("StartIndices"),
			//4 bytes per index * numCellsPerDimension^3
			4 * numCellsPerDimension* numCellsPerDimension *numCellsPerDimension,
			true
		)
	),
	mElementCounts(
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			name + String("ElementCounts"),
			//4 bytes per index * numCellsPerDimension^3
			4 * numCellsPerDimension* numCellsPerDimension *numCellsPerDimension,
			true
		)
	)
{
	//init CPU component of elem count buffer to zero;
	memset( mElementCounts->getCPUBufferHandle(), 0, mElementCounts->getBufferInfo().bufferSizeInByte );
	mElementCounts->copyFromHostToGPU();

	//actually unneccessary to init startindices, but to be sure...
	memset( mStartIndices->getCPUBufferHandle(), 0, mStartIndices->getBufferInfo().bufferSizeInByte );
	mStartIndices->copyFromHostToGPU();
}


UniformGridBufferSet::~UniformGridBufferSet()
{
	//nothing to do
}

//Before calling UniformGrid::updateCells(), the element counts mus be zero everywhere;
//this is why we have to clear the buffer before;
//The returned event is needed for the kernel_updateUniformGrid to wait for the transfer to finish;
//after execution of kernel "kernel_updateForce_integrate_calcZIndex", the uniform grid buffers
//aren't needed before the next "kernel_updateUniformGrid" invocation;
//between the two calls, radix sort happens; we can hide the buffer write latency by
//enqueue the write before invoking radix sort kernels;
//The "kernel_updateUniformGrid" waiting for the mClearElementCountEvent makes security "perfect" ;).
cl::Event UniformGridBufferSet::clearElementCounts()
{
	//son't use the BufferInterface abstraction here, because it does'nt allow event stuff
	//adding this event stuff wouln't be taht hard, but time pressure and the fact that we need it only once
	//in this simulation allows me to legitimate this hack ;)

	//set CPU component to zero; TODO can be omitted here if it is never written;
	//n.B.: this all is a hack due to lack of API functions.
	//why is there no clClear? This sucks, to have to transfer
	//dozens of megabytes of zeros to a bandwidth limitied device :@
	memset( mElementCounts->getCPUBufferHandle(), 0, mElementCounts->getBufferInfo().bufferSizeInByte );
	cl_bool blockGlobalTmp = PARA_COMP_MANAGER->getBlockAfterEnqueue();

	//---------------------------------------------------
	EventVector eventVec;
	cl::Event initial_kernelEvent = CLProgramManager::getInstance().getProgram("updateForce_integrate_calcZIndex.cl")->
			getKernel("kernel_updateForce_integrate_calcZIndex")->getEventOfLastKernelExecution();
	cl::Event default_kernelEvent = CLProgramManager::getInstance().getProgram("_initial_updateForce_integrate_calcZIndex.cl")->
			getKernel("kernel_initial_updateForce_integrate_calcZIndex")->getEventOfLastKernelExecution();

	//the kernels may not have been enqueueud yet; filter them out (zero indicates this);
	if(default_kernelEvent()  != 0)	{
		//we are in a pass > 1
		eventVec.push_back(default_kernelEvent);
	}
	else{
		if( initial_kernelEvent() != 0 ){
			//we are in pass 1; dependency: initial physics copmutation kernel
			eventVec.push_back(initial_kernelEvent);
		}else{
			//we are in pass 0; zero dependencies here
		}
	}
	//-------------------------------------------------


	PARA_COMP_MANAGER->getCommandQueue().enqueueWriteBuffer(
			static_cast<const cl::Buffer&>( mElementCounts->getComputeBufferHandle() ),
			CL_TRUE, //to omit as much bus as possible, despite all precautions and design for asynchronous stuff,
				  //lets stay synchrounous as long as the simulation doesn't work stable;
			//CL_FALSE, // TODO don't block
			0,
			mElementCounts->getBufferInfo().bufferSizeInByte,
			mElementCounts->getCPUBufferHandle(),
			//wait for inital or default versions of SPH force+integr+zindex "kernel_updateForce_integrate_calcZIndex"
			& eventVec,
			& mClearElementCountEvent
	);

	*(PARA_COMP_MANAGER->getLastEventPtr()) = mClearElementCountEvent;

	//restore global blocking stuff
	PARA_COMP_MANAGER->setBlockAfterEnqueue(blockGlobalTmp);

	return mClearElementCountEvent;
}



//##########################################################################################

UniformGrid::UniformGrid(
	String name,
	unsigned int numCellsPerDimension,

	Vector4D minCornerPosition,
	Vector4D extendsOfOneCell,
	//GPU-relevant "chunk-ization" size, denoting the max. element count processed
	//by on work group; default: 32
	unsigned int numMaxElementsPerSimulationWorkGroup
)
: AccelerationStructure(name, UNIFORM_GRID ),

  mNumCellsPerDimension(numCellsPerDimension),
  mMinCornerPosition(minCornerPosition),
  mExtendsOfOneCell(extendsOfOneCell),

  mZIndexLookupTable(
	SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
		//3 dimensions * 4 bytesPerDimension
		"zIndexLookupTable", 3 * 4 * numCellsPerDimension, true)
  ),

  mUniformGridBufferSet( new UniformGridBufferSet(name + String("BufferSet"), numCellsPerDimension) ),
  mUpdateUniformGridProgram(0), //init to zero to be sure that all members are initialized first
  mNumMaxElementsPerSimulationWorkGroup(numMaxElementsPerSimulationWorkGroup),
  mSplitAndCompactUniformGridProgram(0) //init to zero to be sure that all members are initialized first

{
	setupZIndexLookUpTable();

//TODO
//	createAndAddDebugDrawGeometry();

//	mUpdateUniformGridProgram = new UpdateUniformGridProgram(this);
//	mSplitAndCompactUniformGridProgram = new SplitAndCompactUniformGridProgram(this);
//
//
}

UniformGrid::~UniformGrid()
{
	delete mUniformGridBufferSet;
	//nothing else to delete;

}



void UniformGrid::updateCells(PingPongBuffer* sortedZIndicesKeyBuffer)
{
	//TODO
	assert(0&&"TODO implement");

}

unsigned int UniformGrid::splitAndCompactCells(UniformGridBufferSet* compactionResultBufferSet)
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}

void UniformGrid::setupZIndexLookUpTable() //called by constructor
{
	assert( HelperFunctions::isPowerOfTwo( mNumCellsPerDimension ));
	unsigned int log2NumCellsPerDimension = HelperFunctions::log2ui(mNumCellsPerDimension);

	unsigned int* uintPtr = reinterpret_cast<unsigned int*>(mZIndexLookupTable->getCPUBufferHandle());

	for(unsigned int axis = 0; axis < 3; axis++ )
	{
		for(unsigned int currentIndexOnAxis = 0; currentIndexOnAxis < mNumCellsPerDimension; currentIndexOnAxis++ )
		{
			unsigned int posInArray = axis * mNumCellsPerDimension + currentIndexOnAxis;

			//init to zero
			uintPtr[ posInArray ] = 0;

			for(unsigned int currentBitPos = 0; currentBitPos < log2NumCellsPerDimension; currentBitPos++ )
			{
				uintPtr[ posInArray ] |=
					//select the bit in currentIndexOnAxis at bit position "currentBitPos":
					//shift it to the right currentBitPos bits, so that it is at the least significant bit;
					//to extract it from the rest of the bist being irrelevant, AND it with 1;
					//result: if 'currentIndexOnAxis[currentBitPos]'==0: 32bit uint value with all bits zero;
					//		  if 'currentIndexOnAxis[currentBitPos]'==1: 32bit uint value with LSB == 1
					//													 and rest of the bits zero;
					( (currentIndexOnAxis >> currentBitPos) & 1 )
					//'scatter' the bit for interleaved storage:
					//left shift by three times the actual bit pos yields in the end
					//the same bit pattern as currentIndexOnAxis, but with zwo zero bits between each old bit;
					<<
					(
						(3 * currentBitPos)
						+
						// Shifting this "scattered bit" in addition by 'axis' bits, the scttered bit pattern gets
						//an offset so that
						// 	   uintPtr[0* mNumCellsPerDimension + cell3Dindex.x]
						//	|  uintPtr[1* mNumCellsPerDimension + cell3Dindex.y]
						//	|  uintPtr[2* mNumCellsPerDimension + cell3Dindex.z]
						//yields the final z-index ;
						//see http://en.wikipedia.org/wiki/Z-order_curve for further info about Z-order curves
						axis
					)
					;
			}
		}
	}

	//write to memory
	mZIndexLookupTable->copyFromHostToGPU();


}

//create and add so visual sim domain a subobject consisting of
//a VertexBasedGeometry and a DebugDrawVisualMaterial; The geometry hold vertices for direct
//line drawing without index buffer;
void UniformGrid::createAndAddDebugDrawSubObject()
{
	//TODO
	assert(0&&"TODO implement");
}


}
