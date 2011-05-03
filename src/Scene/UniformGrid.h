/*
 * UniformGrid.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once


#include "AccelerationStructure.h"
#include "WorldObject/WorldObject.h"

#include "Simulator/SimulationObject.h"


//#include "SceneRepresentation.h"




namespace Flewnit
{

class UniformGridBufferSet
	 : public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	UniformGridBufferSet(String name, unsigned int numCellsPerDimension);
	virtual ~UniformGridBufferSet();

	//Before calling UniformGrid::updateCells(), the element counts mus be zero everywhere;
	//this is why we have to clear the buffer before;
	//The returned event is needed for the kernel_updateUniformGrid to wait for the transfer to finish;
	//after execution of kernel "kernel_updateForce_integrate_calcZIndex", the uniform grid buffers
	//aren't needed before the next "kernel_updateUniformGrid" invocation;
	//between the two calls, radix sort happens; we can hide the buffer write latency by
	//enqueue the write before invoking radix sort kernels;
	//The "kernel_updateUniformGrid" waiting for the mClearElementCountEvent makes security "perfect" ;).
	cl::Event clearElementCounts();
	inline cl::Event getClearElementCountEvent()const{return mClearElementCountEvent;}

	inline Buffer* getStartIndices()const{return  mStartIndices;}
	inline Buffer* getElementCounts()const{return mElementCounts;}

	void dumpBuffers(
			String dumpName,
			unsigned int frameNumber,
			bool abortAfterDump
	);

private:

	unsigned int mNumCellsPerDimension;

	//Buffers with numCellsPerDimension^3 elements of type uint
	Buffer* mStartIndices;
	Buffer* mElementCounts;

	cl::Event mClearElementCountEvent;

};



class UniformGrid
	 : public AccelerationStructure
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	UniformGrid(
		String name, //provide a name, as there might be several uniform grids in the whole simulation pipeline,
					 //and we need unique object names;
		unsigned int numCellsPerDimension,

		Vector4D minCornerPosition,
		Vector4D extendsOfOneCell,
		//GPU-relevant "chunk-ization" size, denoting the max. element count processed
		//by on work group; default: 32
		unsigned int numMaxElementsPerSimulationWorkGroup,
		//if you have several types to maintain (particles and triangle,e.g.), you have to
		//store them in different buffers in order to don't waste bandwidth if you only need a
		//single type at the moment; specify here implicitely how many UniformGridBufferSets
		//you need and by which name you want them to be referenced
		const std::vector<String>& namesOfUniGridBufferSetsToCreate
	);

	virtual ~UniformGrid();

	inline unsigned int getNumCellsPerDimension()const{return mNumCellsPerDimension; }
	inline const Vector4D& getMinCornerPosition()const{return mMinCornerPosition;}
	inline const Vector4D& getExtendsOfOneCell()const{return mExtendsOfOneCell;}

	inline unsigned int getNumMaxElementsPerSimulationWorkGroup()const{return mNumMaxElementsPerSimulationWorkGroup; }

	inline Buffer* getZIndexLookupTable()const{return mZIndexLookupTable;}

	//throw exception if no buffers are allocated for the specified name
	UniformGridBufferSet* getBufferSet(String name)const throw(BufferException);


	//updates mUniformGridBufferSet according to sortedZIndicesKeyBuffer;
	//there is an optimazation so that after returning,
	//this buffer mUniformGridBufferSet->mElementCounts contains (particleEndIndex+1);
	//this is "fixed" within splitAndCompactCells();
	//see updateUniformGrid.cl and splitAndCompactUniformGrid.cl for further info
	void updateCells(String bufferSetName, BufferInterface* sortedZIndicesKeyBuffer);
	//returns the number of non-empty split cells;
	//(this value implies to the ParticleMechanicsStage
	//how many work groups it must launch for SPH particle physics simulation Kernels)
	unsigned int splitAndCompactCells(String bufferSetName, UniformGridBufferSet* compactionResultBufferSet);



private:

	unsigned int mNumCellsPerDimension; //usually 64
	Vector4D mMinCornerPosition;
	Vector4D mExtendsOfOneCell;

	void setupZIndexLookUpTable(); //called by constructor
	Buffer* mZIndexLookupTable;

	//create and add so visual sim domain a subobject consisting of
	//a VertexBasedGeometry and a DebugDrawVisualMaterial; The geometry hold vertices for direct
	//line drawing without index buffer;
	void createAndAddDebugDrawSubObject();

	typedef std::map<String, UniformGridBufferSet*> UniformGridBufferSetMap;
	UniformGridBufferSetMap mUniformGridBufferSetMap;

	//{ update non-compacted stuff
	UpdateUniformGridProgram* mUpdateUniformGridProgram;

	//{ split and compact stuff
		unsigned int mNumMaxElementsPerSimulationWorkGroup;
		SplitAndCompactUniformGridProgram* mSplitAndCompactUniformGridProgram;


		//following buffers became obolete thanks to IntermediateResultBuffersManager:
		//private buffers for intermediate results:
		//mNumCellsPerDimension^3  elements
		//Buffer* gLocallyScannedTabulatedValues;
		//hardware dependent: (mNumCellsPerDimension^3)/ (2*numMaxWorkItems_Base2Floored) elements
		//Buffer* gPartiallyGloballyScannedTabulatedValues;
		//at least NUM_BASE2_CEILED_COMPUTE_UNITS + 1  elements;
		//+1 because the kernel finishing the "total scan" may wanna write out the total sum;
		//--> this is the value defining the number of work groups to be launched for SPH physics sim;
		//Buffer* gSumsOfPartialGlobalScans;
	//}

};

}


