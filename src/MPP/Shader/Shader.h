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
public:
	Shader(String name, const ShaderFeatures& shaderFeatures,
			Path directoryToTemplateCode);
	virtual ~Shader();

	//check for equality in order to check if a shader with the desired properties
	//(shader feature set) already exists in the ResourceManager;
	//compares path an mShaderFeatures
	bool operator==(const Shader& rhs)const;

	virtual void generateShaderSources()=0;

	void compile();
	void link();

	void use();

	//bind G-buffer textures to output fragments, or bind them as input samplers,
	//bind the lightsource buffers and set the related uniforms,...
	virtual void bindCommonUniforms()=0;
	virtual void bindMaterialSpecificUniforms(Material* currentActiveMaterial);
	virtual void bindSamplers(Material* currentActiveMaterial)=0;

	virtual void setupAndUse();

	inline const ShaderFeatures& getShaderFeatures()const{return mShaderFeatures;}
private:
	ShaderFeatures mShaderFeatures;

	Path mDirectoryToTemplateCode;

	GLuint mGLProgramHandle;
};

}
