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

enum ShaderCodeSectionID
{
	VERSION_TAG,
	PRECISION_TAG,
	PERSISTENT_DEFINES,
	CUSTOMIZABLE_DEFINES,

	DATA_TYPES,
	MATERIAL_SAMPLERS,
	SHADOWMAP_SAMPLERS,
	GBUFFER_SAMPLERS,
	UNIFORMS,
	INPUT,
	OUTPUT,

	SUBROUTINE_GET_DISTANCE_ATTENUATION,
	SUBROUTINE_GET_NORMAL,
	SUBROUTINE_GET_SHADOW_ATTENUATION,
	SUBROUTINE_GET_SPOTLIGHT_ATTENUATION,

	MAIN_ROUTINE,

	__NUM_SHADER_CODE_SECTIONS__
};

class ShaderStage
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

private:

	friend class Shader;

	ShaderStage(ShaderStageType shaderStageType, Path codeDirectory, Path shaderName);

	//throw exception if file for code does not exist or if the section does not apply
	//to the shader type (e.g. customizable #defines cannot be loaded but only generated on the fly)
	void loadCodeSection(ShaderCodeSectionID which )throw(SimulatorException);
	void propagateLoadedSourceToGL();
	void compile();
	//for later debugging of the final code of a stage:
	void writeToDisk();
	void validate()throw(BufferException);

	ShaderStageType type;

	GLuint mGLShaderStageHandle;

	String mCodeSectionStrings[__NUM_SHADER_CODE_SECTIONS__];

};

class Shader
:public MPP
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	virtual ~Shader();

	//check for equality in order to check if a shader with the desired properties
	//(shader feature set) already exists in the ResourceManager;
	//compares path and mLocalShaderFeatures
	bool operator==(const Shader& rhs)const;


	void generateShaderSources();

	void link();

	//check compiled and linked programm for errors
	void validate()throw(BufferException);

	// update uniforms and use the shader program:
	//pass a subobject, as the shader needs access to
	//	- the material for textures and other material dependent uniforms
	//	- the world object for non-instanced geometry to read the golabe transform (model matrix)
	//	- the instance manager for instanced geometry to bind the model matrix uniform buffer;
	virtual void use(SubObject* so)throw(SimulatorException)=0;
	//maybe make virtual later.. shouldn't be TOO hard to extract some switch statements
	//virtual void use(VisualMaterial* visMat)throw(SimulatorException)=0;

	inline const ShaderFeaturesLocal& getLocalShaderFeatures()const{return mLocalShaderFeatures;}


protected:

	friend class ShaderManager;
	Shader(Path codeDirectory, Path shaderName, const ShaderFeaturesLocal& localShaderFeatures);

	//called by constructor
	void build();

	//bind G-buffer textures to output fragments;
	//if the RenderTarget has no texture attached to with a semantics
	//wirtten to by the fragment shader, an exception is thrown;
	//throw also exception if texture type of attached textures isn't equal to the current renderTargetTextureType;
	//optional, only needed when using multiple render targets;
	//called by ShaderManager::setRenderingScenario
	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException)=0;

	virtual void generateCustomDefines()=0;

	void attachCompiledStage(ShaderStageType which);

	ShaderFeaturesLocal mLocalShaderFeatures;

	Path mCodeDirectory;
	Path mShaderName;

	GLuint mGLProgramHandle;
	ShaderStage* mShaderStages[__NUM_SHADER_STAGES__];

	//to be compared to rendertarget; Initial state: all -1;
	GLint mCurrentFragDataBindings[__NUM_TOTAL_SEMANTICS__];
};





class GenericLightingUberShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	//public constructor for material-less deferred lighting
	GenericLightingUberShader(Path codeDirectory, const ShaderFeaturesLocal& localShaderFeatures);

	virtual ~GenericLightingUberShader();

	virtual void use(SubObject* so)throw(SimulatorException);

protected:


	virtual void generateCustomDefines();
	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};

}
