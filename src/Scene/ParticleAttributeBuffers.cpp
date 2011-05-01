/*
 * ParticleAttributeBuffers.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "ParticleAttributeBuffers.h"

#include "Buffer/BufferSharedDefinitions.h"
#include "Buffer/Buffer.h"
#include "Buffer/PingPongBuffer.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Util/HelperFunctions.h"
#include "URE.h"


#include "Util/Log/Log.h"


namespace Flewnit
{

ParticleAttributeBuffers::ParticleAttributeBuffers(
		unsigned int numTotalParticles,
		unsigned int invalidObjectID,
		bool initToInvalidObjectID)
: mNumTotalParticles(numTotalParticles)
{
	BufferInfo glCLSharedIndexBufferInfo(
		"particleIndexTableBuffer",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		INDEX_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_INDEX_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	mParticleIndexTableBuffer = new Buffer(	glCLSharedIndexBufferInfo,	true, 0 );


	BufferInfo glCLSharedObjectInfoBufferInfo(
		"particleObjectInfoBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		PRIMITIVE_ID_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	Buffer* ping = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	glCLSharedObjectInfoBufferInfo.name = "particleObjectInfoBufferPong";
	Buffer* pong = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	mObjectInfoPiPoBuffer = new PingPongBuffer("particleObjectInfoPiPoBuffer",ping,pong);


	BufferInfo glCLSharedZIndicesBufferInfo(
		"particleZIndicesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		Z_INDEX_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedZIndicesBufferInfo,	true, 0 );
	glCLSharedZIndicesBufferInfo.name = "particleZIndicesBufferPong";
	pong = new Buffer(	glCLSharedZIndicesBufferInfo,	true, 0 );
	mZIndicesPiPoBuffer = new PingPongBuffer("particleZIndicesPiPoBuffer",ping,pong);

	BufferInfo glCLSharedOldIndicesBufferInfo(
		"particleOldIndicesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		CUSTOM_SEMANTICS, //actually, it is not planned to bind this buffer as vertex  attribute to GL,
						  //hence it has no real semantics, it is just a helper buffer for sorting;
						  //but maybe for debug purposes, one would want to bind it anyway ;(...
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	glCLSharedOldIndicesBufferInfo.name = "particleOldIndicesBufferPong";
	pong = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	mOldIndicesPiPoBuffer = new PingPongBuffer("particleOldIndicesPiPoBuffer",ping,pong);


	//-------------------------------------------------------------------------------------------------------------------
	//beginning with the actual physical attributes:

	BufferInfo glCLSharedPositionsBufferInfo(
		"particlePositionsBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		POSITION_SEMANTICS,
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedPositionsBufferInfo,	true, 0 );
	glCLSharedPositionsBufferInfo.name = "particlePositionsBufferPong";
	pong = new Buffer(	glCLSharedPositionsBufferInfo,	true, 0 );
	mPositionsPiPoBuffer = new PingPongBuffer("particlePositionsPiPoBuffer",ping,pong);

	BufferInfo glCLSharedDensitiesBufferInfo(
		"particleDensitiesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		DENSITY_SEMANTICS,
		TYPE_FLOAT,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedDensitiesBufferInfo,	true, 0 );
	glCLSharedDensitiesBufferInfo.name = "particleDensitiesBufferPong";
	pong = new Buffer(	glCLSharedDensitiesBufferInfo,	true, 0 );
	mDensitiesPiPoBuffer = new PingPongBuffer("particleDensitiesPiPoBuffer",ping,pong);


	BufferInfo glCLSharedVelocitiesBufferInfo(
		"particleCorrectedVelocitiesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		VELOCITY_SEMANTICS,
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	glCLSharedVelocitiesBufferInfo.name = "particleCorrectedVelocitiesBufferPong";
	pong = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	mCorrectedVelocitiesPiPoBuffer = new PingPongBuffer("particleCorrectedVelocitiesPiPoBuffer",ping,pong);

	//at least one time, a buffer infor object can be reused ;)
	glCLSharedVelocitiesBufferInfo.name = "particlePredictedVelocitiesBufferPing";
	ping = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	glCLSharedVelocitiesBufferInfo.name = "particlePredictedVelocitiesBufferPong";
	pong = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	mPredictedVelocitiesPiPoBuffer = new PingPongBuffer("particlePredictedVelocitiesPiPoBuffer",ping,pong);



	BufferInfo glCLSharedAccelerationsBufferInfo(
		"particleAccelerationsBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		FORCE_SEMANTICS, //k, have to refactor ;(.. but F=m*a, particle mass is known ;)
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedAccelerationsBufferInfo,	true, 0 );
	glCLSharedAccelerationsBufferInfo.name = "particleAccelerationsBufferPong";
	pong = new Buffer(	glCLSharedAccelerationsBufferInfo,	true, 0 );
	mLastStepsAccelerationsPiPoBuffer = new PingPongBuffer("particleAccelerationsPiPoBuffer",ping,pong);

	//-------------------------------------------------------------------------------------------------------
	if(initToInvalidObjectID)
	{
		unsigned int* objectInfos = reinterpret_cast<unsigned int*>( mObjectInfoPiPoBuffer->getCPUBufferHandle() );

		for(unsigned int i = 0; i< numTotalParticles; i++)
		{
			objectInfos[i] =0; //init

			SET_OBJECT_ID(objectInfos[i], invalidObjectID);
			SET_PARTICLE_ID(objectInfos[i], i);
		}

		//no GPU upload, this is done when all RB and fluids are initialized;
	}

}

ParticleAttributeBuffers::~ParticleAttributeBuffers()
{
	//nothing to do, buffers are deleted by SimResourceMan.
}



void ParticleAttributeBuffers::toggleBuffers()
{
	mObjectInfoPiPoBuffer->toggleBuffers();
	mZIndicesPiPoBuffer->toggleBuffers();

	//the ping pong functionality of mOldIndicesPiPoBuffer is only needed
	//because of the several passes during radix sort;
	//After sorting, this buffer is only used for reordering, afterwards, its values are irrelevant
	//and are overwritten with the next radix sort pass;
	//hence, we can toggle this buffer together with the others for logical consistency, and no one gives a sh** ;);
	mOldIndicesPiPoBuffer->toggleBuffers();

	mPositionsPiPoBuffer->toggleBuffers();
	mDensitiesPiPoBuffer->toggleBuffers();
	mCorrectedVelocitiesPiPoBuffer->toggleBuffers();
	mPredictedVelocitiesPiPoBuffer->toggleBuffers();
	mLastStepsAccelerationsPiPoBuffer->toggleBuffers();
}


//after all fluids and rigid bodies have been initialized, they have to be uploaded to the
//cl device;
void ParticleAttributeBuffers::flushBuffers()
{
	//what an amazing piece of code this routine is ;)

	mParticleIndexTableBuffer->copyFromHostToGPU();

	mObjectInfoPiPoBuffer->copyFromHostToGPU();
	mZIndicesPiPoBuffer->copyFromHostToGPU();
	mOldIndicesPiPoBuffer->copyFromHostToGPU();
	mPositionsPiPoBuffer->copyFromHostToGPU();
	mDensitiesPiPoBuffer->copyFromHostToGPU();
	mCorrectedVelocitiesPiPoBuffer->copyFromHostToGPU();
	mPredictedVelocitiesPiPoBuffer->copyFromHostToGPU();
	mLastStepsAccelerationsPiPoBuffer->copyFromHostToGPU();

}

//usually, all particle attribute data remains on the GPU and don't need to be read back;
//but for debugging purposes during development, we will need the read-back functionality;
void ParticleAttributeBuffers::readBackBuffers()
{
	//what an amazing piece of code this routine is ;)

	mParticleIndexTableBuffer->readBack();

	mObjectInfoPiPoBuffer->readBack();
	mZIndicesPiPoBuffer->readBack();
	mOldIndicesPiPoBuffer->readBack();
	mPositionsPiPoBuffer->readBack();
	mDensitiesPiPoBuffer->readBack();
	mCorrectedVelocitiesPiPoBuffer->readBack();
	mPredictedVelocitiesPiPoBuffer->readBack();
	mLastStepsAccelerationsPiPoBuffer->readBack();
}

void ParticleAttributeBuffers::dumpBuffers(String dumpName, unsigned int frameNumber)
{

	readBackBuffers();

	std::fstream fileStream;
	Path path =
		Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH )
		/ String("bufferDumps")
		/
		Path(
			String("bufferDump_")+ dumpName + String("_")+
			HelperFunctions::toString(frameNumber)+String(".txt")
		);


	fileStream.open(
		path.string().c_str(),
		std::ios::out
	);

	Vector4D* positions =
		reinterpret_cast<Vector4D*>(getPositionsPiPoBuffer()->getCPUBufferHandle());

	Vector4D* predVels =
		reinterpret_cast<Vector4D*>(getPredictedVelocitiesPiPoBuffer()->getCPUBufferHandle());

	unsigned int* particleObjectInfos =
		reinterpret_cast<unsigned int*>(getObjectInfoPiPoBuffer()->getCPUBufferHandle());

	//used as index buffer for drawing the point clouds:
	//(is permutated due to radix sort reordering, hence we cannot draw a fixed
	//particle stride without index buffer ;( )
	unsigned int* particleIndices =
		reinterpret_cast<unsigned int*>(getParticleIndexTableBuffer()->getCPUBufferHandle());

	unsigned int* zIndices =
		reinterpret_cast<unsigned int*>(getZIndicesPiPoBuffer()->getCPUBufferHandle());




	for(unsigned int i = 0 ; i< mNumTotalParticles; i++)
	{
		String zIndexString="";
		for(unsigned int bitRunner = 0 ; bitRunner < 32 ; bitRunner++)
		{
			zIndexString.append(
				( (zIndices[i] & (1<< (31-bitRunner) ) ) == 0 )
				?"0":"1"
			);
		}


		fileStream
			<<"Particle number "<<i<<": "

			<<"active Position("
					<<positions[i].x<<","
					<<positions[i].y<<","
					<<positions[i].z<<","
					<<positions[i].w<<"), "

			<<"active predicted velocity("
				<<predVels[i].x<<","
				<<predVels[i].y<<","
				<<predVels[i].z<<","
				<<predVels[i].w<<"), "

			<<"ZIndex: "<<zIndexString

			<<"\n";
	}


	fileStream.close();

	//shut down
	assert(0&&"abort on purpose after programmer requested buffer dump :)");
	//URE_INSTANCE->requestMainLoopQuit();
}

}
