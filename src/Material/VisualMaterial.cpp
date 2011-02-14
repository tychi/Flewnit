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
#include "Simulator/LightingSimulator/LightingStages/ShadowMapGenerator.h"


namespace Flewnit
{

VisualMaterialFlags::VisualMaterialFlags(
		bool castsShadows,bool isTransparent,bool isShadable, bool isDynamicCubeMapRenderable,
		bool isInstanced,bool isCustomMaterial)
:		castsShadows(castsShadows),
 		isTransparent(isTransparent),
		isShadable(isShadable),
		isDynamicCubeMapRenderable(isDynamicCubeMapRenderable),
		isInstanced(isInstanced),
		isCustomMaterial(isCustomMaterial)
{}

VisualMaterialFlags::VisualMaterialFlags(const VisualMaterialFlags& rhs)
:		castsShadows(rhs.castsShadows),
 		isTransparent(rhs.isTransparent),
		isShadable(rhs.isShadable),
		isDynamicCubeMapRenderable(rhs.isDynamicCubeMapRenderable),
		isInstanced(rhs.isInstanced),
		isCustomMaterial(rhs.isCustomMaterial)
{}

//checker to compare a meterial's flags with those of a lighting stage in order to check if they are compatible
bool VisualMaterialFlags::areCompatibleTo(const VisualMaterialFlags& rhs)const
{
	if(rhs.castsShadows && (! castsShadows )) return false;
	if(rhs.isTransparent && (! isTransparent )) return false;
	if(rhs.isShadable && (! isShadable )) return false;
	if(rhs.isDynamicCubeMapRenderable && (! isDynamicCubeMapRenderable )) return false;

	//Instancing is not tested, as it should not play any role, the shader generation
	//and the geometry implementation should completely hide the special treatment of instanced geometry...


	//mask any kind of custom material, is handled by another control flow of special lighting stages;
	if(rhs.isCustomMaterial || isCustomMaterial ) return false;

	return true;
}

bool VisualMaterialFlags::operator==(const VisualMaterialFlags& rhs) const
{
	return
		castsShadows ==rhs.castsShadows &&
		isTransparent ==rhs.isTransparent &&
		isShadable  ==rhs.isShadable &&
		isDynamicCubeMapRenderable ==rhs.isDynamicCubeMapRenderable &&
		isInstanced ==rhs.isInstanced &&
		isCustomMaterial  ==rhs.isCustomMaterial
		;
}


VisualMaterial::VisualMaterial(
			String name,
			VisualMaterialType type,
			//ShaderFeatures shaderFeatures,
			ShadingFeatures shadingFeatures,
			//must contain at least the textures used in the shader as samplers
			const std::map<BufferSemantics, Texture*>& textures,
			const VisualMaterialFlags& visualMaterialFlags
			)
: 	Material(name, VISUAL_SIM_DOMAIN),
	mType(type),
	mShadingFeatures(shadingFeatures),
	mVisMatFlags(visualMaterialFlags),

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
				mVisMatFlags == castedMat->getFlags()
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
	dynamic_cast<ShadowMapGenerator*>(currentStage);

	LightingSimStageBase* castedStage= reinterpret_cast<LightingSimStageBase*>(currentStage);
	assert(castedStage->getRenderingTechnique() != RENDERING_TECHNIQUE_DEFERRED_LIGHTING);
	if( castedStage->getRenderingTechnique() == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION )
		{assert(castsShadows());}
	if( castedStage->getRenderingTechnique() == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL )
		{assert(! isTransparent());}

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

	if(mType == VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY)
	{
		return;
	}


	throw(SimulatorException("no other visual material type supported yet"));

	//tex->getTextureType()
}
//called by setShader();
void VisualMaterial::validateShader()throw(SimulatorException)
{
	assert( (mCurrentlyUsedShader->getLocalShaderFeatures().instancedRendering )
			== isInstanced() );

	//TODO further validdation when needed
}


}
