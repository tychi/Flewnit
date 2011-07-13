/*
 * ParticleLiquidDrawStage.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "ParticleLiquidDrawStage.h"

#include "MPP/Shader/ShaderManager.h"
#include "Simulator/SimulationResourceManager.h"

#include "Scene/SceneGraph.h"

#include "Util/Log/Log.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "URE.h"
#include "Simulator/SimulatorInterface.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"
#include "Util/Loader/LoaderHelper.h"
#include "Material/ParticleLiquidVisualMaterial.h"
#include "MPP/Shader/TextureShowShader.h"
#include "Buffer/Texture.h"




namespace Flewnit {

ParticleLiquidDrawStage::ParticleLiquidDrawStage(ConfigStructNode* simConfigNode)
: LightingSimStageBase(
		"ParticleLiquidDrawStage",
		RENDERING_TECHNIQUE_CUSTOM,
		//mask for custom stuff; isn'nt used anyway, because there is no scene graph traversal
		VisualMaterialFlags(false,false,false,false,false,true),
		simConfigNode),
		mTextureShowShader(0)
{
	//TODO configure main render target etc..

}

ParticleLiquidDrawStage::~ParticleLiquidDrawStage()
{
	//this stage owns this target, so it will destroy it
	delete mUsedRenderTarget;
}

bool ParticleLiquidDrawStage::stepSimulation() throw(SimulatorException)
{
	URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera()
		->setGLViewPort(
				Vector2Di(0,0),
				Vector2Di(WindowManager::getInstance().getWindowResolution())
	);

//	if(! mRenderToScreen)
//	{
//		//render to FBO
//		mUsedRenderTarget->bind();
//		RenderTarget::setEnableDepthTest(true);
//		mUsedRenderTarget->attachStoredColorTexture(FINAL_RENDERING_SEMANTICS,0);
//		mUsedRenderTarget->renderToAttachedTextures();
//	}

	//TODO continue


	//ensure that render target is unbound so that it's not used on accident by following stages
	RenderTarget::renderToScreen();
	//TODO maybe clear srceen? should actualle be done by dimulator at the beginning...

	//--------------------------------------------------
	//TEST: just render the result from previous stage as texture show to test the render target stuff

	//enable the texture show shader with the repsective texture bound
	Texture* renderingOfDefaultLightingStage =
		dynamic_cast<Texture*>(
			URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)
			->getStage("DefaultLightingStage")->getRenderingResult(FINAL_RENDERING_SEMANTICS)
		);
	assert("ParticleLiquidDrawStage::stepSimulation(): The DefaultLightingStage "
			"must expose a Texture with final rendering semantics! " && renderingOfDefaultLightingStage);
	//ShaderManager::getInstance().getTextureShowShader()->use(renderingOfDefaultLightingStage);

	mTextureShowShader->use(renderingOfDefaultLightingStage);

	//draw fullscreenquad
	WindowManager::getInstance().drawFullScreenQuad();

	//-------------------

	return true;

}

bool ParticleLiquidDrawStage::initStage()throw(SimulatorException)
{
	mTextureShowShader= new TextureShowShader();
	mTextureShowShader->build();

//	mRenderToScreen = ConfigCaster::cast<bool>(mSimConfigNode->get("renderToScreen",0));
//
//	//cause shader generation now:
//	ShaderManager::getInstance().setRenderingScenario(this);
//
//	mUsedRenderTarget= new RenderTarget(
//		String("ParticleLiquidDrawStageRenderTarget"),
//		WindowManager::getInstance().getWindowResolution(),
//		TEXTURE_TYPE_2D,
//		DEPTH_RENDER_BUFFER,
//		BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
//		1,
//		1
//	);
//
//	mUsedRenderTarget->requestCreateAndStoreColorTexture(FINAL_RENDERING_SEMANTICS);

	return true;

}

bool ParticleLiquidDrawStage::validateStage()throw(SimulatorException)
{
	//TODO
	return true;
}

bool ParticleLiquidDrawStage::checkCompatibility(VisualMaterial* visMat)
{
	//let only particle based liquids pass
	return dynamic_cast<ParticleLiquidVisualMaterial*>(visMat) != 0;
}

}
