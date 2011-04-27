/*
 * RadixSorter.h
 *
 *  Created on: Apr 22, 2011
 *      Author: tychi
 *
 *  A (hopefully) highly optimized OpenCL implementation of the
 *  "Parallel Radix Sort using Work Efficient Parallel Prefix Sum";
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorForwards.h"

namespace Flewnit
{

class RadixSorter
 : public BasicObject
{
	 FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	RadixSorter(
		unsigned int numElements,
		unsigned int numBitsPerKey,
		unsigned int numRadicesPerPass,

		//implementation dependent stuff, default values indicate that optimal
		//(e.g. hardware dependent) values shall be chosen automatically
		unsigned int numElementsPerRadixCounter = 0
	);

	virtual ~RadixSorter();

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
	void sort(PingPongBuffer* keysBuffer, PingPongBuffer* oldIndicesBuffer);

private:

	unsigned int mNumElements;
	unsigned int mNumBitsPerKey;
	unsigned int mNumRadicesPerPass;

	unsigned int mNumElementsPerRadixCounter;


	//--------------------------------------
	//{
	//members computed from user provided params and hardware dependent attributes
	//( e.g. local memory size):
		//for fermi: 			32768 (32kB out of 48kB available);
		//for GT200 and G80: 	 8192 ( 8kB out of 16kB available);
		unsigned int mLocalMemForRadixCounters;

		//pow2(ceil(log2(CL_DEVICE_MAX_COMPUTE_UNITS)));
		//e.g.: 15 compute units --> log2(15)= 3.9 -->ceil(3.9)=4 --> pow2(4)= 16
		//value needed to do a good work balancing on the openCL device:
		//use all compute units, but do as much as possible per compute unit to reduce
		//management overhead; As we completely work on base2-sizes, we need to
		//ceil ne number of available compute units to the next power of two;
		//Otherwise a device like the Geforce GTX570 with its 15 compute units
		//would use only 8 of them;
		//TODO check:
		//	if there are slightly less than a power of two number of compute units,
		//	(of all things applying to the "high end" devices GTX570 (15 SMP's) and GTX280 (30 SMP's))
		//	there will be very few work groups left in the end, while the rest of the compute units is
		//  in idle; maybe a base2-floored amount of work items, doiung the doubled work,
		//  would perform even better in this case; Is subject to experimentation;
		unsigned int mNumComputeUnits_Base2Ceiled;

		//( mLocalMemForRadixCounters / ( 4 Byte * mNumRadicesPerPass ) )
	    //for fermi architectures, this should be 2^15 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^7 = 128
	    //for GT200 architectures, this should be 2^13 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^5 =  32
		unsigned int mNumRadixCountersPerRadixAndWorkGroup;

		//( mNumElements  / ( mNumRadixCountersPerRadixAndWorkGroup * mNumElementsPerRadixCounter)
		unsigned int mNumWorkGroups_TabulationAndReorderPhase;
	//}

	RadixSortProgram* mRadixSortProgram;

	//{ buffers for intermediate results
	//	check radixSort.cl for further info;

		//became obsolete thanks to IntermediateResultBuffersManager:
		/*

		//mNumRadicesPerPass * ( mNumElements / mNumElementsPerRadixCounter) elements of type uint; (e.g. 64 * 64k);
		Buffer* mLocalRadixCounters;

		//mNumRadicesPerPass * mNumWorkGroups_TabulationAndReorderPhase (e.g. 64*512);
		Buffer* mSumsOfLocalRadixCounters;

		//mNumRadicesPerPass elements (e.g. 64);
		//for each radix,holds the number of keys containing this radix in their currently treated
		//bit-stride at the end of phase 2;
		Buffer* mTotalRadixCounts;


		//mNumComputeUnits_Base2Ceiled elements used; but alloc at least 128 elements
		//to be save that no strange OpenCL-Implementation-caused side effects
		//occur (alignment, cache line size etc..)
		Buffer* mPartialScanResultsOfTotalRadixCounts;

		*/
	//}
};

}

