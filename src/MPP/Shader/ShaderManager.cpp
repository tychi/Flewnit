/*
 * ShaderManager.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "ShaderManager.h"
#include "Util/Log/Log.h"


#include "Shader.h"
#include "GenericLightingUberShader.h"
#include "SkyDomeShader.h"
#include "MPP/Shader/LiquidShader.h"
#include "DepthImageGenerationShader.h"

#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"

#include <boost/foreach.hpp>


namespace Flewnit {

ShaderManager::ShaderManager(
		const ShaderFeaturesGlobal& globalShaderFeatures,
		Path shaderCodeDirectory )
:
		mShaderCodeDirectory(shaderCodeDirectory),
		mGlobalShaderFeatures(globalShaderFeatures),
		//following is actually useless, but i don't like uninitialized values ;(
		mCurrentRenderingTechnique(RENDERING_TECHNIQUE_DEFAULT_LIGHTING),
		mCurrentRenderTargetTextureType(TEXTURE_TYPE_2D),

		mIsInitializedGuard(false)
{
	mGlobalShaderFeatures.validate();


	// TODO Auto-generated constructor stub
//	ShaderFeaturesLocal s;
//	ShaderFeaturesLocal t(RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL);
//
//	boost::unordered_map<ShaderFeaturesLocal, String> testMap;
//
//	testMap[s]= "ASSNESS";
//	testMap[ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,TEXTURE_TYPE_2D_ARRAY)] = "YUMMYMAMMA";
//
//	LOG<<DEBUG_LOG_LEVEL<<"what comes now?"<<testMap[ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,TEXTURE_TYPE_2D_ARRAY)];
//	assert(testMap.find(t) == testMap.end());
//
//	mShaderMap[s]= 0;
}

ShaderManager::~ShaderManager()
{
	//do nothing, every object is "owned" by the sim resource manager
}

//iterates all visual materials and assigns them shaders fitting the current scenario;
//the shaders might need to be generated first (done automatically);
//the attachment status of the rendertaget is validated to fit the material's needs and the shader's
//binding status.
//throws exception if there is something incompatible;
//void setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt)throw(SimulatorException);
void ShaderManager::setRenderingScenario(LightingSimStageBase* lightingStage)throw(SimulatorException)
{
	mIsInitializedGuard = true;

	mCurrentRenderingTechnique = lightingStage->getRenderingTechnique();
	//we use tex2d as type if rendering to screen
	mCurrentRenderTargetTextureType = TEXTURE_TYPE_2D;
	RenderTarget* rt = lightingStage->getUsedRenderTarget();
	if(rt)
	{
		mCurrentRenderTargetTextureType = rt->getTextureType();
	}


	BOOST_FOREACH(VisualMaterial* mat, mRegisteredVisualMaterials)
	{
		assignShader(mat);
	}

}

void  ShaderManager::assignShader(VisualMaterial* mat)
{
	ShaderFeaturesLocal sfl = ShaderFeaturesLocal(
		mCurrentRenderingTechnique,
		mCurrentRenderTargetTextureType,
		mat->getType(),
		mat->getShadingFeatures(),
		mat->isInstanced()
	);

	if(mShaderMap.find(sfl) == mShaderMap.end())
	{
		mat->setShader(generateShader(sfl));
	}
	else
	{
		mat->setShader(mShaderMap[sfl]);
	}

}


Shader* ShaderManager::getShader(const ShaderFeaturesLocal& sfl)
{
	if(mShaderMap.find(sfl) == mShaderMap.end())
	{
		return mShaderMap[sfl];
	}
	else
	{
		return generateShader(sfl);
	}
}

Shader*  ShaderManager::generateShader(const ShaderFeaturesLocal& sfl)
{
	//assert(0 && "have to finish shader implementation first");

	//TODO continue implementation

	assert(mShaderMap.find(sfl) == mShaderMap.end());

	Shader* newShader = 0;

	if( ( sfl.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION ) ||
		( sfl.renderingTechnique == RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION ) ||
		( sfl.renderingTechnique == RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION	) )
	{
		//depth stuff
		newShader = new DepthImageGenerationShader(
				mShaderCodeDirectory,
				sfl.renderingTechnique,
				sfl.renderTargetTextureType,
				sfl.instancedRendering
		);
	}
	else
	{
		//color stuff
		switch(sfl.visualMaterialType)
		{
		case VISUAL_MATERIAL_TYPE_NONE:
			assert(0&&"invalid material flag");
			break;
		case VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING:
			newShader = new GenericLightingUberShader(mShaderCodeDirectory,sfl);
			break;
		case VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING:
			newShader = new SkyDomeShader(mShaderCodeDirectory, sfl.renderTargetTextureType);
			break;
		case VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY:
			assert(0&&"pure debug draw seems not necessary until now");
			break;
		case VISUAL_MATERIAL_TYPE_GAS_RENDERING:
			assert(0&&"gas rendering won't be implemented too soon");
			break;
		case VISUAL_MATERIAL_TYPE_LIQUID_RENDERING:
			//newShader = new LiquidShader (mShaderCodeDirectory,sfl);
			assert(0&&"liquid rendering comes later");
			break;
		default:
			assert(0&&" unknown visual material type");
			break;
		}
	}

	mShaderMap[sfl] = newShader;

	return newShader;

}



void ShaderManager::registerVisualMaterial(VisualMaterial* mat)
{
	mRegisteredVisualMaterials.push_back(mat);


	if(mIsInitializedGuard)
	{
		assignShader(mat);
	}
}

void ShaderManager::unregisterVisualMaterial(VisualMaterial* mat)
{
	//BOOST_FOREACH(VisualMaterial* m, mRegisteredVisualMaterials)
	for(unsigned int i=0; i< mRegisteredVisualMaterials.size(); i++)
	{
		if(mRegisteredVisualMaterials[i] == mat)
		{
			mRegisteredVisualMaterials.erase(mRegisteredVisualMaterials.begin() + i);
			return;
		}
	}
}

}
