/*
 * Shader.h
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "MPP/MPP.h"

#include "Common/FlewnitSharedDefinitions.h"
#include "Simulator/SimulatorForwards.h"

#pragma once



namespace Flewnit
{

enum ShaderStageType
{
	VERTEX_SHADER_STAGE,
	GEOMETRY_SHADER_STAGE,
	TESSELATION_CONTROL_SHADER_STAGE,
	TESSELATION_EVALUATE_SHADER_STAGE,
	FRAGMENT_SHADER_STAGE,
	__NUM_SHADER_STAGES__
};

class ShaderStage
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

private:
	ShaderStageType type;

	GLuint mGLShaderStageHandle;
};

class Shader
:public MPP
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	friend class ShaderManager;

	Shader(String name, const ShaderFeaturesLocal& localShaderFeatures,
			Path codeDirectory);

public:
	virtual ~Shader();

	//check for equality in order to check if a shader with the desired properties
	//(shader feature set) already exists in the ResourceManager;
	//compares path and mLocalShaderFeatures
	bool operator==(const Shader& rhs)const;

	virtual void generateShaderSources()=0;

	void compile();

	void link();

	virtual void use(VisualMaterial* visMat)throw(SimulatorException)=0;

	inline const ShaderFeaturesLocal& getLocalShaderFeatures()const{return mLocalShaderFeatures;}
private:

	//bind G-buffer textures to output fragments, or bind them as input samplers,
	//bind the lightsource buffers and set the related uniforms,...
	virtual void bindUniforms(VisualMaterial* currentActiveMaterial)=0;


	ShaderFeaturesLocal mLocalShaderFeatures;

	Path mCodeDirectory;

	GLuint mGLProgramHandle;
};

}
