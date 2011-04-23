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
	bool areCompatibleTo(const VisualMaterialFlags& lightingStageMask)const;

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
			const VisualMaterialFlags& visualMaterialFlags,
			float shininess = 100.0f,
			float reflectivity = 0.25f
			);

protected:
	//for DebugDrawMaterial
	VisualMaterial(	String name, const Vector4D& debugDrawColor);
public:

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


	//calls validateTextures(); Map semantics key of texture in the texture map will be
	//tex->getBufferInfo().bufferSemantics, i.e. implcitely set
	void setTexture(Texture* tex);
	//for ambigous usages like depth images, use this setter:
	//Map semantics key of texture in the texture map will be
	//explicitSemantics, no matter what the "actual" semantics of the texture is;
	//this way, the same texture storage can have different purposes in different stages;
	void setTexture(BufferSemantics explicitSemantics, Texture* tex);
	//returns NULL if doesn't exist;
	Texture* getTexture(BufferSemantics bs)const;

	inline float getShininess()const{return mShininess;}
	inline float getReflectivity()const{return mReflectivity;}
	inline Shader* getCurrentlyUsedShader()const{return mCurrentlyUsedShader;}

private:

	friend class ShaderManager;
	//called by ShaderManager when a new shader becomes necessary as the rendering scenario has changed;
	//can be NULL if material won't be used by current stage
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

	float mShininess;
	float mReflectivity;

};



}
