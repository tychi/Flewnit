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
	Buffer* ping;
	Buffer* pong;

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

	//----- following the differnt index ping pong buffers

	BufferInfo glCLSharedZIndicesBufferInfo(
		"particleZIndicesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		Z_INDEX_SEMANTICS,
		TYPE_UINT32,
		//numTotalParticles *2,
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
		//numTotalParticles *2,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	glCLSharedOldIndicesBufferInfo.name = "particleOldIndicesBufferPong";
	pong = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	mOldIndicesPiPoBuffer = new PingPongBuffer("particleOldIndicesPiPoBuffer",ping,pong);



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
	ping = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	glCLSharedObjectInfoBufferInfo.name = "particleObjectInfoBufferPong";
	pong = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	mObjectInfoPiPoBuffer = new PingPongBuffer("particleObjectInfoPiPoBuffer",ping,pong);







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



void ParticleAttributeBuffers::toggleAllBuffersButZIndicesAndOldIndices()
{

	//the ping pong functionality of mOldIndicesPiPoBuffer is only needed
	//because of the several passes during radix sort;
	//After sorting, this buffer is only used for reordering, afterwards, its values are irrelevant
	//and are overwritten with the next radix sort pass;
	//mZIndicesPiPoBuffer->toggleBuffers();
	//mOldIndicesPiPoBuffer->toggleBuffers();

	mObjectInfoPiPoBuffer->toggleBuffers();

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

	mZIndicesPiPoBuffer->copyFromHostToGPU();
	mOldIndicesPiPoBuffer->copyFromHostToGPU();
	mObjectInfoPiPoBuffer->copyFromHostToGPU();

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

	//read back with forced barrier; this will be slow, but this is a degug routine!

	mParticleIndexTableBuffer->readBack(true);

	mZIndicesPiPoBuffer->readBack(true);
	mOldIndicesPiPoBuffer->readBack(true);
	mObjectInfoPiPoBuffer->readBack(true);

	mPositionsPiPoBuffer->readBack(true);
	mDensitiesPiPoBuffer->readBack(true);
	mCorrectedVelocitiesPiPoBuffer->readBack(true);
	mPredictedVelocitiesPiPoBuffer->readBack(true);
	mLastStepsAccelerationsPiPoBuffer->readBack(true);
}

void ParticleAttributeBuffers::dumpBuffers(
		String dumpName, unsigned int frameNumber,bool abortAfterDump, bool zIndicesOnly)
{
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


	readBackBuffers();



	uint* particleIndexTableBuffer =
		reinterpret_cast<uint*>(mParticleIndexTableBuffer->getCPUBufferHandle());

	uint* activeOldIndices =
		reinterpret_cast<uint*>(mOldIndicesPiPoBuffer->getCPUBufferHandle());
//irrelevant radix sort internal stuff, must be correct wehn radix sort is correct
//	uint* inactiveOldIndices =
//		reinterpret_cast<uint*>(mOldIndicesPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());

	uint* activeObjectInfos =
		reinterpret_cast<uint*>(mObjectInfoPiPoBuffer->getCPUBufferHandle());
//	uint* inactiveObjectInfos =
//		reinterpret_cast<uint*>(mObjectInfoPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());

	uint* activeZIndices =
		reinterpret_cast<uint*>(mZIndicesPiPoBuffer->getCPUBufferHandle());
//	uint* inactiveZIndices =
//		reinterpret_cast<uint*>(mZIndicesPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());



	Vector4D* activePositions =
		reinterpret_cast<Vector4D*>(mPositionsPiPoBuffer->getCPUBufferHandle());
	Vector4D* inactivePositions =
		reinterpret_cast<Vector4D*>(mPositionsPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());

	float* activeDensities =
		reinterpret_cast<float*>(mDensitiesPiPoBuffer->getCPUBufferHandle());
	float* inactiveDensities =
		reinterpret_cast<float*>(mDensitiesPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());


	Vector4D* activeCorrectedVelocities =
		reinterpret_cast<Vector4D*>(mCorrectedVelocitiesPiPoBuffer->getCPUBufferHandle());
	Vector4D* inactiveCorrectedVelocities =
		reinterpret_cast<Vector4D*>(mCorrectedVelocitiesPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());

	Vector4D* activePredictedVelocities =
		reinterpret_cast<Vector4D*>(mPredictedVelocitiesPiPoBuffer->getCPUBufferHandle());
	Vector4D* inactivePredictedVelocities =
		reinterpret_cast<Vector4D*>(mPredictedVelocitiesPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());

	Vector4D* activeAccelerations =
		reinterpret_cast<Vector4D*>(mLastStepsAccelerationsPiPoBuffer->getCPUBufferHandle());
	Vector4D* inactiveAccelerations =
		reinterpret_cast<Vector4D*>(mLastStepsAccelerationsPiPoBuffer->getInactiveBuffer()->getCPUBufferHandle());




	if(zIndicesOnly)
	{
		for(unsigned int i = 0 ; i< mNumTotalParticles; i++)
		{
			fileStream<<"buff index: "<<i<<"; "
				<<"Z-Index: (bin.)("
					<<HelperFunctions::getBitString(activeZIndices[i])<<"),"
				<<"(dec.):("
					<<activeZIndices[i]<<");"
				<<"part.ID in obj.: "<< GET_PARTICLE_ID(activeObjectInfos[i])<<", "
				<<"obj.ID of part.: "<< GET_OBJECT_ID(activeObjectInfos[i])
				<<";\n"
				;
		}
	}
	else
	{
		for(unsigned int i = 0 ; i< mNumTotalParticles; i++)
		{

			fileStream
				<<"current buffer index: "<<i<<":\n"
				<<"part.ID in obj.: "<< GET_PARTICLE_ID(activeObjectInfos[i])<<", "
				<<"obj.ID of part.: "<< GET_OBJECT_ID(activeObjectInfos[i])<<";\n"
				<<"particle being initially (at the begin of the simulation) at this index is now at index "
					<<particleIndexTableBuffer[i]<<";\n"

				<<"Before the last reordering, this particle was at buffer index "
					<<activeOldIndices[i]<<";\n"

				<<"Z-Index bin.("
						<<HelperFunctions::getBitString(activeZIndices[i])<<"), "
				<<"Z-Index dec.("
					<<activeZIndices[i]<<");\n"

				<<"pos("
						<<activePositions[i].x<<","
						<<activePositions[i].y<<","
						<<activePositions[i].z<<","
						<<activePositions[i].w<<"), "

				<<"dens("<<activeDensities[i]<<"),"

				<<"pred.vel("
					<<activePredictedVelocities[i].x<<","
					<<activePredictedVelocities[i].y<<","
					<<activePredictedVelocities[i].z<<","
					<<activePredictedVelocities[i].w<<"), "
				<<"corr.vel("
					<<activeCorrectedVelocities[i].x<<","
					<<activeCorrectedVelocities[i].y<<","
					<<activeCorrectedVelocities[i].z<<","
					<<activeCorrectedVelocities[i].w<<"), "

				<<"accel.("
					<<activeAccelerations[i].x<<","
					<<activeAccelerations[i].y<<","
					<<activeAccelerations[i].z<<","
					<<activeAccelerations[i].w<<"),"
				<<"\n\n";
		}

	}








	fileStream.close();

	if(abortAfterDump)
	{
		//shut down
		assert(0&&"abort on purpose after programmer requested buffer dump :)");
		//URE_INSTANCE->requestMainLoopQuit();
	}
}

}
