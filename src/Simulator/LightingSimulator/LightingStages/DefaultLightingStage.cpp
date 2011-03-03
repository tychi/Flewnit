/*
 * DefaultLightingStage.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "DefaultLightingStage.h"

#include "MPP/Shader/ShaderManager.h"
#include "Simulator/SimulationResourceManager.h"

#include "Scene/Scene.h"

#include "Util/Log/Log.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "URE.h"
#include "Simulator/SimulatorInterface.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"



namespace Flewnit {

DefaultLightingStage::DefaultLightingStage(ConfigStructNode* simConfigNode)
: LightingSimStageBase(
		"DefaultLightingStage",
		RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
		//mask for shadable materials
		VisualMaterialFlags(false,false,true,false,false,false),
		simConfigNode)
{
	//TODO configure main render target etc..

}

DefaultLightingStage::~DefaultLightingStage()
{
	//nothing to do
}

bool DefaultLightingStage::stepSimulation() throw(SimulatorException)
{
	//LOG<<DEBUG_LOG_LEVEL<<"DefaultLightingStage generator in action! ;(;\n";

	//update the uniform buffers of the light sources and the shadow map matrices
	LightSourceManager::getInstance().updateLightSourcesUniformBuffer(
		URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera());

	SimulationResourceManager::getInstance().getScene()->traverse(this);

	SimulationResourceManager::getInstance().executeInstancedRendering();

	return true;
}

void DefaultLightingStage::initStage()throw(SimulatorException)
{

	//cause shader generation now:
	ShaderManager::getInstance().setRenderingScenario(this);

}

void DefaultLightingStage::validateStage()throw(SimulatorException)
{
	//shadowmap generator as dependency, but that comes later; TODO

	//TODO assert that no deferred stage exists;
}

}
