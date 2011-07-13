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
#include "UserInterface/WindowManager/WindowManager.h"
#include "Buffer/Texture.h"
#include "TextureShowShader.h"


namespace Flewnit {

ShaderManager::ShaderManager(
		const ShaderFeaturesGlobal& globalShaderFeatures,
		ShadingFeatures supportedShadingFeatures,
		Path shaderCodeDirectory )
:
		mShaderCodeDirectory(shaderCodeDirectory),
		mGlobalShaderFeatures(globalShaderFeatures),
		//following is actually useless, but i don't like uninitialized values ;(
		mCurrentRenderingTechnique(RENDERING_TECHNIQUE_DEFAULT_LIGHTING),
		mCurrentRenderTargetTextureType(TEXTURE_TYPE_2D),
		mEnabledShadingFeatures(
			supportedShadingFeatures
//			ShadingFeatures(
//				//SHADING_FEATURE_NONE
//				  SHADING_FEATURE_DIRECT_LIGHTING
//				| SHADING_FEATURE_DIFFUSE_TEXTURING
//				| SHADING_FEATURE_NORMAL_MAPPING
//				| SHADING_FEATURE_CUBE_MAPPING
//				| ( 	(WindowManager::getInstance().getAvailableOpenGLVersion().x >=4)
//						? SHADING_FEATURE_TESSELATION:0)
//			)
		)
		//mTextureShowShader(new TextureShowShader())
		//mTesselationIsEnabled(WindowManager::getInstance().getAvailableOpenGLVersion().x >=4)
		//mIsInitializedGuard(false)
{
	mGlobalShaderFeatures.validate();

	//mTextureShowShader->build();


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
bool ShaderManager::shaderNeedsWorldSpaceTransform()const
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

bool ShaderManager::currentRenderingScenarioNeedsFragmentShader()const
{
	return
		!	(
			    (mCurrentRenderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
			    &&
			    (
			    	(mGlobalShaderFeatures.lightSourcesShadowFeature ==
			    		LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
			    	||
			    	(mGlobalShaderFeatures.lightSourcesShadowFeature ==
			    	   	LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)

			//    	(mLocalShaderFeatures.renderTargetTextureType== TEXTURE_TYPE_2D_DEPTH)
			//    	||
			//    	(mLocalShaderFeatures.renderTargetTextureType== TEXTURE_TYPE_2D_RECT_DEPTH)
			//    	||
			//    	(mLocalShaderFeatures.renderTargetTextureType== TEXTURE_TYPE_2D_ARRAY)
			    )
			);
}


void ShaderManager::setEnableShadingFeatures(ShadingFeatures sfs, bool val)
{
	if(val)
	{
		reinterpret_cast<unsigned int&>(mEnabledShadingFeatures) |= (sfs);
	}
	else
	{
		reinterpret_cast<unsigned int&>(mEnabledShadingFeatures) &= (~sfs);
	}

	if( WindowManager::getInstance().getAvailableOpenGLVersion().x < 4 )
	{
		//mask out tess if not possible for technical reasons
		reinterpret_cast<unsigned int&>(mEnabledShadingFeatures) &= (~SHADING_FEATURE_TESSELATION);
	}


	BOOST_FOREACH(VisualMaterial* mat, mRegisteredVisualMaterials)
	{
		//only play around with non-special shader
		if(mat->getType() == VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING)
		{
			assignShader(mat);
		}
	}

}

bool ShaderManager::shadingFeaturesAreEnabled(ShadingFeatures sfs)const
{
	return (mEnabledShadingFeatures & sfs) != 0;
}


bool ShaderManager::tesselationIsEnabled()const
{
	return ( WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4 )
			&& (( mEnabledShadingFeatures & SHADING_FEATURE_TESSELATION ) != 0);
}



//void ShaderManager::setEnableTesselation(bool val)
//{
//	if(mTesselationIsEnabled == val) return;
//	if((WindowManager::getInstance().getAvailableOpenGLVersion().x < 4)) return; //technically impossible, do nothing;
//
//	mTesselationIsEnabled = val;
//
//	BOOST_FOREACH(VisualMaterial* mat, mRegisteredVisualMaterials)
//	{
//		if(
//			( (mat->getShadingFeatures() & SHADING_FEATURE_TESSELATION ) !=0 )
//			//checked this case already above
//			// &&(WindowManager::getInstance().getAvailableOpenGLVersion().x >=4)
//		)
//		{
//			assignShader(mat);
//		}
//	}
//}



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
		if(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
		{
			Texture* shadowMapTex = dynamic_cast<Texture*>(
					lightingStage->getRenderingResult(SHADOW_MAP_SEMANTICS));
			assert(shadowMapTex);

			//mat->setTexture(SHADOW_MAP_SEMANTICS, rt->getStoredDepthTexture());
			mat->setTexture(SHADOW_MAP_SEMANTICS, shadowMapTex);

			if ( mat->getFlags().castsShadows )
			{
				assignShader(mat);
			}
		}
		else
		{

			if ( mat->getFlags().areCompatibleTo(lightingStage->getMaterialFlagMask()))
			{
				assignShader(mat);
			}
//			else {
//				//will be masked anyway, set shader to 0 as there is nothing useful to generate
//				mat->setShader(0);
//			}
		}
	}

}

void  ShaderManager::assignShader(VisualMaterial* mat)
{
	ShadingFeatures shadingFeaturesToGenerate =
			ShadingFeatures( mat->getShadingFeatures() & (mEnabledShadingFeatures) );
	if(
		//(WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4 )
		//&&
		(	(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
			||	(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION)
			||	(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION)
			||	(mCurrentRenderingTechnique == RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION)
		)
	)
	{
		//mask all shading features of those "none-lighting-techniques" but tessellation
		reinterpret_cast<unsigned int&>(shadingFeaturesToGenerate) &= ShadingFeatures(
			SHADING_FEATURE_NONE | SHADING_FEATURE_TESSELATION
		);
	}

	if(mat->getType() == VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING)
	{
		//for skydome, cube mapping must always be enabled, no matter what the gloable state says ;(
		reinterpret_cast<unsigned int&>(shadingFeaturesToGenerate) |=
			SHADING_FEATURE_CUBE_MAPPING;
	}

	ShaderFeaturesLocal sfl = ShaderFeaturesLocal(
		mCurrentRenderingTechnique,
		mCurrentRenderTargetTextureType,
		mat->getType(),
		//permit only the enabled features!
		shadingFeaturesToGenerate,
		mat->isInstanced()
	);


//	if( (!tesselationIsEnabled()) )
//	{
//		//mask out tesselation feature!
//		reinterpret_cast<unsigned int&>(sfl.shadingFeatures) &= (~SHADING_FEATURE_TESSELATION);
//	}

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
 *	lightsource uniform buffer and shadowmap matrix uniform buffer can be queried,
 *	if the global shader features indicate their possible usage;
 *
 *	note: an instance transform uniform block is not necessary, as every instance manager
 *	has its "own" shader to query from; only the other two uniform blocks have a "global" character
 */
Shader* ShaderManager::getUniformBufferOffsetQueryShader(bool forShadowMapGeneration)
{
	//shaderfeatures to request from the shadermanager a minimalistic shader containing the
	//uniform buffer declaration, so that offsets can be queried
//	static ShaderFeaturesLocal sfl =
//		ShaderFeaturesLocal(
//				RENDERING_TECHNIQUE_DEFAULT_LIGHTING, //lighting, for the creation of the lightsource uniform block
//				TEXTURE_TYPE_2D_RECT, //play no role, dummy..
//				VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING, //lighting, for the creation of the lightsource uniform block
//				ShadingFeatures(
//						SHADING_FEATURE_DIRECT_LIGHTING //lighting, for the creation of the lightsource uniform block
//						//TODO add shadowing when the rest is stable
//				),
//				false //no global instance buffer query needed
//	);

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
				sfl
//				sfl.renderingTechnique,
//				sfl.visualMaterialType,
//				sfl.renderTargetTextureType,
//				((sfl.shadingFeatures & SHADING_FEATURE_TESSELATION) !=0),
//				sfl.instancedRendering
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
			//newShader = new SkyDomeShader(mShaderCodeDirectory, sfl.renderTargetTextureType);
			newShader = new SkyDomeShader(mShaderCodeDirectory, sfl);
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
//			//no such material, obsolete
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
