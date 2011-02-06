/*
 * VisualMaterial
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *
 * 	Base class for all Materials in the visual domain.
 */

#pragma once

#include "Material.h"

#include "Simulator/SimulatorMetaInfo.h"

namespace Flewnit
{

/*
 * Base class of all Materials in the visual domain; In classic 3D-Rendering-Engine
 * terms, it is the "Material" ;).
 */
class VisualMaterial
: public Material
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	VisualMaterial(
			String name,
			VisualMaterialType type,
			//ShaderFeatures shaderFeatures,
			ShadingFeatures shadingFeatures,
			//must contain at least the textures used in the shader as samplers
			const Map<BufferSemantics, Texture*>& textures,
			//some stuff shall not cast shadows, like the skybox or
			//lightsource/camera visualization geometry
			bool castsShadows = true,
			bool isTransparent = false,
			//mask out th to-be-cube mapped geometry itself (like a car chassis),
			//as otherwise it would occlude everything
			bool isDynamicCubeMapRenderable = true,
			//value to mask some dummy geometry, e.g.
			//-a low-detail model only used for shadowmap generation,
			//-the skybox cube which shall not be lit
			//the uniform grid structure, which shall only be rendered for debug purposes
			bool isShadable = true,
			bool isInstanced = false,
			//flag to indicate that this material does not fit the default rendering
			//structure, i.e. the default shaders etc.
			//in default _shading_ rendering stages, gemometry associated to such a custom material
			//will be completely ignored; Anyway, if the castsShadows flag is set,
			//it will be issued for shadowmap generation;
			bool isCustomMaterial =false
			);

	virtual ~VisualMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const = 0;


	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate();

	//called by ShaderManager when a new shader becomes necessary as the rendering scenario has changed;
	//can be overriden by special materials to react appropriat (throw exception, for example ;))
	virtual void setShader(Shader* shader)throw(SimulatorException);

	//looks within the ResourceManager for a shader with the same feature set as this
	//Material; if found, mShader is set to this found shader and its reference count;
	//Otherwise, a new shader is generated;
	//virtual void generateOrGrabShader()=0;

	inline bool castsShadows()const{return mCastsShadows;}
	inline bool isShadable()const{return mIsShadable;}
	inline bool isDynamicCubeMapRenderable()const{return mIsDynamicCubeMapRenderable;}
	inline bool isInstanced()const{return mIsInstanced;}
	inline bool isCustomMaterial()const{return mIsCustomMaterial;}

	inline VisualMaterialType getType()const{return mType;}
	//inline const ShaderFeatures& getShaderFeatures()const{return mShaderFeatures;}
	ShadingFeatures getShadingFeatures()const{return mShadingFeatures;}

	//calls "validate"
	void setTexture(Texture* tex);
	Texture* getTexture(BufferSemantics bs)const;

private:
	//assert that every shader-used texture is available and that it is of the correct type;
	//called by constructor
	void validate();

	bool mCastsShadows;
	bool mIsShadable;
	bool mIsDynamicCubeMapRenderable;
	bool mIsCustomMaterial;


	VisualMaterialType mType;

	ShadingFeatures mShadingFeatures;
	bool mIsInstanced;

	//ShaderFeatures mShaderFeatures;
	Shader* mCurrentlyUsedShader;

	Map<BufferSemantics, Texture*> mTextures;

	//friend class ShaderManager;
	//friend void ShaderManager::setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt);

};

class SkyDomeMaterial
{

};


}
