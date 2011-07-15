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
#include "Simulator/MechanicsSimulator/ParticleMechanicsStages/ParticleMechanicsStage.h"
#include "Scene/ParticleSceneRepresentation.h"
#include "WorldObject/ParticleFluid.h"
#include "WorldObject/SubObject.h"
#include "Geometry/Geometry.h"

using Flewnit::SubObject;
using Flewnit::Geometry;




namespace Flewnit {

ParticleLiquidDrawStage::ParticleLiquidDrawStage(ConfigStructNode* simConfigNode)
: LightingSimStageBase(
		"ParticleLiquidDrawStage",
		RENDERING_TECHNIQUE_CUSTOM,
		//mask for custom stuff; isn'nt used anyway, because there is no scene graph traversal
		VisualMaterialFlags(false,false,false,false,false,true),
		simConfigNode),
		mTextureShowShader(0),
		mCompositedRendering(0)
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

	//enable the texture show shader with the respective texture bound
	RenderTarget* rt =
			dynamic_cast<LightingSimStageBase*>(
			URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)
						->getStage("DefaultLightingStage"))->getUsedRenderTarget();
	assert(rt);

	Texture* renderingOfDefaultLightingStage =
		dynamic_cast<Texture*>(
			URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)
			->getStage("DefaultLightingStage")->getRenderingResult(FINAL_RENDERING_SEMANTICS)
		);
	assert("The DefaultLightingStage must expose a Texture with final rendering semantics! "
			&& renderingOfDefaultLightingStage);

	//render to FBO
	rt->bind();
	rt->detachAllColorTextures(); //free from previous stage's relicts
	rt->attachColorTexture(mCompositedRendering,0); //attach own texture

	RenderTarget::setEnableDepthTest(true);
	rt->attachStoredDepthBuffer();
	rt->renderToAttachedTextures();


	//haxx: just copy the texture to have both an image to sample from
	//and to amend by direct fluid rendering
	mTextureShowShader->use(renderingOfDefaultLightingStage);
	WindowManager::getInstance().drawFullScreenQuad();

	RenderTarget::setEnableDepthTest(true);

	//now, render fluid onto the just copied texture;
	//again: this hack is needed to have a consistent depth buffer!
	int numCurrentFluids =
			dynamic_cast<ParticleMechanicsStage*>(
				URE_INSTANCE->getSimulator(MECHANICAL_SIM_DOMAIN)
				->getStage("ParticleMechanicsStage")
			)->getParticleSceneRepresentation()
			->getNumCurrentFluids();

	for(int i=0; i< numCurrentFluids; i++)
	{
		ParticleFluid* fluid =
			dynamic_cast<ParticleMechanicsStage*>(
				URE_INSTANCE->getSimulator(MECHANICAL_SIM_DOMAIN)
					->getStage("ParticleMechanicsStage")
				)->getParticleSceneRepresentation()
				->getFluid(i);

		assert( ! fluid->getSubObjects(VISUAL_SIM_DOMAIN).empty());
		SubObject* so = fluid->getSubObjects(VISUAL_SIM_DOMAIN)[0];

		ParticleLiquidVisualMaterial* mat =
			dynamic_cast<ParticleLiquidVisualMaterial*>(so->getMaterial());
		assert(mat);

		RenderTarget::setEnableDepthTest(true);
		mat->mCompositionShader->use(so);
		so->getGeometry()->draw();

		//draw fullscreenquad
		//WindowManager::getInstance().drawFullScreenQuad();
	}

	//now, just show the result rendered into the FBO
	//damn, so many useless copies, but time pressure is forcing me to do so...
	RenderTarget::renderToScreen();
	mTextureShowShader->use(mCompositedRendering);
	WindowManager::getInstance().drawFullScreenQuad();



	//-------------------

	return true;

}

bool ParticleLiquidDrawStage::initStage()throw(SimulatorException)
{
	mTextureShowShader= new TextureShowShader();
	mTextureShowShader->build();

	BufferElementInfo texeli(4,GPU_DATA_TYPE_UINT,8,true);
	mCompositedRendering = new Texture2D(
			"particleLiquidDrawStageCompositedRendering", FINAL_RENDERING_SEMANTICS,
			WindowManager::getInstance().getWindowResolution().x,
			WindowManager::getInstance().getWindowResolution().y,
			texeli,false,false,false,0,false
			);

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
