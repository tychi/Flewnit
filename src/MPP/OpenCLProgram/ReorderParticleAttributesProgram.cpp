/*
 * ReorderParticleAttributesProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "ReorderParticleAttributesProgram.h"

#include "Simulator/ParallelComputeManager.h"
#include "Scene/ParticleSceneRepresentation.h"
#include "Util/HelperFunctions.h"
#include "CLKernelArguments.h"
#include "Buffer/Buffer.h"
#include "Scene/ParticleAttributeBuffers.h"
#include "Buffer/PingPongBuffer.h"

namespace Flewnit
{

ReorderParticleAttributesProgram::ReorderParticleAttributesProgram(ParticleSceneRepresentation* partScene)
 :
	BasicCLProgram(String("reorderParticleAttributes.cl"), MECHANICAL_SIM_DOMAIN),
	mParticleSceneRepresentation(partScene)
{
	//nothing to do
}

ReorderParticleAttributesProgram::~ReorderParticleAttributesProgram()
{
	//nothing to do
}


//issue the createKernel() call for reorderAttributes.cl with initial argument list
void ReorderParticleAttributesProgram::createKernels()
{
	mKernels["kernel_reorderParticleAttributes"] = new CLKernel(
		this,
		"kernel_reorderParticleAttributes",
		new CLKernelWorkLoadParams(
			mParticleSceneRepresentation->getNumTotalParticles(),
			HelperFunctions::floorToNextPowerOfTwo(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
			)
		),
		new CLKernelArguments(
		{
			new CLBufferKernelArgument("cObjectGenericFeatures",
				mParticleSceneRepresentation->mObjectGenericFeaturesBuffer),
//HAXX TEST
//mParticleSceneRepresentation->mParticleAttributeBuffers->mOldIndicesPiPoBuffer->getActiveBuffer()),

			//least recently written i.e. active buffer is automatically selected if not specified otherwise,
			//hence just pass the ping pong buffer as-is;
			//we are not writing to this buffer in this kernel
			new CLBufferKernelArgument("gReorderedOldIndices",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mOldIndicesPiPoBuffer),
//HAXX TEST
//mParticleSceneRepresentation->mParticleAttributeBuffers->mOldIndicesPiPoBuffer->getActiveBuffer()),

			//not a ping pong buffer, hence just pass without any special case considferation
			new CLBufferKernelArgument("gParticleIndexTable",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mParticleIndexTableBuffer),

			//-------- followong ping pongs

			new CLBufferKernelArgument("gParticleObjectInfosOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mObjectInfoPiPoBuffer),
			new CLBufferKernelArgument("gParticleObjectInfosReordered",
				//set ifPingPongBufferUseInactiveOne=true, so that the out-dated buffer is bound for writing
				mParticleSceneRepresentation->mParticleAttributeBuffers->mObjectInfoPiPoBuffer, true ),

			new CLBufferKernelArgument("gPositionsOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mPositionsPiPoBuffer),
			new CLBufferKernelArgument("gPositionsReordered",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mPositionsPiPoBuffer, true ),

			new CLBufferKernelArgument("gDensitiesOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mDensitiesPiPoBuffer),
			new CLBufferKernelArgument("gDensitiesReordered",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mDensitiesPiPoBuffer, true ),

			new CLBufferKernelArgument("gCorrectedVelocitiesOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer),
			new CLBufferKernelArgument("gCorrectedVelocitiesReordered",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer, true ),

			new CLBufferKernelArgument("gPredictedVelocitiesOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mPredictedVelocitiesPiPoBuffer),
			new CLBufferKernelArgument("gPredictedVelocitiesReordered",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mPredictedVelocitiesPiPoBuffer, true ),

			new CLBufferKernelArgument("gAccelerationsOld",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer),
			new CLBufferKernelArgument("gAccelerationsReordered",
				mParticleSceneRepresentation->mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer, true )
			}
		)
	);
}


}
