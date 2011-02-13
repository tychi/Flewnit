
/*
 * LightingSimStageBase.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"
#include "Simulator/SimulatorMetaInfo.h"

#include <typeinfo>


namespace Flewnit {


class LightingSimStageBase
: public SimulationPipelineStage
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	LightingSimStageBase( String name, RenderingTechnique renderingTechnique,ConfigStructNode* simConfigNode);
	virtual ~LightingSimStageBase();

	inline RenderingTechnique getRenderingTechnique()const {return mRenderingTechnique;}

	//iterates over the scenegraph and issues material activations and geometry draw calls on materials
	//compatible to the specific rendering technique;
	void drawAllCompliantGeometry(const ShaderFeaturesLocal& sfl);

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual void initStage()throw(SimulatorException) = 0;
	virtual void validateStage()throw(SimulatorException) = 0;

	inline RenderTarget* getUsedRenderTarget()const{return mUsedRenderTarget;}


protected:
	//init to 0
	RenderTarget* mUsedRenderTarget;

private:

	//NULL if rendering to screen;
	RenderingTechnique mRenderingTechnique;

};



}

