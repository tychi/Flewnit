/*
 * RadixSorter.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: tychi
 */

#include "RadixSorter.h"

#include "Util/HelperFunctions.h"
#include "Simulator/ParallelComputeManager.h"
#include "MPP/OpenCLProgram/RadixSortProgram.h"


//don't know if hardcode is necessary, but numbers deep in source code are even worse ;(
//TODO outsource to XML
#define FLEWNIT_NUM_RADIX_SORT_PASSES 3

#define FLEWNIT_FERMI_NUM_KEY_ELEMENTS_PER_RADIX_COUNTER 4
#define FLEWNIT_NON_FERMI_NUM_KEY_ELEMENTS_PER_RADIX_COUNTER 8

namespace Flewnit
{

RadixSorter::RadixSorter(
		unsigned int numElements,
		unsigned int numBitsPerKey //should be log2ui(numPaxParticles)
									//= 15 for 2^15== 32786
									//= 18 for 2^18==262144
		//implementation dependent stuff, default zero values indicate that optimal
		//(e.g. hardware dependent) values shall be chosen automatically
		//unsigned int numRadicesPerPass,
		//unsigned int numElementsPerRadixCounter
)
:
	mNumElements(numElements),
	mNumBitsPerKey(numBitsPerKey),

	mNumRadixSortPasses(FLEWNIT_NUM_RADIX_SORT_PASSES),
	mNumRadicesPerPass(
		1 <<  //pow2 of..
		(
			//log2 of numRadicesPerPass
			//kinda dumb "metric", but time pressure...
			//numBitsPerKey==14 --> 15 --> 5 ; one radix too much in the last pass, but let's not make is complicated..
			//numBitsPerKey==15 --> 15 --> 5
			//numBitsPerKey==18 --> 18 --> 6
			HelperFunctions::ceilToNextMultiple(numBitsPerKey,FLEWNIT_NUM_RADIX_SORT_PASSES)
			/ FLEWNIT_NUM_RADIX_SORT_PASSES
		)
	),
	mNumElementsPerRadixCounter(
		//nvidia fermit devices have compute capability 2.x
		PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.x >= 2
		? FLEWNIT_FERMI_NUM_KEY_ELEMENTS_PER_RADIX_COUNTER
		: FLEWNIT_NON_FERMI_NUM_KEY_ELEMENTS_PER_RADIX_COUNTER
	),

	//for fermi: 			32768 (32kB out of 48kB available);
	//for GT200 and G80: 	 8192 ( 8kB out of 16kB available);
	mLocalMemForRadixCounters(
		//We cannot use the whole local memory of the OpenCL device;
		//We want a power of two value;
		//--> the trick: sub 1 from localMemorySize, florr it to next lower base2
		//GT200 and G80: floorPow2(16kb-1)=  8 kb
		//fermi		   : floorPow2(48kb-1)= 32 kb
		HelperFunctions::floorToNextPowerOfTwo(
			static_cast<unsigned int>
				( PARA_COMP_MANAGER->getParallelComputeDeviceInfo().localMemorySizeByte -1 )
		)
	),
	//Geforxe GT  435M :  2 -->  2
	//Geforce GTX 280  : 30 --> 32 <-- bottleneck expected, last work group will be alone :(
	//Geforce GTX 570  : 15 --> 16 <-- bottleneck expected, last work group will be alone :(
	//Geforce GTX 580  : 16 --> 16
	mNumComputeUnits_Base2Ceiled(
		HelperFunctions::ceilToNextPowerOfTwo(
			( PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxComputeUnits )
		)
	),

	//( mLocalMemForRadixCounters / ( 4 Byte * mNumRadicesPerPass ) )
    //for fermi architectures, this should be 2^15 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^7 = 128
    //for GT200 architectures, this should be 2^13 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^5 =  32
	mNumRadixCountersPerRadixAndWorkGroup(
		mLocalMemForRadixCounters / (4 * mNumRadicesPerPass)
	),

	mNumWorkGroups_TabulationAndReorderPhase(
	  mNumElements  / ( mNumRadixCountersPerRadixAndWorkGroup * mNumElementsPerRadixCounter)
	),


	mRadixSortProgram(0)
{
	assert( HelperFunctions::isPowerOfTwo(mNumElements) );
	assert( (mNumBitsPerKey <= 32 ) && (mNumBitsPerKey >= 1) );


	//create after assertions so that the RadixSortProgram class does not have to catch those cases;
	mRadixSortProgram = new RadixSortProgram(this);

}

RadixSorter::~RadixSorter()
{
	//nothing to delete
}


/*
 * input: - keysbuffer: contains numElements uint elements, where the most significant
 * 						(32-numBitsPerKey) bits are zero, to be sorted in ascending order;
 * 		  - oldIndicesBuffer: contains numElements uint elements, their values are irrelevant;
 * output:- keysBuffer: the active of the ping pong buffer contains the sorted keys, the inactive
 * 						contains irrelevant values;
 * 		  - oldIndicesBuffer: the active of the ping pong buffer contains the old indices of the
 * 						now-sorted key array, so that buffers associated to the key array can be
 * 						reordered according to this index buffer:
 * 							reorderedValues[currentIndex] = unorderedValues[oldIndicesBuffer[currentIndex]];
 * 						The inactive buffer contains irrelevant values;
 *
 * */
void RadixSorter::sort(PingPongBuffer* keysBuffer, PingPongBuffer* oldIndicesBuffer)
{
	//TODO
	assert(0&&"TODO implement");
}



}
