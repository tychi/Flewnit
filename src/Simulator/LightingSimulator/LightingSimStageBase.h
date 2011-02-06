
/*
 * LightingSimStageBase.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"
#include "Simulator/SimulatorMetaInfo.h"



namespace Flewnit {


class LightingSimStageBase
: public SimulationPipelineStage
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	RenderingTechnique mRenderingTechnique;

public:
	LightingSimStageBase( String name, RenderingTechnique renderingTechnique);
	virtual ~LightingSimStageBase();

	inline RenderingTechnique getRenderingTechnique()const {return mRenderingTechnique;}
	void drawAllCompliantGeometry();

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual void initStage()throw(SimulatorException) = 0;
	virtual void validateStage()throw(SimulatorException) = 0;
};



}

