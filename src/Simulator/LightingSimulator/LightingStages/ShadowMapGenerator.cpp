/*
 * ShadowMapGenerator.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "ShadowMapGenerator.h"

#include "MPP/Shader/ShaderManager.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"
#include "Buffer/BufferHelperUtils.h"
#include "Buffer/Texture.h"
#include "Util/Log/Log.h"
#include "Util/Loader/LoaderHelper.h"
#include "Simulator/SimulationResourceManager.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Scene/Scene.h"
#include "Simulator/OpenCL_Manager.h"

namespace Flewnit
{

ShadowMapGenerator::ShadowMapGenerator(ConfigStructNode* simConfigNode)
: LightingSimStageBase("ShadowMapGenerator",
		RENDERING_TECHNIQUE_SHADOWMAP_GENERATION,
		//mask every material not castnig shadows:
		VisualMaterialFlags(true,false,false,false,false),
		simConfigNode),
	mShadowMapResolution(0) //set in init function
{

	mShadowMapResolution =  ConfigCaster::cast<int>( simConfigNode->get("shadowMapResolution",0));
	assert( (mShadowMapResolution >0) && (BufferHelper::isPowerOfTwo(mShadowMapResolution)));
}

ShadowMapGenerator::~ShadowMapGenerator()
{
	//this stage owns this target, so it will destroy it
	delete mUsedRenderTarget;
}


void ShadowMapGenerator::initStage()throw(SimulatorException)
{
	assert("need shadow map generation stage only if shadowing is desired from the lighting stages :@"
			&& (ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
					!= LIGHT_SOURCES_SHADOW_FEATURE_NONE));


	TextureType texType = TEXTURE_TYPE_2D;
	switch(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature)
	{
	case LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT:
		texType = TEXTURE_TYPE_2D;
		break;
	case LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT:
		texType = TEXTURE_TYPE_2D_CUBE;
		break;
	case LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS:
		texType = TEXTURE_TYPE_2D_ARRAY;
		//keep ini shadowcaster value
		break;
	default: break; //omit waning that the NONE value wasn't checked
	}

	mUsedRenderTarget= new RenderTarget(
		String("ShadowMapGeneratorRenderTarget"),
		Vector2Dui(mShadowMapResolution,mShadowMapResolution),
		texType,
		DEPTH_TEXTURE,
		BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
		1,
		ShaderManager::getInstance().getGlobalShaderFeatures().numMaxShadowCasters
	);


	//make shadowmap(s) accessible from other stages
	assert("depth tex for SM gen has been created by render target" && mUsedRenderTarget->getStoredDepthTexture());
	mRenderingResults[SHADOW_MAP_SEMANTICS] = mUsedRenderTarget->getStoredDepthTexture();

	mUsedRenderTarget->bind();
	assert(mUsedRenderTarget->hasDepthAttachment());
	//disable fragment shader:
	mUsedRenderTarget->setEnableColorRendering(false);
	mUsedRenderTarget->setEnableDepthTest(true);
	RenderTarget::renderToScreen();

	//make even the depth generation shader material-associated;
	//like this, there is no special treatment of "global shaders" in the logic,
	//and some special materials like those of voxel fluids can contribute with their own shaders
	//generated for them;
	//initiate generation of the "shadow map generation shader collection" now, so that the first frame of the
	//app has no interruption due to late shader generation:
	ShaderManager::getInstance().setRenderingScenario(this);



}

void ShadowMapGenerator::validateStage()throw(SimulatorException)
{
	//no dependencies;
}




bool ShadowMapGenerator::stepSimulation() throw(SimulatorException)
{

	//LOG<<DEBUG_LOG_LEVEL<<"Shadowmap generator in action! ;(;\n";

	ShaderManager::getInstance().setRenderingScenario(this);

	GUARD(glViewport(0,0,mShadowMapResolution,mShadowMapResolution));
	GUARD(glEnable(GL_POLYGON_OFFSET_FILL));
	GUARD(glEnable(GL_POLYGON_OFFSET_LINE));
	GUARD(glEnable(GL_POLYGON_OFFSET_POINT));
	//GUARD(glPolygonOffset(-0.1f,-5.0f));
	//fill transform matrix buffer for shadow map gen, i.e. lookup parem set to false ;)
	LightSourceManager::getInstance().setupShadowCamMatricesUniformBuffer(false);

	mUsedRenderTarget->bind();

	RenderTarget::setEnableDepthTest(true);

	mUsedRenderTarget->clear();

	//--------------------------------------------------------
	//maybe superflous repeated setup, checkout removal after SM works stable
	mUsedRenderTarget->setEnableColorRendering(false);
	mUsedRenderTarget->clear();
	//-------------------------------------------------------

	//traverse the scene; like the DefaultLightingStage, we can re-use the
	//void LightingSimStageBase::visitSceneNode(SceneNode* node) routine, as the
	//masking of the VisualMAterialType flags is still sufficient
	SimulationResourceManager::getInstance().getScene()->traverse(this);

	SimulationResourceManager::getInstance().executeInstancedRendering(this);


	RenderTarget::renderToScreen();


	GUARD(glDisable(GL_POLYGON_OFFSET_FILL));
	GUARD(glDisable(GL_POLYGON_OFFSET_LINE));
	GUARD(glDisable(GL_POLYGON_OFFSET_POINT));

	return true;
}




}

//supposedly unneeded legace code:

//Texture* mShadowMapDepthTexture; //<-- wil lbe integrated as member of the rendertarget;
//Shader* mShadowMapGenerationShader;

//	mShadowMapGenerationShader = new DepthImageGenerationShader(
//			ShaderManager::getInstance().getShaderCodeDirectory(),
//			RENDERING_TECHNIQUE_SHADOWMAP_GENERATION,
//			mUsedRenderTarget->getTextureType,
//			false TODO HOWTO HANDLE GLOBAL SHADERS FOR INSTANCING
//	);
