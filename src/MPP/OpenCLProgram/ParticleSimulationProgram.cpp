/*
 * ParticleSimulationProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "ParticleSimulationProgram.h"

#include "Scene/ParticleSceneRepresentation.h"
#include "CLProgram.h"
#include "Util/HelperFunctions.h"
#include "Simulator/ParallelComputeManager.h"

#include "Simulator/MechanicsSimulator/ParticleMechanicsStages/ParticleMechanicsStage.h"
#include "Scene/UniformGrid.h"
#include "CLKernelArguments.h"

#include "Buffer/Buffer.h"
#include "Buffer/PingPongBuffer.h"

#include <grantlee/engine.h>
#include "Scene/ParticleAttributeBuffers.h"


namespace Flewnit
{

ParticleSimulationProgram::ParticleSimulationProgram(
		ParticleSimulationProgramType type, ParticleMechanicsStage* particleMechStage)
:
	UniformGridRelatedProgram(
		type == INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM
			? "_initial_updateForce_integrate_calcZIndex.cl"
			: (
				type == DENSITIY_PARTICLE_SIM_PROGRAM
				? "updateDensity.cl"
				: "updateForce_integrate_calcZIndex.cl"
			),
			particleMechStage->mParticleUniformGrid,
			MECHANICAL_SIM_DOMAIN
	),
	mParticleMechanicsStage(particleMechStage),
	mParticleSimulationProgramType(type)
{

}

ParticleSimulationProgram::~ParticleSimulationProgram()
{
	//nothing to do
}


//calls UniformGridRelatedProgram::setupTemplateContext() and sets up particle simulation related template params;
void ParticleSimulationProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	UniformGridRelatedProgram::setupTemplateContext(contextMap);

	contextMap.insert(
		"numMaxParticlesPerRigidBody",
		mParticleMechanicsStage->mParticleSceneRepresentation->getNumMaxParticlesPerRigidBody()
	);


	contextMap.insert(
		"numFluidObjects",
		mParticleMechanicsStage->mParticleSceneRepresentation->getNumMaxFluids()
	);
}


void ParticleSimulationProgram::createKernels()
{
	if(mParticleSimulationProgramType == INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM)
	{
		mKernels["kernel_initial_CalcZIndex"] = new CLKernel(
			this,
			"kernel_initial_CalcZIndex",

			new CLKernelWorkLoadParams(
				mParticleMechanicsStage->mParticleSceneRepresentation->getNumTotalParticles(),
				HelperFunctions::floorToNextPowerOfTwo(
					PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
				)
			),

			new CLKernelArguments(
				{
					new CLBufferKernelArgument("cGridPosToZIndexLookupTable",
						mParticleMechanicsStage->mParticleUniformGrid->getZIndexLookupTable()
					),
					new CLBufferKernelArgument("cSimParams",
						mParticleMechanicsStage->mSimulationParametersBuffer
					),
					new CLBufferKernelArgument("gPositions",
						mParticleMechanicsStage->mParticleSceneRepresentation
							->getParticleAttributeBuffers()->getPositionsPiPoBuffer()
					),
					new CLBufferKernelArgument("gZIndices",
						//yes, bind to and write to ACTIVE component;
						//this is an initialization kernel, so we do write to those buffers
						//which are to be read
						mParticleMechanicsStage->mParticleSceneRepresentation
						->getParticleAttributeBuffers()->getZIndicesPiPoBuffer()
					),
				}
			)
		);

	}


	//----------------------------------------------------------------------------------




	String SPHkernelName =
		(mParticleSimulationProgramType == INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM)
		? "kernel_initial_updateForce_integrate_calcZIndex"
		:(
			(mParticleSimulationProgramType == DENSITIY_PARTICLE_SIM_PROGRAM)
			? "kernel_updateDensity"
			: "kernel_updateForce_integrate_calcZIndex"
		);


	std::vector< CLKernelArgumentBase* > argumentVector;

	//begin with the arguments common to all SPH kernels:
	//constant global and abstract object param buffers ...
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"cSimParams",
			mParticleMechanicsStage->mSimulationParametersBuffer ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"cGridPosToZIndexLookupTable",
			mParticleMechanicsStage->mParticleUniformGrid->getZIndexLookupTable() ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"cObjectGenericFeatures",
			mParticleMechanicsStage->mParticleSceneRepresentation->mObjectGenericFeaturesBuffer ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"cUserForceControlPoints",
			mParticleMechanicsStage->mUserForceControlPointBuffer ) );


	//...  and uniform grid buffers:
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"gSimWorkGroups_ParticleStartIndex",
			mParticleMechanicsStage->mSplitAndCompactedUniformGridCells->getStartIndices() ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"gSimWorkGroups_NumParticles",
			mParticleMechanicsStage->mSplitAndCompactedUniformGridCells->getElementCounts() ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"gUniGridCells_ParticleStartIndex",
			mParticleMechanicsStage->mParticleUniformGrid->getBufferSet("particles")->getStartIndices() ) );
	argumentVector.push_back(
		new CLBufferKernelArgument(
			"gUniGridCells_NumParticles",
			mParticleMechanicsStage->mParticleUniformGrid->getBufferSet("particles")->getElementCounts() ) );


	//density kernel has other params but the both force/integrate/zindexCalc kernels: branch;
	ParticleAttributeBuffers* attrBuffers =
		mParticleMechanicsStage->mParticleSceneRepresentation->getParticleAttributeBuffers();

	if(mParticleSimulationProgramType == DENSITIY_PARTICLE_SIM_PROGRAM)
	{
		//build active components, read/write buffer, has no neigbour-particle density reads occur in this kernel
		//Note that because of this, no buffer toggling has to be done after this kernel
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gPositionsOld",
				attrBuffers->getPositionsPiPoBuffer() ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gDensitiesNew",
				attrBuffers->getDensitiesPiPoBuffer() ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gParticleObjectInfos",
				attrBuffers->getObjectInfoPiPoBuffer() ) );
	}
	else
	{
		//don't toggle this buffer after the kernel invocation
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gParticleObjectInfos",
				attrBuffers->getObjectInfoPiPoBuffer() ) );

	    //write-only buffer; IMPORTANT: bind the activ ping pong ccomponent to this arg, although it is a write arg;
	    //don't toggle this buffer after the kernel invocation
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gZindicesNew",
				//bind inactive component
				attrBuffers->getZIndicesPiPoBuffer() ) );


		//toggle all following particle attribute buffers after kernel invocation:
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gPositionsOld",
				attrBuffers->getPositionsPiPoBuffer(),false ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gPositionsNew",
				attrBuffers->getPositionsPiPoBuffer(),true ) );

		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gDensitiesOld",
				attrBuffers->getDensitiesPiPoBuffer(),false ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gDensitiesNew",
				attrBuffers->getDensitiesPiPoBuffer(),true ) );

		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gCorrectedVelocitiesOld",
				attrBuffers->getCorrectedVelocitiesPiPoBuffer(), false ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gCorrectedVelocitiesNew",
				attrBuffers->getCorrectedVelocitiesPiPoBuffer(), true ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gPredictedVelocitiesCurrent",
				attrBuffers->getPredictedVelocitiesPiPoBuffer(),false ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gPredictedVelocitiesFuture",
				attrBuffers->getPredictedVelocitiesPiPoBuffer(), true ) );

		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gAccelerationsOld",
				attrBuffers->getLastStepsAccelerationsPiPoBuffer(), false ) );
		argumentVector.push_back(
			new CLBufferKernelArgument(
				"gAccelerationsNew",
				attrBuffers->getLastStepsAccelerationsPiPoBuffer(), true ) );
	}



	mKernels[SPHkernelName] = new CLKernel(
		this,
		SPHkernelName,
		new CLKernelWorkLoadParams(
			//empty as unkknown yet, vary with stream compaction of uniform grid results
		),
		new CLKernelArguments(argumentVector)
	);





}


}
