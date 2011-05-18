/*
 * ParticleMechanicsStage.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "ParticleMechanicsStage.h"
#include "Util/Loader/LoaderHelper.h"
#include "Simulator/SimulationResourceManager.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneGraph.h"
#include "Scene/ParticleSceneRepresentation.h"
#include "Simulator/ParallelComputeManager.h"
#include "Scene/UniformGrid.h"
#include "Util/RadixSorter.h"
#include "Util/HelperFunctions.h"

#include "Buffer/PingPongBuffer.h"


#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Buffer/Buffer.h"
#include "Util/Time/Timer.h"

#include "URE.h"
#include "Util/Time/FPSCounter.h"


#include "MPP/OpenCLProgram/ParticleSimulationProgram.h"
#include "Material/ParticleLiquidVisualMaterial.h"
#include "Material/ParticleFluidMechMat.h"
#include "WorldObject/ParticleFluid.h"
#include "Scene/ParticleAttributeBuffers.h"
#include "Util/Log/Log.h"
#include "MPP/OpenCLProgram/CLKernelArguments.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "../../../MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

namespace Flewnit
{

ParticleMechanicsStage::ParticleMechanicsStage(ConfigStructNode* simConfigNode)
	: SimulationPipelineStage(String("ParticleMechanicsStage"), MECHANICAL_SIM_DOMAIN, simConfigNode),
	  mUseFrameRateIndependentSimulationTimestep(false),
	  mMaxTimestepPerSimulationStep(0.0f),
	  mParticleSceneParentSceneNode(0),
	  mParticleSceneRepresentation(0),
	  mParticleUniformGrid(0),
	  mSplitAndCompactedUniformGridCells(0),
	  mNumCurrentSplitAndCompactedUniformGridCells(0),
	  mRadixSorter(0),

	  mUseConstantTimeStep(false),
	  mConstantTimeStep(0.01666f),
	  mTimestepScale(1.0f),
	  mTimer(0),

	  mSimulationParametersBuffer(0),
	  mNumMaxUserForceControlPoints(0),
	  mNumCurrentUserForceControlPoints(0),
	  mUserForceControlPointBuffer(0),

	  mInitial_UpdateForce_Integrate_CalcZIndex_Program(0),
	  mUpdateDensityProgram(0),
	  mUpdateForce_Integrate_CalcZIndex_Program(0),
	  mCLProgram_updateRigidBodies(0)

{
	//everything done in init()

}

ParticleMechanicsStage::~ParticleMechanicsStage()
{
	delete  mParticleSceneRepresentation;
	// DON'T delete  mParticleUniformGrid; It is a worldobjects, hence managed by the scenegraph!;
	delete mSplitAndCompactedUniformGridCells;
	delete mRadixSorter;
	delete mTimer;
}

bool ParticleMechanicsStage::initStage()throw(SimulatorException)
{
	ConfigStructNode& generalSettingsNode = mSimConfigNode->get("generalSettings",0);

	mUseFrameRateIndependentSimulationTimestep =
		ConfigCaster::cast<bool>(
					generalSettingsNode.get("useFrameRateIndependentSimulationTimestep",0)
		);

	mMaxTimestepPerSimulationStep =
		ConfigCaster::cast<float>(
			generalSettingsNode.get("maxTimestepPerSimulationStep",0)
		);

	mParticleSceneParentSceneNode =
		SimulationResourceManager::getInstance().getSceneGraph()->root().addChild(
			new SceneNode("particleSceneParentSceneNode", PURE_NODE)
		);


	mParticleSceneRepresentation = new ParticleSceneRepresentation(
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ),
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxFluids",0) ),
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxRigidBodies",0) ),
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticlesPerRigidBody",0) ),
		ConfigCaster::cast<float>( generalSettingsNode.get("voxelSideLengthRepresentedByRigidBodyParticle",0) )
	);

	mParticleSceneRepresentation->associateParticleAttributeBuffersWithRenderingResults(mRenderingResults);

	//-------------------------------------

	ConfigStructNode& uniGridSettingsNode = mSimConfigNode->get("UniformGrid",0);

	mParticleUniformGrid = new UniformGrid(
		"particleUniformGrid",
		ConfigCaster::cast<int>( uniGridSettingsNode.get("numCellsPerDimension",0) ),
		ConfigCaster::cast<Vector4D>( uniGridSettingsNode.get("minCornerPosition",0) ),
		ConfigCaster::cast<Vector4D>( uniGridSettingsNode.get("extendsOfOneCell",0) ),
		ConfigCaster::cast<int>( uniGridSettingsNode.get("numMaxElementsPerSimulationWorkGroup",0) ),
		//alloc only one UniformGridBufferSet, namely for particles; triangle come later ;)
		std::vector<String>{ "particles" }
	);
	//add to scene graph to be debug drawable
	mParticleSceneParentSceneNode->addChild(mParticleUniformGrid);


	mSplitAndCompactedUniformGridCells = new UniformGridBufferSet(
		"particleSplitAndCompactedUniformGridCells",
		ConfigCaster::cast<int>( uniGridSettingsNode.get("numCellsPerDimension",0) )
	);


	mRadixSorter = new RadixSorter(
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ),
		//HelperFunctions::log2ui(ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ))
		//log2 ov total cell count yield the needed radix sort count
		HelperFunctions::log2ui( mParticleUniformGrid->getNumCellsPerDimension()*mParticleUniformGrid->getNumCellsPerDimension()*mParticleUniformGrid->getNumCellsPerDimension())
	);


	mUseConstantTimeStep = ConfigCaster::cast<bool>( generalSettingsNode.get("useConstantTimestep",0) );
	mConstantTimeStep = ConfigCaster::cast<float>( generalSettingsNode.get("constantTimeStep",0) );
	mTimestepScale = ConfigCaster::cast<float>( generalSettingsNode.get("timestepScale",0) );
	mTimer = Timer::create();



	mSimulationParametersBuffer = SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
		String("particleSimulationParametersBuffer"),
		//we only need one instance ;)
		sizeof( CLShare::SimulationParameters ),
		true
	);

	//inititialize the sim params:
	getSimParams()->setUniformGridParams(
			mParticleUniformGrid->getMinCornerPosition(),
			mParticleUniformGrid->getExtendsOfOneCell());

	getSimParams()->setSimulationDomainBorders(
		ConfigCaster::cast<Vector4D>( generalSettingsNode.get("simulationDomainBorderMin",0) ),
		ConfigCaster::cast<Vector4D>( generalSettingsNode.get("simulationDomainBorderMax",0) )
	);
	getSimParams()->setGravityAcceleration(
		ConfigCaster::cast<Vector4D>( generalSettingsNode.get("gravityAcceleration",0) )
	);

	//init current num to zero
	getSimParams()->setNumUserForceControlPoints(0);

	getSimParams()->setPenaltyForceConstants(
		ConfigCaster::cast<float>( generalSettingsNode.get("penaltyForceSpringConstant",0) ),
		ConfigCaster::cast<float>( generalSettingsNode.get("penaltyForceDamperConstant",0) )
	);

	getSimParams()->setSPHsupportRadius(
		ConfigCaster::cast<float>( generalSettingsNode.get("SPHSupportRadius",0) )
	);

	//init to mConstantTimeStep, because otherweise, the timestep would change every frame anyway;
	getSimParams()->setTimeStep(
		mConstantTimeStep
	);

	//before forgetting it, upload ;(
	mSimulationParametersBuffer->copyFromHostToGPU(true);


	mNumMaxUserForceControlPoints =
		ConfigCaster::cast<int>( generalSettingsNode.get("numMaxUserForceControlPoints",0) );
	assert(mNumMaxUserForceControlPoints > 0);
	mUserForceControlPointBuffer = SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSimulationUserForceControlPointBuffer"),
			mNumMaxUserForceControlPoints * sizeof( CLShare::UserForceControlPoint ),
			true
		);


	//--------------------------------------------------------------------

	mInitial_UpdateForce_Integrate_CalcZIndex_Program =
		new ParticleSimulationProgram(INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM,	this);

//alloc and build when needed; to reduce compile times
	//TODO

	mUpdateDensityProgram =
		new ParticleSimulationProgram(DENSITIY_PARTICLE_SIM_PROGRAM, this);

	mUpdateForce_Integrate_CalcZIndex_Program =
		new ParticleSimulationProgram(FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM, this);


//	for later ;(
//	  mCLProgram_updateRigidBodies(0)


	parseParticleScene();




	return true;
}


void ParticleMechanicsStage::parseParticleScene()
{
	ConfigStructNode& particleSceneConfigNode = mSimConfigNode->get("ParticleScene",0);
	unsigned int numParticleFluids = particleSceneConfigNode.get("ParticleFluid").size();
	assert(numParticleFluids <= mParticleSceneRepresentation->getNumMaxFluids());

	for(unsigned int fluidRunner = 0; fluidRunner< numParticleFluids; fluidRunner++ )
	{
		ConfigStructNode& fluidNode  = particleSceneConfigNode.get("ParticleFluid",fluidRunner);
		ConfigStructNode& fluidVisMatNode  = fluidNode.get("ParticleLiquidVisualMaterial",fluidRunner);
		ConfigStructNode& fluidMechMatNode  = fluidNode.get("ParticleFluidMechMat",fluidRunner);

		mParticleSceneParentSceneNode->addChild(
			mParticleSceneRepresentation->createParticleFluid(
				ConfigCaster::cast<String>( fluidNode.get("name",0) ),
				ConfigCaster::cast<int>( fluidNode.get("numContainingParticles",0) ),
				AABB(
					ConfigCaster::cast<Vector4D>( fluidNode.get("spawnAABBMin",0) ),
					ConfigCaster::cast<Vector4D>( fluidNode.get("spawnAABBMax",0) )
				),
				ConfigCaster::cast<Vector4D>( fluidNode.get("initialVelocity",0) ),
				new ParticleLiquidVisualMaterial(
					ConfigCaster::cast<String>( fluidVisMatNode.get("name",0) ),
					ConfigCaster::cast<Vector4D>( fluidVisMatNode.get("liquidColor",0) ),
					ConfigCaster::cast<float>( fluidVisMatNode.get("particleDrawRadius",0) ),
					ConfigCaster::cast<Vector4D>( fluidVisMatNode.get("foamColor",0) ),
					ConfigCaster::cast<float>( fluidVisMatNode.get("foamGenerationAcceleration",0) ),
					ConfigCaster::cast<float>( fluidVisMatNode.get("shininess",0) ),
					ConfigCaster::cast<float>( fluidVisMatNode.get("reflectivity",0) ),
					ConfigCaster::cast<float>( fluidVisMatNode.get("refractivity",0) ),
					ConfigCaster::cast<int>( fluidVisMatNode.get("numCurvatureFlowRelaxationSteps",0) )
				),
				new ParticleFluidMechMat(
					ConfigCaster::cast<String>( fluidMechMatNode.get("name",0) ),
					ConfigCaster::cast<int>( fluidNode.get("numContainingParticles",0) ),
					ConfigCaster::cast<float>( fluidMechMatNode.get("massPerParticle",0) ),
					ConfigCaster::cast<float>( fluidMechMatNode.get("restDensity",0) ),
					ConfigCaster::cast<float>( fluidMechMatNode.get("gasConstant",0) ),
					ConfigCaster::cast<float>( fluidMechMatNode.get("viscosity",0) )
				)
			)
		)
		;
	}

	mParticleSceneRepresentation->getParticleAttributeBuffers()->flushBuffers();
	mParticleSceneRepresentation->flushObjectBuffers();
}


bool ParticleMechanicsStage::stepSimulation() throw(SimulatorException)
{
	//acquire and relase done already by mechanics simulator;
	//PARA_COMP_MANAGER->acquireSharedBuffersForCompute();

	//getSimParams()->setSimulationDomainBorders(Vector4D(0.0f,0.0f,0.0f,0.0f),Vector4D(100.0f,100.0f,100.0f,0.0f));

	mSimulationParametersBuffer->copyFromHostToGPU(true);
	mUserForceControlPointBuffer->copyFromHostToGPU(true);

	//PARA_COMP_MANAGER->barrierCompute();



	if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames() == 0)
	{
		//init step
		mInitial_UpdateForce_Integrate_CalcZIndex_Program->getKernel("kernel_initial_CalcZIndex")
			->run(
				//empty event vec, no dependencies
				EventVector()
			);

		mParticleSceneRepresentation->getParticleAttributeBuffers()->dumpBuffers(
				"initialZIndexCalc",URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(), false );
	}


	//------------------------------------------------------------------------------------------------------
	//{sort and reorder



	PARA_COMP_MANAGER->barrierCompute();



	if(
	    (URE_INSTANCE->bufferDumpCondition() )
	)
	{
		mParticleSceneRepresentation->getParticleAttributeBuffers()->dumpBuffers(
			"AttributeBufferDump_zIndexOnly_BEFORE_RadixSort",
			URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
			false,
			true
		);
	}



	//PARA_COMP_MANAGER->barrierCompute();



	mRadixSorter->sort(
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getZIndicesPiPoBuffer(),
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getOldIndicesPiPoBuffer()
	);


	//PARA_COMP_MANAGER->getCommandQueue().enqueueBarrier();
	//PARA_COMP_MANAGER->getCommandQueue().flush();
	//PARA_COMP_MANAGER->getCommandQueue().finish();


	PARA_COMP_MANAGER->barrierCompute();



	mParticleSceneRepresentation->reorderAttributes();



	PARA_COMP_MANAGER->barrierCompute();



	if(
	    (URE_INSTANCE->bufferDumpCondition() )
	)
	{
		mParticleSceneRepresentation->getParticleAttributeBuffers()->dumpBuffers(
			"AttributeBufferDump_zIndexOnly_AFTER_RadixSort",
			URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
			false,
			true
		);
	}


	//} end sort and reorder
	//------------------------------------------------------------------------------------------------------


	PARA_COMP_MANAGER->barrierCompute();




	//{ uniform grid stuff

	mParticleUniformGrid->updateCells(
		"particles",
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getZIndicesPiPoBuffer()
	);




	PARA_COMP_MANAGER->barrierCompute();


	unsigned int numCurrentSPHSimulationWorkGroups=0;
	numCurrentSPHSimulationWorkGroups =
			mParticleUniformGrid->splitAndCompactCells(
				"particles",
				mSplitAndCompactedUniformGridCells
			);


	PARA_COMP_MANAGER->barrierCompute();



	LOG<<DEBUG_LOG_LEVEL<<"current number of simulation work groups for SPH related kernels: "
			<< numCurrentSPHSimulationWorkGroups << ";\n";


	//}

	//PARA_COMP_MANAGER->getCommandQueue().enqueueBarrier();
	//PARA_COMP_MANAGER->getCommandQueue().flush();
	//PARA_COMP_MANAGER->getCommandQueue().finish();

	//{ SPH stuff

		CLKernelWorkLoadParams currentSPHKErnelWorkLoadParams(
				numCurrentSPHSimulationWorkGroups
				//std::min((uint)(10000),numCurrentSPHSimulationWorkGroups)
					* mParticleUniformGrid->getNumMaxElementsPerSimulationWorkGroup(),
				mParticleUniformGrid->getNumMaxElementsPerSimulationWorkGroup()
		);

		mUpdateDensityProgram->getKernel("kernel_updateDensity")->run(
			EventVector{
				CLProgramManager::getInstance().getProgram("reorderParticleAttributes.cl")
						->getKernel("kernel_reorderParticleAttributes")
						->getEventOfLastKernelExecution(),
				CLProgramManager::getInstance().getProgram("splitAndCompactUniformGrid.cl")
					->getKernel("kernel_splitAndCompactUniformGrid")
					->getEventOfLastKernelExecution()
			},
			currentSPHKErnelWorkLoadParams
		);



		//PARA_COMP_MANAGER->getCommandQueue().enqueueBarrier();
		//PARA_COMP_MANAGER->getCommandQueue().flush();
	//	PARA_COMP_MANAGER->getCommandQueue().finish();



		if(
		    (URE_INSTANCE->bufferDumpCondition() )
		)
		{
			mParticleSceneRepresentation->getParticleAttributeBuffers()->dumpBuffers(
				"AttributeBufferDump_DensityComputation",
				URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
				false
			);
		}



		PARA_COMP_MANAGER->barrierCompute();



		if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames() == 0)
		{
			//init integrate step
			mInitial_UpdateForce_Integrate_CalcZIndex_Program->getKernel("kernel_initial_updateForce_integrate_calcZIndex")
				->run(
					EventVector{
						mUpdateDensityProgram->getKernel("kernel_updateDensity")->getEventOfLastKernelExecution(),
					},
					currentSPHKErnelWorkLoadParams
				);
		}
		else
		{
			//default integrate step
			mUpdateForce_Integrate_CalcZIndex_Program->getKernel("kernel_updateForce_integrate_calcZIndex")
				->run(
					EventVector{
						mUpdateDensityProgram->getKernel("kernel_updateDensity")->getEventOfLastKernelExecution(),
					},
					currentSPHKErnelWorkLoadParams
			);

		}



		PARA_COMP_MANAGER->barrierCompute();



		//mParticleSceneRepresentation->getParticleAttributeBuffers()->getZIndicesPiPoBuffer()->toggleBuffers(); <--ping ponging only needed during radix sort!
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getPositionsPiPoBuffer()->toggleBuffers();
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getCorrectedVelocitiesPiPoBuffer()->toggleBuffers();
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getPredictedVelocitiesPiPoBuffer()->toggleBuffers();
		mParticleSceneRepresentation->getParticleAttributeBuffers()->getLastStepsAccelerationsPiPoBuffer()->toggleBuffers();

		if(
		    (URE_INSTANCE->bufferDumpCondition() )
		)
		{
			mParticleSceneRepresentation->getParticleAttributeBuffers()->dumpBuffers(
				"AttributeBufferDump_ForceIntrZIndComputation_AfterToggle",
				URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames(),
				false
			);
		}

	//} //end SPH


		PARA_COMP_MANAGER->getCommandQueue().enqueueBarrier();
		PARA_COMP_MANAGER->getCommandQueue().flush();
		//PARA_COMP_MANAGER->getCommandQueue().finish();



	//reset element counts to zero
	mParticleUniformGrid->getBufferSet("particles")->clearElementCounts();



	PARA_COMP_MANAGER->getCommandQueue().enqueueBarrier();
	PARA_COMP_MANAGER->getCommandQueue().flush();
	//PARA_COMP_MANAGER->getCommandQueue().finish();







	return true;
}


bool ParticleMechanicsStage::validateStage()throw(SimulatorException)
{
	//TODO
	return true;
}


//directly do re reinterpret_cast on mSimulationParametersBuffer, no dedicated object necessary;
//read only acces for app
CLShare::SimulationParameters* const ParticleMechanicsStage::getSimParams()const
{
	//return a direct pointer to the host component of the CL buffer;
	return reinterpret_cast<CLShare::SimulationParameters*>(mSimulationParametersBuffer->getCPUBufferHandle());
}



//the returne pointer points directly to the corresponding stride in the host-buffer representation;
//So you can mod the CL behaviour by directly writing to the dereferenced object; The info will be uploaded automatically
//at the begin of every simulation tick;
CLShare::UserForceControlPoint* ParticleMechanicsStage::addUserForceControlPoint(
		const Vector4D& forceOriginWorldPos,
		float influenceRadius,
		float intensity //positive: push away; negative: pull towards origin;
)throw(BufferException)
{
	CLShare::UserForceControlPoint* ctrlPoint =
			&(
				reinterpret_cast<CLShare::UserForceControlPoint*>(
						mUserForceControlPointBuffer->getCPUBufferHandle( ))[mNumCurrentUserForceControlPoints++]
		    );

	ctrlPoint->setInfluenceRadius(influenceRadius);
	ctrlPoint->setForceOriginWorldPos( forceOriginWorldPos );
	ctrlPoint->setIntensity(intensity);

	getSimParams()->setNumUserForceControlPoints(mNumCurrentUserForceControlPoints);

	return ctrlPoint;
}

void setGravityAcceleration(const Vector4D& gravAcc)
{
	//TODO
	assert(0&&"TODO implement");
}

void setSPHSupportRadius(float val)
{
	//TODO
	assert(0&&"TODO implement");
}

}
