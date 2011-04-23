/*
 * UniformGrid.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once


#include "AccelerationStructure.h"
#include "WorldObject/WorldObject.h"

#include "SceneRepresentation.h"


#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE


namespace Flewnit
{



class UniformGrid
	 : public AccelerationStructure
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

//	class UniformGridBuffers
//	{
//
//		//four Buffers with mNumCellsPerDimension^3 elements of type uint
//		Buffer* startIndices;
//		Buffer* startIndicesCompacted;
//		Buffer* elementCounts;
//		Buffer* elementCountsCompatced;
//
//		unsigned int numNonEmptyCellsAfterSplitAndCompaction;
//	};

	UniformGrid(
		unsigned int numMaxContainingElements, //param needed for radix sort;
		unsigned int numCellsPerDimension,

		Vector4D centerPosition,
		Vector4D extendsOfOneCell,
		//GPU-relevant "chunk-ization" size, denoting the max. element count processed
		//by on work group; default: 32
		unsigned int numMaxElementsPerSimulationWorkGroup
	);


	virtual ~UniformGrid();

	//just passes sort request to mRadixSorter;
	void sort(PingPongBuffer* zIndicesKeyBuffer, PingPongBuffer* oldIndicesBuffer);

	void updateCells(PingPongBuffer* sortedZIndicesKeyBuffer, PingPongBuffer* reorderedOldIndicesBuffer);

	void splitAndCompactCells( Buffer* startIndicesCompacted, Buffer* elementCountsCompatced);

	//throw exception if no buffers are allocated for the specified element type
	inline Buffer* getStartIndices()const{return mStartIndices;}
	inline PingPongBuffer* getlementCounts()const{return mElementCounts;}


private:

	unsigned int mNumCellsPerDimension; //usually 64
	Vector4D mCenterPosition;
	Vector4D mExtendsOfOneCell;


	unsigned int mNumMaxContainingElements;
	RadixSorter* mRadixSorter;

	//Buffers with mNumCellsPerDimension^3 elements of type uint
	Buffer* mStartIndices;
	//must be ping pong, because one buffer needs to be cleared while the other is compacted,
	//because ther is only one non-generic kernel where work items are directly associated with
	//uniform grid cells; To ensure a valid state before only partial updating is done in the following
	//frame, we need one buffer for neighbour lookup and one to be cleared for next frame;
	//don't forget toggle;
	PingPongBuffer* mElementCounts;
	CLProgram* mCLProgram_UpdateUniformGrid;

	//{ scan and split stuff
		//private buffers for intermediate results:

		//mNumCellsPerDimension^3  elements
		Buffer* gLocallyScannedTabulatedValues;
		//hardware dependent: (mNumCellsPerDimension^3)/ (2*numMaxWorkItems_Base2Floored) elements
		Buffer* gPartiallyGloballyScannedTabulatedValues;
		//at least NUM_BASE2_CEILED_COMPUTE_UNITS + 1  elements;
		//+1 because the kernel finishing the "total scan" may wanna write out the total sum;
		//--> this is the value defining the number of work groups to be launched for SPH physics sim;
		Buffer* gSumsOfPartialGlobalScans;

		unsigned int mNumMaxElementsPerSimulationWorkGroup;

		CLProgram* mCLProgram_splitAndCompactUniformGrid;
	//}

};

}

