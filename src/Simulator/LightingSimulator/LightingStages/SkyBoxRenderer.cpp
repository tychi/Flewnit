/*
 * SkyBoxRenderer.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#include "SkyBoxRenderer.h"

namespace Flewnit {

SkyBoxRenderer::SkyBoxRenderer(ConfigStructNode* simConfigNode)
:
	LightingSimStageBase( String("SkyBoxRenderer"),RENDERING_TECHNIQUE_DEFAULT_LIGHTING, simConfigNode)
{
	// TODO Auto-generated constructor stub
	assert(0 && "here is the skybox renderer; implement me, or comment me out from config file; i'm not ready to be used yet ;)");
}

SkyBoxRenderer::~SkyBoxRenderer()
{
	// TODO Auto-generated destructor stub
}

bool SkyBoxRenderer::stepSimulation() throw(SimulatorException)
{

	assert(0 && "here is the skybox renderer; implement me, or comment me out from config file; i'm not ready to be used yet ;)");

	return true;
}

void SkyBoxRenderer::initStage()throw(SimulatorException)
{
	assert(0 && "here is the skybox renderer; implement me, or comment me out from config file; i'm not ready to be used yet ;)");

}

void SkyBoxRenderer::validateStage()throw(SimulatorException)
{
	assert(0 && "here is the skybox renderer; implement me, or comment me out from config file; i'm not ready to be used yet ;)");

	//no dependencies;
}



}
