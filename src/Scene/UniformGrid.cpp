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

#include "Geometry/VertexBasedGeometry.h"
#include "Material/DebugDrawVisualMaterial.h"
#include "WorldObject/SubObject.h"
#include "Material/VisualMaterial.h"
#include "MPP/OpenCLProgram/UpdateUniformGridProgram.h"

#include <boost/foreach.hpp>
#include "MPP/OpenCLProgram/CLKernelArguments.h"
#include "Buffer/PingPongBuffer.h"
#include "MPP/OpenCLProgram/SplitAndCompactUniformGridProgram.h"

#include "URE.h"
#include "Util/Time/FPSCounter.h"
#include "Util/Log/Log.h"


namespace Flewnit
{


UniformGridBufferSet::UniformGridBufferSet(String name, unsigned int numCellsPerDimension)
:
	//generic simulation domain, becaus a uniform grid has many purposes :P
	SimulationObject(name, GENERIC_SIM_DOMAIN),

	mNumCellsPerDimension(numCellsPerDimension),

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
	clearElementCounts();

//	//init CPU component of elem count buffer to zero;
//	memset( mElementCounts->getCPUBufferHandle(), 0, mElementCounts->getBufferInfo().bufferSizeInByte );
//	mElementCounts->copyFromHostToGPU();
//
//	//actually unneccessary to init startindices, but to be sure...
//	memset( mStartIndices->getCPUBufferHandle(), 0, mStartIndices->getBufferInfo().bufferSizeInByte );
//	//copy blocking initially
//	mStartIndices->copyFromHostToGPU(true);
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
	//son't use the BufferInterface abstraction here, because it does'nt allow event stuff;
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

	try
	{
		cl::Event default_kernelEvent = CLProgramManager::getInstance().getProgram("_initial_updateForce_integrate_calcZIndex.cl")->
				getKernel("kernel_initial_updateForce_integrate_calcZIndex")->getEventOfLastKernelExecution();

		//the kernels may not have been enqueueud yet; filter them out (zero indicates this);
		if(default_kernelEvent()  != 0)	{
			//we are in a pass > 1
			eventVec.push_back(default_kernelEvent);
		}
		else{
			cl::Event initial_kernelEvent =
				CLProgramManager::getInstance().
					getProgram("updateForce_integrate_calcZIndex.cl")->
					getKernel("kernel_updateForce_integrate_calcZIndex")->
					getEventOfLastKernelExecution();

			if( initial_kernelEvent() != 0 ){
				//we are in pass 1; dependency: initial physics copmutation kernel
				eventVec.push_back(initial_kernelEvent);
			}
			//else{
				//we are in pass 0; zero dependencies here
			//}
		}
	}
	catch(SimulatorException ex)
	{
		LOG<<INFO_LOG_LEVEL<<"UniformGridBufferSet::clearElementCounts(): no program existing yet to depend from ;)\n";
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


void UniformGridBufferSet::dumpBuffers(
		String dumpName,
		unsigned int frameNumber,
		bool abortAfterDump
)
{
	std::fstream fileStream;

	Path path =
		Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH )
		/ String("bufferDumps")
		/
		Path(
		String("bufferDump_")+ dumpName +
		String("_frame_")+
		HelperFunctions::toString(frameNumber)
		+String(".txt")
	);


	fileStream.open(
		path.string().c_str(),
		std::ios::out
	);

	mStartIndices->readBack(true);
	mElementCounts->readBack(true);


	uint* startIndices = reinterpret_cast<uint*>(mStartIndices->getCPUBufferHandle());
	uint* elemCounts = reinterpret_cast<uint*>(mElementCounts->getCPUBufferHandle());

	const uint totalNumGridCells = mNumCellsPerDimension*mNumCellsPerDimension*mNumCellsPerDimension;

	for(uint cellrunner = 0; cellrunner < totalNumGridCells; cellrunner++)
	{
		fileStream
			<< "cell ZIndex: "
				<<"bin.: "<<HelperFunctions::getBitString(cellrunner)<<", "
				<<"dec.: "<<cellrunner<<"; "
			<<"elem. startIndex: "<<startIndices[cellrunner]<<"; "
			<<"elem. cnt. resp. endIndex+1: "<<elemCounts[cellrunner]<<"; "
			<<";\n "
			;
	}



	fileStream.close();


	if(abortAfterDump)
	{
		assert(0&&"abort on purpose after programmer requested UniformGridBufferSet buffer dump :)");
		//URE_INSTANCE->requestMainLoopQuit();
	}

}



//##########################################################################################

UniformGrid::UniformGrid(
	String name,
	unsigned int numCellsPerDimension,

	Vector4D minCornerPosition,
	Vector4D extendsOfOneCell,
	//GPU-relevant "chunk-ization" size, denoting the max. element count processed
	//by on work group; default: 32
	unsigned int numMaxElementsPerSimulationWorkGroup,

	const std::vector<String>& namesOfUniGridBufferSetsToCreate
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

  mUpdateUniformGridProgram(0), //init to zero to be sure that all members are initialized first
  mNumMaxElementsPerSimulationWorkGroup(numMaxElementsPerSimulationWorkGroup),
  mSplitAndCompactUniformGridProgram(0) //init to zero to be sure that all members are initialized first

{
	BOOST_FOREACH(String name, namesOfUniGridBufferSetsToCreate)
	{
		assert( mUniformGridBufferSetMap.find(name) == mUniformGridBufferSetMap.end() );

		mUniformGridBufferSetMap[name] =
				new UniformGridBufferSet(name + String("UniformGridBufferSetBufferSet"), numCellsPerDimension);
	}


	setupZIndexLookUpTable();

	createAndAddDebugDrawSubObject();


	mUpdateUniformGridProgram = new UpdateUniformGridProgram(this);
	mSplitAndCompactUniformGridProgram = new SplitAndCompactUniformGridProgram(this);


}

UniformGrid::~UniformGrid()
{
	BOOST_FOREACH( UniformGridBufferSetMap::value_type & pair, mUniformGridBufferSetMap)
	{
		delete pair.second;
	}

	//nothing else to delete;

}


//throw exception if no buffers are allocated for the specified name
UniformGridBufferSet* UniformGrid::getBufferSet(String name)const throw(BufferException)
{
	if(mUniformGridBufferSetMap.find(name) == mUniformGridBufferSetMap.end())
	{
		throw(SimulatorException(
				String("UniformGrid::getBufferSet(String name): Buffer set with specified name ")
				+ name + String(" doesn't exist!") ));
	}

	return mUniformGridBufferSetMap.find(name)->second;
	//return mUniformGridBufferSetMap[name];
}


void UniformGrid::updateCells(String bufferSetName, BufferInterface* sortedZIndicesKeyBuffer)
{
	CLKernel* currentKernel = mUpdateUniformGridProgram->getKernel("kernel_updateUniformGrid");
	EventVector eventsToWaitFor =
		{	//wait for reorder phase of radix sort kernel to finish ;)
			CLProgramManager::getInstance().getProgram("radixSort.cl")
				->getKernel("kernel_radixSort_reorder_Phase")->getEventOfLastKernelExecution()
		};

	if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames() >0)
	{
		eventsToWaitFor.push_back(
			getBufferSet(bufferSetName)->getClearElementCountEvent()
		);
	}

	if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames()==0)
	{
		getBufferSet(bufferSetName)->dumpBuffers(
				String("UniGridBuffSet_")+bufferSetName+String("_BEFORE_Update"),
				URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
				false
		);
	}


	currentKernel->getCLKernelArguments()->getBufferArg("gSortedZIndices")
		->set(sortedZIndicesKeyBuffer);

	currentKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_ElementStartIndex")
		->set( getBufferSet(bufferSetName)->getStartIndices() );

	currentKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_ElementEndIndexPlus1")
		->set( getBufferSet(bufferSetName)->getElementCounts() );

	//currentKernel->run( eventsToWaitFor );
	currentKernel->run( eventsToWaitFor,
		CLKernelWorkLoadParams(
			sortedZIndicesKeyBuffer->getBufferInfo().numElements,
			HelperFunctions::floorToNextPowerOfTwo(PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize)
		)
	);

	if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames() == 0)
	{
		getBufferSet(bufferSetName)->dumpBuffers(
				String("UniGridBuffSet_")+bufferSetName+String("_AFTER_Update"),
				URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
				false
		);
	}


}

unsigned int UniformGrid::splitAndCompactCells(String bufferSetName,UniformGridBufferSet* compactionResultBufferSet)
{
	//scan phase

	CLKernel* scanKernel = mSplitAndCompactUniformGridProgram->getKernel("kernel_scan_localPar_globalSeq");

	scanKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_ElementStartIndex")
		->set( getBufferSet(bufferSetName)->getStartIndices() );

	scanKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_ElementEndIndexPlus1")
		->set( getBufferSet(bufferSetName)->getElementCounts() );

	//nothing else to set as argument for this kernel, intermediate buffers are already bound
	scanKernel->run(
		EventVector{
			//wait for the uniform grid update ;)
			mUpdateUniformGridProgram->getKernel("kernel_updateUniformGrid")
				->getEventOfLastKernelExecution()
		}
	);


	//-----------------------------------------------------------------------------------------------------------
	//finish scan anc split&compact phase

	CLKernel* splitNCompactKernel = mSplitAndCompactUniformGridProgram->getKernel("kernel_splitAndCompactUniformGrid");


	splitNCompactKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_ElementStartIndex")
		->set( getBufferSet(bufferSetName)->getStartIndices() );

	splitNCompactKernel->getCLKernelArguments()->getBufferArg("gUniGridCells_NumElements")
		->set( getBufferSet(bufferSetName)->getElementCounts() );

	splitNCompactKernel->getCLKernelArguments()->getBufferArg("gCompactedUniGridCells_ElementStartIndex")
		->set( compactionResultBufferSet->getStartIndices() );

	splitNCompactKernel->getCLKernelArguments()->getBufferArg("gCompactedUniGridCells_NumElements")
		->set( compactionResultBufferSet->getElementCounts() );

	//nothing else to set as argument for this kernel, intermediate buffers are already bound
	splitNCompactKernel->run(
		EventVector{
			//wait for the scan kernel to return (invocated above)
			scanKernel->getEventOfLastKernelExecution()
		}
	);


	//------------------------------------------------------------------------------------
	//read back total count:
	return mSplitAndCompactUniformGridProgram->readBackNumGeneratedNonEmptySplijtCells();

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
						// Shifting this "scattered bit" in addition by 'axis' bits, the scttered bit pattern gets
						//an offset so that
						// 	   uintPtr[0* mNumCellsPerDimension + cell3Dindex.x]
						//	|  uintPtr[1* mNumCellsPerDimension + cell3Dindex.y]
						//	|  uintPtr[2* mNumCellsPerDimension + cell3Dindex.z]
						//yields the final z-index ;
						//see http://en.wikipedia.org/wiki/Z-order_curve for further info about Z-order curves
						+ axis
					)
					;
			}
		}
	}

	//write to memory
	mZIndexLookupTable->copyFromHostToGPU(true);


	mZIndexLookupTable->readBack(true);
	for(unsigned int i = 0; i < 3* mNumCellsPerDimension; i++ )
	{
		LOG<< DEBUG_LOG_LEVEL <<"mZIndexLookupTable["<<i<<"]:"
				<<uintPtr[i]<<";\n";
	}


}

//create and add so visual sim domain a subobject consisting of
//a VertexBasedGeometry and a DebugDrawVisualMaterial; The geometry hold vertices for direct
//line drawing without index buffer;
void UniformGrid::createAndAddDebugDrawSubObject()
{
	VertexBasedGeometry* lineGeo = new VertexBasedGeometry(
		getName() + String("DebugDrawLineGeometry"),
		VERTEX_BASED_LINES
	);

	Buffer* linesPositionBuffer = new Buffer(
		BufferInfo(
			getName() + lineGeo->getName() + String("PositionBuffer"),
			ContextTypeFlags( HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG ),
			POSITION_SEMANTICS,
			TYPE_VEC4F,
			//6 faces @ (mNumCellsPerDimension +1) verts per axis, sqare to get num of verts filling a whole side
			6 * (mNumCellsPerDimension +1 ) * (mNumCellsPerDimension +1 ),
			BufferElementInfo( 4, GPU_DATA_TYPE_FLOAT,32,false ),
			VERTEX_ATTRIBUTE_BUFFER_TYPE,
			NO_CONTEXT_TYPE
		),
		false,
		0
	);

	Vector4D* posBuffer= reinterpret_cast<Vector4D*>(linesPositionBuffer->getCPUBufferHandle());

	Vector4D uniGridCentre = mMinCornerPosition + mNumCellsPerDimension * 0.5f * mExtendsOfOneCell;
	//Vector3D distFaceToCentre;  mNumCellsPerDimension * 0.5f ;

	unsigned int offsetInPosBuffer = 0;

	//handle the two xy-planes
	for( unsigned int yIndex = 0; yIndex <= mNumCellsPerDimension; yIndex ++)
	{
		for( unsigned int xIndex = 0; xIndex <= mNumCellsPerDimension; xIndex ++)
		{
			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * yIndex +  2 * xIndex + 0 ]
			  =
			     mMinCornerPosition +
			     xIndex * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
			     yIndex * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
			     0      * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
			     ;

			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * yIndex +  2 * xIndex + 1 ]
			  =
			     mMinCornerPosition +
			     xIndex * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
			     yIndex * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
			     mNumCellsPerDimension * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
			     ;
		}
	}

	offsetInPosBuffer += 2 * (mNumCellsPerDimension +1 ) * (mNumCellsPerDimension +1 );

	//handle the two zy-planes
	for( unsigned int yIndex = 0; yIndex <= mNumCellsPerDimension; yIndex ++)
	{
		for( unsigned int zIndex = 0; zIndex <= mNumCellsPerDimension; zIndex ++)
		{
			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * yIndex +  2 * zIndex + 0 ]
			  =
			     mMinCornerPosition +
			     0      * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
			     yIndex * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
			     zIndex * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
			     ;

			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * yIndex +  2 * zIndex + 1 ]
			  =
				mMinCornerPosition +
				mNumCellsPerDimension * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
			    yIndex 				  * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
				zIndex                * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
				;
		}
	}

	offsetInPosBuffer += 2 * (mNumCellsPerDimension +1 ) * (mNumCellsPerDimension +1 );

	//handle the two xz-planes
	for( unsigned int zIndex = 0; zIndex <= mNumCellsPerDimension; zIndex ++)
	{
		for( unsigned int xIndex = 0; xIndex <= mNumCellsPerDimension; xIndex ++)
		{
			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * zIndex +  2 * xIndex + 0 ]
			  =
			     mMinCornerPosition +
			     xIndex * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
			     0      * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
			     zIndex * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
			     ;

			posBuffer[ offsetInPosBuffer + 2 * (mNumCellsPerDimension +1) * zIndex +  2 * xIndex + 1 ]
			  =
				mMinCornerPosition +
				xIndex 				  * mExtendsOfOneCell.x * Vector4D(1.0f,0.0f,0.0f,0.0f) +
				mNumCellsPerDimension * mExtendsOfOneCell.y * Vector4D(0.0f,1.0f,0.0f,0.0f) +
				zIndex                * mExtendsOfOneCell.z * Vector4D(0.0f,0.0f,1.0f,0.0f)
				;
		}
	}

	linesPositionBuffer->copyFromHostToGPU();

	lineGeo->setAttributeBuffer(linesPositionBuffer);

	DebugDrawVisualMaterial* debugDrawMat = new DebugDrawVisualMaterial(
		getName() + lineGeo->getName() + String("VisualMat"),
		Vector4D(0.1f,0.0f,0.3f,1.0f),false,VERTEX_BASED_LINES
	);

//	VisualMaterial* debugDrawMat =
//		new VisualMaterial(
//			getName() + lineGeo->getName() + String("VisualMat"),
//			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
//			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
//			ShadingFeatures(
//				SHADING_FEATURE_DIRECT_LIGHTING
//			),
//			std::map<BufferSemantics,Texture*>(),
//			VisualMaterialFlags(true,false,true,true,false,false),
//			100.0f,
//			0.5f,
//			Vector4D(1.1f,0.0f,0.3f,1.0f)
//		);

	addSubObject(
		new SubObject(
			getName() + String("DebugDrawVisualSubObject"),
			VISUAL_SIM_DOMAIN,
			lineGeo,
			debugDrawMat
		)
	);
}


}
