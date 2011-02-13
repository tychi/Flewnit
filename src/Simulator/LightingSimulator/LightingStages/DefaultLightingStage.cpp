/*
 * DefaultLightingStage.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "DefaultLightingStage.h"

#include "Util/Log/Log.h"
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit {

DefaultLightingStage::DefaultLightingStage(ConfigStructNode* simConfigNode)
: LightingSimStageBase("ShadowMapGenerationStage",RENDERING_TECHNIQUE_DEFAULT_LIGHTING,simConfigNode)
{
	//TODO configure main render target etc..

}

DefaultLightingStage::~DefaultLightingStage()
{
	//nothing to do
}

bool DefaultLightingStage::stepSimulation() throw(SimulatorException)
{
	LOG<<DEBUG_LOG_LEVEL<<"DefaultLightingStage generator in action! ;(;\n";

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
