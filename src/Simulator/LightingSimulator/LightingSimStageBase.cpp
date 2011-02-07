/*
 * LightingSimStageBase.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "LightingSimStageBase.h"

namespace Flewnit
{

LightingSimStageBase::LightingSimStageBase( String name, RenderingTechnique renderingTechnique,ConfigStructNode* simConfigNode)
:
	SimulationPipelineStage( name, VISUAL_SIM_DOMAIN, simConfigNode),
	mUsedRenderTarget(0),
	mRenderingTechnique(renderingTechnique)
{

}

}
