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

struct VisualMaterialFlags
{
	VisualMaterialFlags(
			bool castsShadows = true,
			bool isTransparent = false,
			bool isShadable = true,
			bool isDynamicCubeMapRenderable = true,
			bool isInstanced=false,
			bool isCustomMaterial=false);
	VisualMaterialFlags(const VisualMaterialFlags& rhs);
	virtual ~VisualMaterialFlags(){}
	bool operator==(const VisualMaterialFlags& rhs) const;


	//checker to compare a meterial's flags with those of a lighting stage in order to check if they are compatible
	bool areCompatibleTo(const VisualMaterialFlags& rhs)const;

	//some stuff shall not cast shadows, like the skybox or
	//lightsource/camera visualization geometry
	bool castsShadows;
	bool isTransparent;
	//value to mask some dummy geometry, e.g.
	//-a low-detail model only used for shadowmap generation,
	//-the skybox cube which shall not be lit
	//the uniform grid structure, which shall only be rendered for debug purposes
	bool isShadable;
	//mask out th to-be-cube mapped geometry itself (like a car chassis),
	//as otherwise it would occlude everything
	bool isDynamicCubeMapRenderable;
	bool isInstanced;
	//flag to indicate that this material does not fit the default rendering
	//structure, i.e. the default shaders etc.
	//in default _shading_ rendering stages, gemometry associated to such a custom material
	//will be completely ignored; Anyway, if the castsShadows flag is set,
	//it will be issued for shadowmap generation;
	bool isCustomMaterial;
};



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
			const std::map<BufferSemantics, Texture*>& textures,
			const VisualMaterialFlags& visualMaterialFlags
//			float shininess = 100.0f,
//			float reflectivity = 0.25f
			);

	virtual ~VisualMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);

	//looks within the ResourceManager for a shader with the same feature set as this
	//Material; if found, mShader is set to this found shader and its reference count;
	//Otherwise, a new shader is generated;
	//virtual void generateOrGrabShader()=0;

	inline VisualMaterialType getType()const{return mType;}
	//inline const ShaderFeatures& getShaderFeatures()const{return mShaderFeatures;}
	inline ShadingFeatures getShadingFeatures()const{return mShadingFeatures;}

	inline  const VisualMaterialFlags& getFlags()const{ return mVisMatFlags;}
	inline bool castsShadows()const{return mVisMatFlags.castsShadows;}
	inline bool isTransparent()const{return mVisMatFlags.isTransparent;}
	inline bool isShadable()const{return mVisMatFlags.isShadable;}
	inline bool isDynamicCubeMapRenderable()const{return mVisMatFlags.isDynamicCubeMapRenderable;}
	inline bool isInstanced()const{return mVisMatFlags.isInstanced;}
	inline bool isCustomMaterial()const{return mVisMatFlags.isCustomMaterial;}


	//calls validateTextures()
	void setTexture(Texture* tex);
	//returns NULL if doesn't exist;
	Texture* getTexture(BufferSemantics bs)const;

private:

	friend class ShaderManager;
	//called by ShaderManager when a new shader becomes necessary as the rendering scenario has changed;
	//calls validateShader()
	void setShader(Shader* shader);

	//assert that every material- (and hence shader-)used texture is available and that it is of the correct type;
	//called by constructor
	void validateTextures()throw(SimulatorException);
	//called by setShader();
	void validateShader()throw(SimulatorException);

	VisualMaterialType mType;
	ShadingFeatures mShadingFeatures;

	VisualMaterialFlags mVisMatFlags;

	//ShaderFeatures mShaderFeatures;
	Shader* mCurrentlyUsedShader;

	typedef std::map<BufferSemantics, Texture*> SemanticsToTextureMap;
	SemanticsToTextureMap mTextures;

	//friend class ShaderManager;
	//friend void ShaderManager::setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt);

};


/*
 * A dummy visual material; Exists only to to nothing durcing (de)activation;
 * This desgn decision was done in order to ensure a minimum overhead due to
 * special case treatment  No shader activation or anything else;
 *
 * Note: the actual drawing of instanced geometry happens at the end of a lighting simulation stage,
 * when all to-be-drawn instances have been registered; Then, the LightingSimulationStage
 * calls SimulationResourceManager::executeInstancedRendering(); This routine will iterate over
 * it Instancemanagers, and those will issue an instanced activation/GL-draw call to their
 * "real" Material/Geometry; The "real" material has its isInstanced-flag set and an associated
 * compatible shader;
 *
 * */
//class InstancedVisualMaterial
//: public VisualMaterial
//{
//	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
//
//	friend class InstanceManager;
//	InstancedVisualMaterial(String name, VisualMaterialFlags visFlags);
//public:
//	virtual ~InstancedVisualMaterial();
//	virtual bool operator==(const Material& rhs) const;
//	virtual void activate(
//			SimulationPipelineStage* currentStage,
//			SubObject* currentUsingSuboject) throw(SimulatorException);
//	virtual void deactivate(SimulationPipelineStage* currentStage,
//			SubObject* currentUsingSuboject) throw(SimulatorException);
//};



class SkyDomeMaterial
: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	SkyDomeMaterial(String name, Texture2DCube* cubeTex);
	virtual ~SkyDomeMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
};



//we'll see if we need an actual debug draw material or if it is handled by the visual mat..



class LiquidVisualMaterial
: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	//will be extended when needed
	LiquidVisualMaterial(String name);
	virtual ~LiquidVisualMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
};


}
