/*
 * LightingSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "../SimulatorInterface.h"



namespace Flewnit
{


class LightingSimulator:
	public SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	LightingSimulator(ConfigStructNode* simConfigNode);
	virtual ~LightingSimulator();

	virtual bool stepSimulation()throw(SimulatorException) ;
	//build pipeline according to config;
	virtual bool initPipeLine()throw(SimulatorException) ;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()throw(SimulatorException) ;

	ShaderManager* getShaderManager()const;
	RenderTarget* getGlobalRenderTarget()const;
	LightSourceManager* getLightSourceManager()const;

	Camera* getMainCamera()const;
	inline void setCurrentlyActiveCamera(Camera* cam){mCurrentlyActiveCamera=cam;}
	inline Camera* getCurrentlyActiveCamera()const{return mCurrentlyActiveCamera;}

	//debug
	void testStuff();

private:


	ShaderFeaturesGlobal parseGlobalShaderFeatureFromConfig();
	ShadingFeatures parseSupportedShadingFeaturesFromConfig();

	RenderTarget* mGlobalRenderTarget;
	Camera* mMainCamera;
	Camera* mCurrentlyActiveCamera;
	ShaderManager* mShaderManager;
	LightSourceManager* mLightSourceManager;

};

}

