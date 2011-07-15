/*
 * ParticleLiquidDrawStage.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once



#include "Simulator/LightingSimulator/LightingSimStageBase.h"



namespace Flewnit
{


class ParticleLiquidDrawStage
: public LightingSimStageBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	ParticleLiquidDrawStage(ConfigStructNode* simConfigNode);
	virtual ~ParticleLiquidDrawStage();

	virtual bool stepSimulation() throw(SimulatorException);
	virtual bool initStage()throw(SimulatorException);
	virtual bool validateStage()throw(SimulatorException);

	virtual bool checkCompatibility(VisualMaterial* visMat);

private:
	TextureShowShader* mTextureShowShader;

	//some hack to work around the depth buffer f***up between screen and FBO;
	//will be obsolete when fluid is visualized "correctly"
	Texture* mCompositedRendering;

};

}

