/*
 * ShaderManager.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "ShaderManager.h"
#include "Util/Log/Log.h"

#include <boost/foreach.hpp>

namespace Flewnit {

ShaderManager::ShaderManager(
		const ShaderFeaturesGlobal& globalShaderFeatures,
		Path shaderCodeDirectory )
:
		mShaderCodeDirectory(shaderCodeDirectory),
		mGlobalShaderFeatures(globalShaderFeatures),
		//following is actually useless, but i don't like uninitialized values ;(
		mRenderingTechnique(RENDERING_TECHNIQUE_DEFAULT_LIGHTING),
		mRenderTargetTextureType(TEXTURE_TYPE_2D),

		mIsInitializedGuard(false)
{
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
	//to nothing, every object is "owned" by the sim resource manager
}

//iterates all visual materials and assigns them shaders fitting the current scenario;
//the shaders might need to be generated first (done automatically);
//the attachment status of the rendertaget is validated to fit the material's needs and the shader's
//binding status.
//throws exception if there is something incompatible;
//void setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt)throw(SimulatorException);
void ShaderManager::setRenderingScenario(LightingSimStageBase* lightingStage)throw(SimulatorException)
{
	BOOST_FOREACH(VisualMaterial* mat, mRegisteredVisualMaterials)
	{

	}

}



void ShaderManager::registerVisualMaterial(VisualMaterial* mat)
{
	mRegisteredVisualMaterials.push_back(mat);


	if(mIsInitializedGuard)
	{
		ShaderFeaturesLocal sfl = ShaderFeaturesLocal(
			mRenderingTechnique,
			mRenderTargetTextureType,
			mat->getType(),
			mat->getShadingFeatures(),
			mat->isInstanced()
		);

		if(mShaderMap.find(sfl) == mShaderMap.end())
		{
			Shader* newShader = 0;
			switch(mat->getType())
			{
			case VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING:
				newShader = 0;
				break;
			case VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING:
				newShader = 0;
				break;
			case VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY:
				assert(0&&"pure debug draw seems not necessary until now");
				break;
			case VISUAL_MATERIAL_TYPE_GAS_RENDERING:
				assert(0&&"gas rendering won't be implemented too soon");
				break;
			case VISUAL_MATERIAL_TYPE_LIQUID_RENDERING:
				newShader = 0;
				break;
			}
			assert(newShader);
			mShaderMap[sfl] = newShader;
		}

		mat->setShader(mShaderMap[sfl]);
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
