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
#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "UserInterface/WindowManager/WindowManager.h"



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

	//Camera* cam = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

	URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera()
		->setGLViewPort(
				Vector2Di(0,0),
				Vector2Di(WindowManager::getInstance().getWindowResolution())
	);

	//assign lighting shaders to materials after shadowmap gen matrials might have been set by a preceding
	//SM gen stage
	ShaderManager::getInstance().setRenderingScenario(this);

	//update the uniform buffers of the light sources and the shadow map matrices
	LightSourceManager::getInstance().updateLightSourcesUniformBuffer();

	SimulationResourceManager::getInstance().getScene()->traverse(this);

	SimulationResourceManager::getInstance().executeInstancedRendering(this);

	return true;
}

bool DefaultLightingStage::initStage()throw(SimulatorException)
{

	//cause shader generation now:
	ShaderManager::getInstance().setRenderingScenario(this);

	return true;

}

bool DefaultLightingStage::validateStage()throw(SimulatorException)
{
	//shadowmap generator as dependency, but that comes later; TODO

	//TODO assert that no deferred stage exists;

	return true;
}

}
