/*
 * ShadowMapGenerationStage.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once


#include "Simulator/LightingSimulator/LightingSimStageBase.h"

namespace Flewnit {

class ShadowMapGenerationStage
: public LightingSimStageBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	ShadowMapGenerationStage(ConfigStructNode* simConfigNode);
	virtual ~ShadowMapGenerationStage();


	virtual bool stepSimulation() throw(SimulatorException);
	virtual void initStage()throw(SimulatorException);
	virtual void validateStage()throw(SimulatorException) ;


private:

	//Texture* mShadowMapDepthTexture; //<-- wil lbe integrated as member of the rendertarget;
	//Shader* mShadowMapGenerationShader;

	int mShadowMapResolution; //shadowmap is alwayas of squared size:

};

}

