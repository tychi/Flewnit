/*
 * UniformGrid.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "AccelerationStructure.h"
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

	struct UniformGridBuffers
	{
		//four Buffers with mNumCellsPerDimension^3 elements of type uint
		Buffer* startIndices;
		Buffer* startIndicesCompacted;
		Buffer* elementCounts;
		Buffer* elementCountsCompatced;
	};

	UniformGrid(
		unsigned int numCellsPerDimension,
		Vector4D centerPosition,
		Vector4D extendsOfOneCell,
		//GPU-relevant "chunk-ization" size, denoting the max. element count processed
		//by on work group; default: 32
		unsigned int numMaxElementsPerSimulationWorkGroup,
		//particles, primitives.. what do you want to be mainainted by this acc. structure?
		std::vector<SceneRepresentationType> elementTypesToAllocBuffersFor
	);


	virtual ~UniformGrid();

	//throws exception if scene representation is not compatible
	//currently, only particle representation is supported
	virtual void update(SceneRepresentation* sceneRep) throw(SimulatorException);

	//throw exception if no buffers are allocated for the specified element type
	UniformGridBuffers& getUniformGridBuffers(SceneRepresentationType elementType)throw(BufferException);


private:

	unsigned int mNumCellsPerDimension; //usually 64

	Vector4D mCenterPosition;
	Vector4D mExtendsOfOneCell;

	unsigned int mNumMaxElementsPerSimulationWorkGroup;

	//one index and element count buffer per required element type (particle, primitive, voxel, node..)
	std::map<SceneRepresentationType, UniformGridBuffers> mUniformGridBuffers;

};

}

