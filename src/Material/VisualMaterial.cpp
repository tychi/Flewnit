/*
 * VisualMaterial
 *
 *  Created on: Dec 22, 2010
 *      Author: tychi
 */

#include "VisualMaterial.h"
#include "MPP/Shader/ShaderManager.h"
#include "Buffer/Texture.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

VisualMaterial::VisualMaterial(
			String name,
			VisualMaterialType type,
			//ShaderFeatures shaderFeatures,
			ShadingFeatures shadingFeatures,
			//must contain at least the textures used in the shader as samplers
			const Map<BufferSemantics, Texture*>& textures,
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

//void VisualMaterial::activate(
//			SimulationPipelineStage* currentStage,
//			SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//
//}
//
//void VisualMaterial::deactivate()
//{
//
//}
//
//
////called by ShaderManager when a new shader becomes necessary as the rendering scenario has changed;
////calls validate()
//void VisualMaterial::setShader(Shader* shader)
//{
//
//}
//
//
////calls "validate"
//void VisualMaterial::setTexture(Texture* tex)
//{
//
//}
//
////returns NULL if doesn't exist;
//Texture* VisualMaterial::getTexture(BufferSemantics bs)const
//{
//
//}
//
//
////assert that every material- (and hence shader-)used texture is available and that it is of the correct type;
////called by constructor
//void VisualMaterial::validateTextures()throw(SimulatorException)
//{
//
//}
////called by setShader();
//void VisualMaterial::validateShader()throw(SimulatorException)
//{
//
//}
////-----------------------------------------------------------------
//
//
//SkyDomeMaterial::SkyDomeMaterial(String name, Texture2DCube* cubeTex)
//{
//
//}
//
//SkyDomeMaterial::~SkyDomeMaterial()
//{
//
//}
//
////check for equality in order to check if a material with the desired properties
////(shader feature set and textures) already exists in the ResourceManager;
//bool SkyDomeMaterial::operator==(const Material& rhs) const
//{
//
//}
//
//void SkyDomeMaterial::activate(
//			SimulationPipelineStage* currentStage,
//			SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//
//}
//
//void SkyDomeMaterial::deactivate()
//{
//
//}


}
