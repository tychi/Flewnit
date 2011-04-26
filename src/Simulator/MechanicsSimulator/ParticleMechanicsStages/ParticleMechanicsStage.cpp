/*
 * ParticleMechanicsStage.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "ParticleMechanicsStage.h"
#include "Util/Loader/LoaderHelper.h"

namespace Flewnit
{

ParticleMechanicsStage::ParticleMechanicsStage(ConfigStructNode* simConfigNode)
	: SimulationPipelineStage(String("ParticleMechanicsStage"), MECHANICAL_SIM_DOMAIN, simConfigNode),
	  mUseFrameRateIndependentSimulationTimestep(false),
	  mMaxTimestepPerSimulationStep(0.0f),
	  mParticleSceneParentSceneNode(0),
////	  //TODO init correctly already here where possible ?
	  mSimulationParametersBuffer(0),
	  mNumMaxUserForceControlPoints(0),
	  mUserForceControlPointBuffer(0),
	  mParticleSceneRepresentation(0),
	  mParticleUniformGrid(0),
////	  //mStaticTriangleUniformGrid(0), //for later ;)
	  mSplitAndCompactedUniformGridCells(0),
	  mNumCurrentSplitAndCompactedUniformGridCells(0), //<-- this is the bad one... why?
	  mRadixSorter(0),										//-- until here run but srash on shutdown
	  mCLProgram_initial_CalcZIndex(0),
	  mCLProgram_updateDensity(0),
	  mCLProgram_initial_updateForce_integrate_calcZIndex(0),
	  mCLProgram_updateForce_integrate_calcZIndex(0)

{
	//everything done in init()

}

ParticleMechanicsStage::~ParticleMechanicsStage()
{
//	delete  mParticleSceneRepresentation;
//	delete  mParticleUniformGrid;
//	//delete  mStaticTriangleUniformGrid; //for later ;)
//	delete mSplitAndCompactedUniformGridCells;
//
//	delete mRadixSorter;
//
//	delete mCLProgram_initial_CalcZIndex;
//	delete mCLProgram_updateDensity;
//	delete mCLProgram_initial_updateForce_integrate_calcZIndex;
//	delete mCLProgram_updateForce_integrate_calcZIndex;

}

bool ParticleMechanicsStage::initStage()throw(SimulatorException)
{
//	ConfigStructNode& generalSettingsNode = mSimConfigNode->get("generalSettings",0);
//
//	  mUseFrameRateIndependentSimulationTimestep =
//		ConfigCaster::cast<bool>(
//					generalSettingsNode.get("useFrameRateIndependentSimulationTimestep",0)
//		);

//	  mMaxTimestepPerSimulationStep(0.0f),
//	  mParticleSceneParentSceneNode(0),
//	  //TODO init correctly already here where possible ?
//	  mSimulationParametersBuffer(0),
//	  mNumMaxUserForceControlPoints(0),
//	  mUserForceControlPointBuffer(0),
//	  mParticleSceneRepresentation(0),
//	  mParticleUniformGrid(0),
//	  //mStaticTriangleUniformGrid(0), //for later ;)
//	  mSplitAndCompactedUniformGridCells(0),
//	  mNumCurrentSplitAndCompactedUniformGridCells(0),
//	  mRadixSorter(0),
//	  mCLProgram_initial_CalcZIndex(0),
//	  mCLProgram_updateDensity(0),
//	  mCLProgram_initial_updateForce_integrate_calcZIndex(0),
//	  mCLProgram_updateForce_integrate_calcZIndex(0)

	return true;
}


bool ParticleMechanicsStage::stepSimulation() throw(SimulatorException)
{
	//TODO
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
