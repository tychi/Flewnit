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

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Buffer/Buffer.h"
#include "Util/Time/Timer.h"

#include "URE.h"
#include "Util/Time/FPSCounter.h"


#include "MPP/OpenCLProgram/ParticleSimulationProgram.h"


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
		HelperFunctions::log2ui(ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ))
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
		new ParticleSimulationProgram(INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM,
				mParticleUniformGrid,mParticleSceneRepresentation);

	mUpdateDensityProgram =
		new ParticleSimulationProgram(DENSITIY_PARTICLE_SIM_PROGRAM,
				mParticleUniformGrid,mParticleSceneRepresentation);

	mUpdateForce_Integrate_CalcZIndex_Program =
		new ParticleSimulationProgram(FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM,
				mParticleUniformGrid,mParticleSceneRepresentation);


//	for later ;(
//	  mCLProgram_updateRigidBodies(0)

	return true;
}


bool ParticleMechanicsStage::stepSimulation() throw(SimulatorException)
{

	//TODO


//	if(URE_INSTANCE->getFPSCounter()->getTotalRenderedFrames() == 0)
//	{
//		//init step
//		mTimer
//	}




	//TEST ONLY; DELETE CALL!
	//mParticleSceneRepresentation->reorderAttributes();

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
	//TODO
	assert(0&&"TODO implement");

	return 0;
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
