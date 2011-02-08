/*
 * LightingSimStageBase.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "LightingSimStageBase.h"

namespace Flewnit {

LightingSimStageBase::LightingSimStageBase(String name,
		RenderingTechnique renderingTechnique, ConfigStructNode* simConfigNode) :
	SimulationPipelineStage(name, VISUAL_SIM_DOMAIN, simConfigNode),
			mUsedRenderTarget(0), mRenderingTechnique(renderingTechnique) {

}

LightingSimStageBase::~LightingSimStageBase() {
	//nothing to do
}

//iterates over the scenegraph and issues material activations and geometry draw calls on materials
//compatible to the specific rendering technique;
void LightingSimStageBase::drawAllCompliantGeometry() {
	//TODO
	assert(0 && "still to implement");
}

}
