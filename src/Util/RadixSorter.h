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


namespace Flewnit
{

class RadixSorter
 : public BasicObject
{
public:
	RadixSorter(
		unsigned int numElements,
		unsigned int numBitsPerKey,
		unsigned int numRadicesPerPass,

		//implementation dependent stuff, default values indicate that optimal
		//(e.g. hardware dependent) values shall be chosen automatically
		unsigned int numElementsPerRadixCounter = 0,

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


	CLProgram* mClProgram_RadixSort;

	//--------------------------------------
	//{
	//members computed from user provided params and hardware dependent attributes
	//( e.g. local memory size):
		//for fermi: 			32768 (32kB out of 48kB available);
		//for GT200 and G80: 	 8192 ( 8kB out of 16kB available);
		unsigned int mLocalMemForRadixCounters;

		unsigned int mNumComputeUnits_Base2Ceiled;

		//( mLocalMemForRadixCounters / ( 4 Byte * mNumRadicesPerPass ) )
	    //for fermi architectures, this should be 2^15 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^7 = 128
	    //for GT200 architectures, this should be 2^13 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^5 =  32
		unsigned int mNumRadixCountersPerRadixAndWorkGroup;

		//( mNumElements  / ( mNumRadixCountersPerRadixAndWorkGroup * mNumElementsPerRadixCounter)
		unsigned int mNumWorkGroups_TabulationAndReorderPhase;
	//}

	//{ buffers for intermediate results
	//	check redixSort.cl for further info;

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

	//}
};

}

