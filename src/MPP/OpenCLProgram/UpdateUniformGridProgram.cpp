/*
 * UpdateUniformGridProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "UpdateUniformGridProgram.h"


#include "Scene/UniformGrid.h"




namespace Flewnit
{

UpdateUniformGridProgram::UpdateUniformGridProgram(UniformGrid* uniGrid)
:
	UniformGridRelatedProgram(String("updateUniformGrid.cl"), uniGrid)
{
	//nothing to do
}

UpdateUniformGridProgram::~UpdateUniformGridProgram()
{
	//nothing to do
}


void UpdateUniformGridProgram::createKernels()
{
//	mKernels["kernel_updateUniformGrid"] = new CLKernel(
//		this,
//		"kernel_updateUniformGrid",
//
//		new CLKernelWorkLoadParams(
//			mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension(),
//			HelperFunctions::floorToNextPowerOfTwo(
//				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
//			)
//		),
//
//		new CLKernelArguments(
//		{
//			new CLBufferKernelArgument("gSortedZIndices",
//				mParticleSceneRepresentation->mObjectGenericFeaturesBuffer),
//
//
//			//least recently written i.e. active buffer is automatically selected if not specified otherwise,
//			//hence just pass the ping pong buffer as-is;
//			//we are not writing to this buffer in this kernel
//			new CLBufferKernelArgument("gReorderedOldIndices",
//				mParticleSceneRepresentation->mParticleAttributeBuffers->mOldIndicesPiPoBuffer),
//
//			//not a ping pong buffer, hence just pass without any special case considferation
//			new CLBufferKernelArgument("gParticleIndexTable",
//				mParticleSceneRepresentation->mParticleAttributeBuffers->mParticleIndexTableBuffer),
//
//			//-------- followong ping pongs
//
//			new CLBufferKernelArgument("gParticleObjectInfosOld",
//				mParticleSceneRepresentation->mParticleAttributeBuffers->mObjectInfoPiPoBuffer),
//			new CLBufferKernelArgument("gParticleObjectInfosReordered",
//				//set ifPingPongBufferUseInactiveOne=true, so that the out-dated buffer is bound for writing
//				mParticleSceneRepresentation->mParticleAttributeBuffers->mObjectInfoPiPoBuffer, true )
//
//
//			}
//		)
//	);
}

}
