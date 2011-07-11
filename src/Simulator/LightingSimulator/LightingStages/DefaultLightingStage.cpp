/*
 * DefaultLightingStage.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "DefaultLightingStage.h"

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



namespace Flewnit {

DefaultLightingStage::DefaultLightingStage(ConfigStructNode* simConfigNode)
: LightingSimStageBase(
		"DefaultLightingStage",
		RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
		//mask for shadable materials
		VisualMaterialFlags(false,false,true,false,false,false),
		simConfigNode)
{
	//TODO configure main render target etc..

}

DefaultLightingStage::~DefaultLightingStage()
{
	//this stage owns this target, so it will destroy it
	delete mUsedRenderTarget;
}

bool DefaultLightingStage::stepSimulation() throw(SimulatorException)
{
	//LOG<<DEBUG_LOG_LEVEL<<"DefaultLightingStage generator in action! ;(;\n";

	//Camera* cam = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

	URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera()
		->setGLViewPort(
				Vector2Di(0,0),
				Vector2Di(WindowManager::getInstance().getWindowResolution())
	);

	if(! mRenderToScreen)
	{
		//render to FBO
		mUsedRenderTarget->bind();
		RenderTarget::setEnableDepthTest(true);
		mUsedRenderTarget->attachStoredColorTexture(FINAL_RENDERING_SEMANTICS,0);
		mUsedRenderTarget->renderToAttachedTextures();
		mUsedRenderTarget->clear();
	}

	//assign lighting shaders to materials after shadowmap gen matrials might have been set by a preceding
	//SM gen stage
	ShaderManager::getInstance().setRenderingScenario(this);

	//update the uniform buffers of the light sources and the shadow map matrices
	LightSourceManager::getInstance().updateLightSourcesUniformBuffer();

	SimulationResourceManager::getInstance().getSceneGraph()->traverse(this);

	SimulationResourceManager::getInstance().executeInstancedRendering(this);

	//ensure that render target is unbound so that it's not used on accident by following stages
	RenderTarget::renderToScreen();

	return true;
}

bool DefaultLightingStage::initStage()throw(SimulatorException)
{
	mRenderToScreen = ConfigCaster::cast<bool>(mSimConfigNode->get("renderToScreen",0));

	//cause shader generation now:
	ShaderManager::getInstance().setRenderingScenario(this);

	mUsedRenderTarget= new RenderTarget(
		String("DefaultLightingStageRenderTarget"),
		WindowManager::getInstance().getWindowResolution(),
		TEXTURE_TYPE_2D,
		DEPTH_RENDER_BUFFER,
		BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
		1,
		1
	);

	mUsedRenderTarget->requestCreateAndStoreColorTexture(FINAL_RENDERING_SEMANTICS);

	return true;

}

bool DefaultLightingStage::validateStage()throw(SimulatorException)
{
	//shadowmap generator as dependency, but that comes later; TODO

	//TODO assert that no deferred stage exists;

	return true;
}

bool DefaultLightingStage::checkCompatibility(VisualMaterial* visMat)
{
	return
		visMat->getFlags().areCompatibleTo(getMaterialFlagMask())
		&&
		(
			//draw debug stuff only if desired
			( visMat->getType() != VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY )
			||
			URE_INSTANCE->doDebugDraw()
		);
}

}
