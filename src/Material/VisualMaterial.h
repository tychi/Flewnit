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
	VisualMaterial(String name,
			VisualMaterialType type, ShaderFeatures shaderFeatures,
			//must contain at least the textures used in the shader as samplers
			const Map<BufferSemantics, Texture*>& textures,
			//some stuff shall not cast shadows, like the skybox or
			//lightsource/camera visualization geometry
			bool castsShadows = true,
			//value to mask some dummy geometry, e.g.
			//-a low-detail model only used for shadowmap generation,
			//-the skybox cube which shall not be lit
			//the uniform grid structure, which shall only be rendered for debug purposes
			bool isShadable = true);

	virtual ~VisualMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const = 0;

	virtual bool activate(SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject)=0;
	virtual bool deactivate()=0;

	//looks within the ResourceManager for a shader with the same feature set as this
	//Material; if found, mShader is set to this found shader and its reference count;
	//Otherwise, a new shader is generated;
	virtual void generateOrGrabShader()=0;

	bool castsShadows()const{return mCastsShadows;}
	bool isShadable()const{return mIsShadable;}

	inline VisualMaterialType getType()const{return mType;}
	inline const ShaderFeatures& getShaderFeatures()const{return mShaderFeatures;}

	void setTexture(Texture* tex);
	Texture* getTexture(BufferSemantics bs)const;

private:
	bool mCastsShadows;
	bool mIsShadable;

	VisualMaterialType mType;
	ShaderFeatures mShaderFeatures;

	Map<BufferSemantics, Texture*> mTextures;

	Shader* mShader;
};

}
