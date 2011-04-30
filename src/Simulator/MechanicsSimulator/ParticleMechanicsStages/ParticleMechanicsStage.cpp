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


	mSplitAndCompactedUniformGridCells =
		new UniformGridBufferSet(
			"particleSplitAndCompactedUniformGridCells",
			ConfigCaster::cast<int>( uniGridSettingsNode.get("numCellsPerDimension",0) )
		);


	  mRadixSorter = new RadixSorter(
			  ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ),
			  HelperFunctions::log2ui(ConfigCaster::cast<int>( generalSettingsNode.get("numMaxParticles",0) ))
	  	  	  //keep rest @ default values;
	  );

//	  mSimulationParametersBuffer(0),
//	  mNumMaxUserForceControlPoints(0),
//	  mUserForceControlPointBuffer(0),
//
//	  mInitial_UpdateForce_Integrate_CalcZIndex_Program(0),
//	  mUpdateDensityProgram(0),
//	  mUpdateForce_Integrate_CalcZIndex_Program(0),
//	  mCLProgram_updateRigidBodies(0)

	return true;
}


bool ParticleMechanicsStage::stepSimulation() throw(SimulatorException)
{

	//TODO

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
CLshare::SimulationParameters* const ParticleMechanicsStage::getSimParams()const
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}



//the returne pointer points directly to the corresponding stride in the host-buffer representation;
//So you can mod the CL behaviour by directly writing to the dereferenced object; The info will be uploaded automatically
//at the begin of every simulation tick;
CLshare::UserForceControlPoint* ParticleMechanicsStage::addUserForceControlPoint(
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
