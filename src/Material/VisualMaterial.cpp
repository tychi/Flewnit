/*
 * VisualMaterial
 *
 *  Created on: Dec 22, 2010
 *      Author: tychi
 */

#include "VisualMaterial.h"
#include "MPP/Shader/ShaderManager.h"
#include "Buffer/Texture.h"
#include "MPP/Shader/Shader.h"
#include "Simulator/LightingSimulator/LightingSimStageBase.h"

#include <boost/foreach.hpp>
#include "Simulator/LightingSimulator/LightingStages/ShadowMapGenerationStage.h"


namespace Flewnit
{

VisualMaterial::VisualMaterial(
			String name,
			VisualMaterialType type,
			//ShaderFeatures shaderFeatures,
			ShadingFeatures shadingFeatures,
			//must contain at least the textures used in the shader as samplers
			const std::map<BufferSemantics, Texture*>& textures,
			//some stuff shall not cast shadows, like the skybox or
			//lightsource/camera visualization geometry
			bool castsShadows,
			bool isTransparent,
			//value to mask some dummy geometry, e.g.
			//-a low-detail model only used for shadowmap generation,
			//-the skybox cube which shall not be lit
			//the uniform grid structure, which shall only be rendered for debug purposes
			bool isShadable,
			//mask out th to-be-cube mapped geometry itself (like a car chassis),
			//as otherwise it would occlude everything
			bool isDynamicCubeMapRenderable,
			bool isInstanced,
			//flag to indicate that this material does not fit the default rendering
			//structure, i.e. the default shaders etc.
			//in default _shading_ rendering stages, gemometry associated to such a custom material
			//will be completely ignored; Anyway, if the castsShadows flag is set,
			//it will be issued for shadowmap generation;
			bool isCustomMaterial
			)
: 	Material(name, VISUAL_SIM_DOMAIN),
	mType(type),
	mShadingFeatures(shadingFeatures),

	mCastsShadows(castsShadows),
	mIsTransparent(isTransparent),
	mIsShadable(isShadable),
	mIsDynamicCubeMapRenderable(isDynamicCubeMapRenderable),
	mIsInstanced(isInstanced),
	mIsCustomMaterial(isCustomMaterial),

	mCurrentlyUsedShader(0)
{
	mTextures = textures;

	validateTextures();

	ShaderManager::getInstance().registerVisualMaterial(this);
}

VisualMaterial::~VisualMaterial()
{
	ShaderManager::getInstance().unregisterVisualMaterial(this);

	//nothing to delete, every used stuff is "owned by other manager/container classes
}

//check for equality in order to check if a material with the desired properties
//(shader feature set and textures) already exists in the ResourceManager;
bool VisualMaterial::operator==(const Material& rhs) const
{
	const VisualMaterial* castedMat = dynamic_cast<const VisualMaterial*>(&rhs);

	if(castedMat)
	{
		//DON't compare name, as the name plays no role for logical equality

		//first, compare the logical features:
		if(
				mType == castedMat->getType() &&
				mShadingFeatures == castedMat->getShadingFeatures() &&

				mCastsShadows == castedMat->castsShadows() &&
				mIsTransparent == castedMat->isTransparent() &&
				mIsShadable  == castedMat->isShadable() &&
				mIsDynamicCubeMapRenderable == castedMat->isDynamicCubeMapRenderable() &&
				mIsInstanced == castedMat->isInstanced() &&
				mIsCustomMaterial  == castedMat->isCustomMaterial()
		)
		{
			//compare textures; that the texture state is valid, has already been asserted,
			//hence we don't need to check for existance
			BOOST_FOREACH(SemanticsToTextureMap::value_type semTexPair, mTextures)
			{
				assert(castedMat->getTexture(semTexPair.first));
				if(
					!(
						*(castedMat->getTexture(semTexPair.first)) ==
						*(semTexPair.second)
					)
				)
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void VisualMaterial::activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//assert("is lighting stage" && dynamic_cast<LightingSimStageBase*>(currentStage));
	dynamic_cast<ShadowMapGenerationStage*>(currentStage);

	LightingSimStageBase* castedStage= reinterpret_cast<LightingSimStageBase*>(currentStage);
	assert(castedStage->getRenderingTechnique() != RENDERING_TECHNIQUE_DEFERRED_LIGHTING);
	if( castedStage->getRenderingTechnique() == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION )
		{assert(mCastsShadows);}
	if( castedStage->getRenderingTechnique() == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL )
		{assert(! mIsTransparent);}

	mCurrentlyUsedShader->use(currentUsingSuboject);
}

void VisualMaterial::deactivate(
		SimulationPipelineStage* currentStage,
		SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//do nothing in this base class;
}


//called by ShaderManager when a new shader becomes necessary as the rendering scenario has changed;
//calls validate()
void VisualMaterial::setShader(Shader* shader)
{
	mCurrentlyUsedShader = shader;
	validateShader();
}


//calls "validate"
void VisualMaterial::setTexture(Texture* tex)
{
	mTextures[tex->getBufferInfo().bufferSemantics] = tex;
	validateTextures();
}

//returns NULL if doesn't exist;
Texture* VisualMaterial::getTexture(BufferSemantics bs)const
{
	if(mTextures.find(bs)== mTextures.end())
	{
		return 0;
	}
	return mTextures.find(bs)->second;
	//return mTextures[bs];
}


//assert that every material- (and hence shader-)used texture is available and that it is of the correct type;
//called by constructor
void VisualMaterial::validateTextures()throw(SimulatorException)
{
	if(mType == VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING)
	{
		if( (mShadingFeatures &  SHADING_FEATURE_DECAL_TEXTURING) != 0)
		{
			assert(mTextures.find(DECAL_COLOR_SEMANTICS) != mTextures.end());
		}
		if( (mShadingFeatures &  SHADING_FEATURE_DETAIL_TEXTURING) != 0)
		{
			assert(mTextures.find(DETAIL_TEXTURE_SEMANTICS) != mTextures.end());
		}
		if( (mShadingFeatures &  SHADING_FEATURE_NORMAL_MAPPING) != 0)
		{
			assert(mTextures.find(NORMAL_SEMANTICS) != mTextures.end());
		}
		if( (mShadingFeatures &  SHADING_FEATURE_CUBE_MAPPING) != 0)
		{
			assert(mTextures.find(ENVMAP_SEMANTICS) != mTextures.end());
		}
		if( (mShadingFeatures & SHADING_FEATURE_AMBIENT_OCCLUSION ) != 0)
		{
			assert(mTextures.find(POSITION_SEMANTICS) != mTextures.end()
			        && "for ambient occlusion calc, at the moment, "
					"there is no other way then binding a position texture, "
					"as in the shader generation, the depth-optimazation is not included yet");
		}

		//shadow map check:
		if( (ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
				!= LIGHT_SOURCES_SHADOW_FEATURE_NONE)
			&&
			( (mShadingFeatures & SHADING_FEATURE_DIRECT_LIGHTING  ) != 0)
		)
		{
			assert(mTextures.find(SHADOW_MAP_SEMANTICS) != mTextures.end());
		}

		return;

	}

	if(mType == VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING)
	{
		assert(mTextures.find(ENVMAP_SEMANTICS) != mTextures.end());
		return;
	}


	throw(SimulatorException("no other visual material type supported yet"));

	//tex->getTextureType()
}
//called by setShader();
void VisualMaterial::validateShader()throw(SimulatorException)
{
	assert( (mCurrentlyUsedShader->getLocalShaderFeatures().instancedRendering )
			== mIsInstanced );

	//TODO further validdation when needed
}


}
