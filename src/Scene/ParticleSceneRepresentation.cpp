/*
 * ParticleSceneRepresentation
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "ParticleSceneRepresentation.h"

#include "ParticleAttributeBuffers.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Simulator/SimulationResourceManager.h"
#include "Simulator/ParallelComputeManager.h"

#include "Buffer/Buffer.h"
#include "MPP/OpenCLProgram/CLProgram.h"
#include "Buffer/PingPongBuffer.h"
#include "MPP/OpenCLProgram/BasicCLProgram.h"
#include "Util/HelperFunctions.h"
#include "MPP/OpenCLProgram/CLKernelArguments.h"
#include "MPP/OpenCLProgram/ReorderParticleAttributesProgram.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"
#include "Material/ParticleFluidMechMat.h"
#include "WorldObject/ParticleFluid.h"
#include "Geometry/Geometry.h"
#include "Geometry/VertexBasedGeometry.h"


namespace Flewnit
{


ParticleSceneRepresentation::ParticleSceneRepresentation(
		unsigned int numTotalParticles,
		unsigned int numMaxFluids,
		unsigned int numMaxRigidBodies,
		//be careful with this value: the maximum may be hardware/implementation dependent and hence may be clamped;
		//GT200: max 256; Fermi: max 1024; reason: scarce local memory,
		//hence value reloading for greater particle counts;
		//not implemented a compromise allowing greater RB particle count for lesser performance yet (april 2011)
		unsigned int numMaxParticlesPerRigidBody,
		//should be 1/3* unigrid cell size --> 27 particles/cell-->good
		float voxelSideLengthRepresentedByRigidBodyParticle
	) throw(BufferException)
	:
	SceneRepresentation(PARTICLE_SCENE_REPRESENTATION),

	mNumTotalParticles(numTotalParticles),
	mNumMaxFluids(numMaxFluids),
	mNumCurrentFluids(0),
	mNumMaxRigidBodies(numMaxRigidBodies),
	mNumCurrentRigidBodies(0),
	mNumMaxParticlesPerRigidBody(numMaxParticlesPerRigidBody),
  	mVoxelSideLengthRepresentedByRigidBodyParticle(voxelSideLengthRepresentedByRigidBodyParticle),

	mObjectGenericFeaturesBuffer(0),
	mRigidBodyBuffer(0),
	mRigidBodyRelativePositionsBuffer(0),
	mParticleAttributeBuffers(0),
	mReorderParticleAttributesProgram(0)

{
	mObjectGenericFeaturesBuffer =
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneObjectGenericFeaturesBuffer"),
			//+1 because of the invalid object feature;
			(mNumMaxFluids + 1 + mNumMaxRigidBodies) * sizeof( CLShare::ObjectGenericFeatures ),
			true
		);

	//NOTE: even if mNumMaxRigidBodies == 0, we create a 128 byte buffer;
	//why? Because OpenCL kernels bug  if the don't get valid buffers passed to their
	//buffer arguments; NULL pointers don't seem to exist for kernel args ;(
	mRigidBodyBuffer =
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneRigidBodyBuffer"),
		    mNumMaxRigidBodies * sizeof( CLShare::ParticleRigidBody ),
			true
		);

	//TODO ensure that copies between a shared CL/GL Buffer and a pure CL buffer work bug free
	//note same stuff a above
	mRigidBodyRelativePositionsBuffer=
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneRigidBodyRelativePositionsBuffer"),
		    mNumMaxRigidBodies * mNumMaxParticlesPerRigidBody * sizeof( Vector4D ),
			true
		);

	mParticleAttributeBuffers = new ParticleAttributeBuffers(
		mNumTotalParticles, getInvalidObjectID(),
		//init to be save ;(
		true
	);

	mReorderParticleAttributesProgram = new ReorderParticleAttributesProgram(this);


}

ParticleSceneRepresentation::~ParticleSceneRepresentation()
{
	delete mParticleAttributeBuffers;
	//nothing els to delete, cl programs and buffers are globally managed
}


//precondition: mParticleAttributeBuffers->mZIndicesPiPoBuffer has been sorted and
//				mParticleAttributeBuffers->mOldIndicesPiPoBuffer has been reordered accordingly
void ParticleSceneRepresentation::reorderAttributes()
{
	//my CLKernelArguments implementation ensures the the ping pong buffers are correctly bound,
	//independent how often they were toggled;
	//As long as one sticks to the convention
	//to toggle after buffer writing, so that the least recently written one is always the active one
	//to be read, and the inactive one is the one to be written to,
	//everything should be alright.

	mReorderParticleAttributesProgram->getKernel("kernel_reorderParticleAttributes")->run(
		//wait for the event that the last radix sort reorder phase has finished; only after finishing,
		//mParticleAttributeBuffers->mOldIndicesPiPoBuffer has valid values usable for reordering
		EventVector{
			CLProgramManager::getInstance().
				getProgram("radixSort.cl")->
				getKernel("kernel_radixSort_reorder_Phase")->
				getEventOfLastKernelExecution()
		}
	);

	//toggle to make the least recently written buffers the active ones
	mParticleAttributeBuffers->toggleBuffers();

}

//upload mRigidBodyBuffer and mObjectGenericFeaturesBuffer to GPU before Simulation tick,
//as features may have been changed by the user;
void ParticleSceneRepresentation::flushObjectBuffers()
{
	mObjectGenericFeaturesBuffer->copyFromHostToGPU();
	mRigidBodyBuffer->copyFromHostToGPU();
}

//to be called after a simulation step:
// mRigidBodyBuffer is read back and features are applied to managed ParticleRigidBody's;
void ParticleSceneRepresentation::synchronizeRigidBodies()
{
	//TODO
	assert(0&&"TODO implement");

}


CLShare::ObjectGenericFeatures* ParticleSceneRepresentation::getObjectGenericFeatures(unsigned int i)
{
	assert(i < mNumMaxFluids+1+mNumMaxRigidBodies );

//i'm not that familiar with pointer arithmetics, hence the below version ;(
//	return reinterpret_cast<CLShare::ObjectGenericFeatures*>(
//			mObjectGenericFeaturesBuffer->getCPUBufferHandle())
//			+i;

	return
		& (
			reinterpret_cast<CLShare::ObjectGenericFeatures*>(
					mObjectGenericFeaturesBuffer->getCPUBufferHandle()
			)[i]
		);

}



//--------------------------------------------------------------------------------

//factory function; exception if not enough unassigned particles are left in the ParticleAttributeBuffers;
ParticleFluid* ParticleSceneRepresentation::createParticleFluid(
	String name,
	uint numParticles,
	const AABB& spawnRegion,
	const Vector4D& initialVelocity,
	//should be a ParticleLiquidVisualMaterial, but when simulating smoke, it would be a different one..
	//I'm tired of all those base class stubs; Maybe whis will be refactored once..
	VisualMaterial* visMat,
	ParticleFluidMechMat* mechMat
	) throw(BufferException)
{
	assert(mNumCurrentFluids< mNumMaxFluids);

	unsigned int particleStartIndexInAttributeBuffers=
		(mNumCurrentFluids == 0)
		? 0
		:   getObjectGenericFeatures(mNumCurrentFluids)->offsetInIndexTableBuffer
		  + getObjectGenericFeatures(mNumCurrentFluids)->particleCount;

	ID fluidObjectID = mNumCurrentFluids++;

	getObjectGenericFeatures(fluidObjectID)->massPerParticle =
			mechMat->mMassPerParticle;
	getObjectGenericFeatures(fluidObjectID)->restDensity =
				mechMat->mRestDensity;
	getObjectGenericFeatures(fluidObjectID)->gasConstant =
				mechMat->mGasConstant;
	getObjectGenericFeatures(fluidObjectID)->viscosity =
				mechMat->mViscosity;

	getObjectGenericFeatures(fluidObjectID)->offsetInIndexTableBuffer =
			particleStartIndexInAttributeBuffers;
	getObjectGenericFeatures(mNumCurrentFluids)->particleCount =
			numParticles;


	//init pos and vel buffers;

	Vector4D* positions =
		reinterpret_cast<Vector4D*>(mParticleAttributeBuffers->getPositionsPiPoBuffer()->getCPUBufferHandle());
	Vector4D* velocities =
		//initial velocities to PREDICTED buffer; See
		//_initial_updateForce_integrate_calcZIndex.cl for an explanation
		reinterpret_cast<Vector4D*>(mParticleAttributeBuffers->getPredictedVelocitiesPiPoBuffer()->getCPUBufferHandle());
	unsigned int* particleObjectInfos =
		reinterpret_cast<unsigned int*>(mParticleAttributeBuffers->getObjectInfoPiPoBuffer()->getCPUBufferHandle());

	//used as index buffer for drawing the point clouds:
	//(is permutated due to radix sort reordering, hence we cannot draw a fixed
	//particle stride without index buffer ;( )
	unsigned int* particleIndices =
		reinterpret_cast<unsigned int*>(mParticleAttributeBuffers->getParticleIndexTableBuffer()->getCPUBufferHandle());


	unsigned int numParticlesPerDimension;
	if( ( HelperFunctions::log2ui(numParticles) % 3 ) == 0 )
	{
		//third root of numParticles is an integer value; we can compute the third root like this:
		//2^(log2(numParticle)/3)
		numParticlesPerDimension = 1 <<
				( HelperFunctions::log2ui(numParticles) / 3 );
		//example: 2^15 particles --> log2(2^15)= 15 --> 15/3 = 5; 1<<5 = 2^5 = 32;
		//		   probe: (2^5)^3 = 2^18; correct ;)
	}
	else
	{
		//take float third root, truncate it and add one
		numParticlesPerDimension =
			(unsigned int)
			( std::pow((float)(numParticles),1.0f/3.0f) )
			+1;
		//example: 2^14 particles --> (2^14)^(1/3) = 25.39 --> trunc: 25 --> +1: 26
		//		  probe: 26^3 = 17576; this is bigger than 16384, i.e. we can work with this number, given
		//				we break the setup loop in time;
	}


	Vector4D stepSizes = (spawnRegion.getMax() - spawnRegion.getMin()) / numParticlesPerDimension;

	Vector4D currentParticlePos = spawnRegion.getMin();
	unsigned int currentParticleID =0;
	for(unsigned int z=0; z < (numParticlesPerDimension) && (currentParticleID < numParticles);  z++)
	{
		for( unsigned int y=0; y < (numParticlesPerDimension) && (currentParticleID < numParticles);  y++)
		{
			for(unsigned int x=0; x < (numParticlesPerDimension) && (currentParticleID < numParticles);  x++)
			{
				//non-spectecular index: it begins unpermutated in descending order;
				//it becomes only interesting during and after radix sort ;(
				particleIndices[currentParticleID] = currentParticleID;

				positions[currentParticleID] = currentParticlePos;
				velocities[currentParticleID] = initialVelocity;

				particleObjectInfos[currentParticleID] =0; //init
				SET_OBJECT_ID( particleObjectInfos[currentParticleID], fluidObjectID);
				SET_PARTICLE_ID( particleObjectInfos[currentParticleID],currentParticleID);

				currentParticleID++;

				currentParticlePos.x += stepSizes.x;
			}
			currentParticlePos.x = spawnRegion.getMin().x;
			currentParticlePos.y += stepSizes.y;
		}
		currentParticlePos.y = spawnRegion.getMin().y;
		currentParticlePos.z += stepSizes.z;
	}

	//dont copy buffers to GPU yet, there may be other fluid and/or rigid body objects following;



	VertexBasedGeometry* geo = createGeometryFromAttributeBuffers(
			name,
			particleStartIndexInAttributeBuffers,numParticles);

	mParticleFluids.push_back(
		new ParticleFluid(
			name,
			fluidObjectID,
			visMat,
			mechMat,
			geo
		)
	);


	return mParticleFluids[fluidObjectID];
}

//factory function; exception if not enough unassigned particles are left in the ParticleAttributeBuffers;
ParticleRigidBody* ParticleSceneRepresentation::createParticleRigidBody(
	String name,
	uint numParticles,
	const AmendedTransform& initialGlobalTransform,
	//visual stuff for default rendering:
	const std::vector<SubObject*>& visualSubobjects,
	//should be a debug daw material or whatsoever
	VisualMaterial* particleVisualizationMat,
	Buffer* relativePositionsBuffer,
	ParticleFluidMechMat* mechMat) throw(BufferException)
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}


//internal helper routine for creating the geometry objects for fluid objects, maybe the particle of rigid body
//objects (t.b.d.);
VertexBasedGeometry* ParticleSceneRepresentation::createGeometryFromAttributeBuffers(
		String name,
		unsigned int particleStartIndex, unsigned int particleCount)
{
	//doing graphics stuff here
	PARA_COMP_MANAGER->acquireSharedBuffersForGraphics();

	VertexBasedGeometry* pointGeo = new VertexBasedGeometry(
			name,
			VERTEX_BASED_POINT_CLOUD
	);

	pointGeo->setIndexBuffer(
			mParticleAttributeBuffers->getParticleIndexTableBuffer(),
			particleStartIndex,
			particleCount
			);

	pointGeo->setAttributeBuffer(
		mParticleAttributeBuffers->getPositionsPiPoBuffer()
	);

	pointGeo->setAttributeBuffer(
			//TODO decide if to use corrected or predicted velocitiese for drawing
			//(if we use it for drawing anyway ;( )
			mParticleAttributeBuffers->getPredictedVelocitiesPiPoBuffer()
	);

	pointGeo->setAttributeBuffer(
		mParticleAttributeBuffers->getDensitiesPiPoBuffer()
	);

	pointGeo->setAttributeBuffer(
		mParticleAttributeBuffers->getDensitiesPiPoBuffer()
	);

	pointGeo->setAttributeBuffer(
		mParticleAttributeBuffers->getLastStepsAccelerationsPiPoBuffer()
	);

	//return to comput stuff
	PARA_COMP_MANAGER->acquireSharedBuffersForCompute();

	return pointGeo;
}




//bind the Buffers of ParticleMechanicsStage::mParticleAttributeBuffers to SimulationPipelineStage::mRenderingResults;
//called by ParticleSceneRepresentation's constructor;
void ParticleSceneRepresentation::associateParticleAttributeBuffersWithRenderingResults(
		std::map<BufferSemantics,BufferInterface*>& renderingResults)
{

	renderingResults[
	  //INDEX_SEMANTICS
	  mParticleAttributeBuffers->mParticleIndexTableBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mParticleIndexTableBuffer;

	renderingResults[
	  //PRIMITIVE_ID_SEMANTICS
	  mParticleAttributeBuffers->mObjectInfoPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mObjectInfoPiPoBuffer;

	renderingResults[
	  //Z_INDEX_SEMANTICS
	  mParticleAttributeBuffers->mZIndicesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mZIndicesPiPoBuffer;

	renderingResults[
	  //CUSTOM_SEMANTICS
	  mParticleAttributeBuffers->mOldIndicesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mOldIndicesPiPoBuffer;

	renderingResults[
	  //POSITION_SEMANTICS
	  mParticleAttributeBuffers->mPositionsPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mPositionsPiPoBuffer;

	renderingResults[
	  //DENSITY_SEMANTICS
	  mParticleAttributeBuffers->mDensitiesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mDensitiesPiPoBuffer;

	renderingResults[
	  //VELOCITY_SEMANTICS
	        //optional use mPredictedVelocitiesPiPoBuffer ;(
	  mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer;

	renderingResults[
	  //FORCE_SEMANTICS
	  mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer;

}




}
