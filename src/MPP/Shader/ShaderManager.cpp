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
		mCurrentRenderTargetTextureType(TEXTURE_TYPE_2D)

		//mIsInitializedGuard(false)
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

bool ShaderManager::currentRenderingScenarioPerformsLayeredRendering()const
{
	return
		//dyn. envmap rendering?
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_CUBE) ||
		//pointlight shadow map gen?
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_CUBE_DEPTH) ||
		//multiple spotlight shadow map gen?
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_ARRAY_DEPTH)
		;
}

//returns true if doing layered rendering or other stuff involving multiple view/projection cameras
//which would currupt view space transformed data;
bool ShaderManager::vertexShaderNeedsWorldSpaceTransform()const
{
	return currentRenderingScenarioPerformsLayeredRendering();
}



bool ShaderManager::currentRenderingScenarioNeedsGeometryShader()const
{
	//when do we need a geometry shader?
	//if we need to render to a cubemap, an array texture or if wee need to render primitive IDs
	return
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_CUBE) ||
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_CUBE_DEPTH) ||
		(mCurrentRenderTargetTextureType == TEXTURE_TYPE_2D_ARRAY_DEPTH) ||
		(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION )
		;
}

//iterates all visual materials and assigns them shaders fitting the current scenario;
//the shaders might need to be generated first (done automatically);
//the attachment status of the rendertaget is validated to fit the material's needs and the shader's
//binding status.
//throws exception if there is something incompatible;
//void setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt)throw(SimulatorException);
void ShaderManager::setRenderingScenario(LightingSimStageBase* lightingStage)throw(SimulatorException)
{
	//mIsInitializedGuard = true;

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
	if(mShaderMap.find(sfl) != mShaderMap.end())
	{
		return mShaderMap[sfl];
	}
	else
	{
		return generateShader(sfl);
	}
}

/*
 *	Shall return a shader where offsets of members in
 *	lightsource buffer and shadowmap matrix buffer,
 *	if the global shader features indicate their possible usage;
 *
 *	note: an instance transform uniform block is not necessary, as every instance manager
 *	has its "own" shader to query from; only the other two uniform blocks have a "global" character
 */
Shader* ShaderManager::getUniformBufferOffsetQueryShader(bool forShadowMapGeneration)
{
	//shaderfeatures to request from the shadermanager a minimalistic shader containing the
	//uniform buffer declaration, so that offsets can be queried
	static ShaderFeaturesLocal sfl =
		ShaderFeaturesLocal(
				RENDERING_TECHNIQUE_DEFAULT_LIGHTING, //lighting, for the creation of the lightsource uniform block
				TEXTURE_TYPE_2D_RECT, //play no role, dummy..
				VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING, //lighting, for the creation of the lightsource uniform block
				ShadingFeatures(
						SHADING_FEATURE_DIRECT_LIGHTING //lighting, for the creation of the lightsource uniform block
						//TODO add shadowing when the rest is stable
				),
				false //no global instance buffer query needed
	);

	if(forShadowMapGeneration)
	{
		//shadow map generation,layout(shared) uniform ShadowCameraTransformBuffer block
		//note the ShadowCameraTransformBuffer is used twice:
		//	- for generation of shadowmaps in a geometry shader
		//	- for shadowmap lookup in a fragment shader
		//though the same buffer object is used, the logical uniform block in the shaders is different;
		//to omit bugs as far as possible, two meta infos for the same buffer will be maintained
		//queried;
		return getShader(
			ShaderFeaturesLocal(
				RENDERING_TECHNIQUE_SHADOWMAP_GENERATION,
				TEXTURE_TYPE_2D_RECT, //play no role, dummy..
				VISUAL_MATERIAL_TYPE_NONE,
				ShadingFeatures(SHADING_FEATURE_NONE),
				false //no global instance buffer query needed
			)
		);
	}
	else
	{
		//lighting, for the creation of the "layout(shared) uniform LightSourceBuffer"  block
		return getShader(
			ShaderFeaturesLocal(
				RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
				TEXTURE_TYPE_2D_RECT, //play no role, dummy..
				VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
				ShadingFeatures(SHADING_FEATURE_DIRECT_LIGHTING),
				false //no global instance buffer query needed
			)
		);
	}

}



Shader*  ShaderManager::generateShader(const ShaderFeaturesLocal& sfl)
{
	//TODO continue implementation

	assert( ( mShaderMap.find(sfl) == mShaderMap.end() ) );

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
			newShader = new GenericLightingUberShader(mShaderCodeDirectory,sfl);
			//assert(0&&"pure debug draw seems not necessary until now");
			break;
		case VISUAL_MATERIAL_TYPE_GAS_RENDERING:
			assert(0&&"gas rendering won't be implemented too soon");
			break;
		case VISUAL_MATERIAL_TYPE_LIQUID_RENDERING:
			//newShader = new LiquidShader (mShaderCodeDirectory,sfl);
			assert(0&&"liquid rendering comes later");
			break;
//		case VISUAL_MATERIAL_TYPE_INSTANCED:
//			//create nothing, as an instancedMaterial needs no shader;
//			return 0;
//			break;
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


	//if(mIsInitializedGuard)
	//{
		assignShader(mat);
	//}
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
