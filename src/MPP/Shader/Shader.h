/*
 * Shader.h
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#pragma once


#include "MPP/MPP.h"

#include "Common/FlewnitSharedDefinitions.h"
#include "Simulator/SimulatorForwards.h"



namespace Flewnit
{




enum ShaderStageType
{
	VERTEX_SHADER_STAGE,
	TESSELATION_CONTROL_SHADER_STAGE,
	TESSELATION_EVALUATION_SHADER_STAGE,
	GEOMETRY_SHADER_STAGE,
	FRAGMENT_SHADER_STAGE,
	__NUM_SHADER_STAGES__
};

const String shaderStageFileEndings[] =
{
	"vert",
	"tessCtrl",
	"tessEval",
	"geom",
	"frag"
};

//define binding points for the several potentially used uniform buffers;
//this way, every buffer has its own index and we don't need to track
enum UniformBufferBindingPoint
{
	INSTANCE_TRANSFORM_BUFFER_BINDING_POINT =0,
	LIGHT_SOURCE_BUFFER_BINDING_POINT =1,
	SHADOW_CAMERA_TRANSFORM_BUFFER_BINDING_POINT = 2
};


class ShaderStage
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

private:

	friend class Shader;

	ShaderStage(ShaderStageType shaderStageType, String sourceCode,
				Path codeDirectory, Path shaderName, Shader* owningShader);
	~ShaderStage();


	void setSource(String sourceCode);
	void compile();

	void validate()throw(BufferException);


	ShaderStageType mType;

	GLuint mGLShaderStageHandle;


	String mSourceCode;

	Path mCodeDirectory;
	Path mSpecificShaderCodeSubFolderName;

	static GLuint mGLShaderStageIdentifiers[__NUM_SHADER_STAGES__];

	Shader* mOwningShader;
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
	virtual bool operator==(const Shader& rhs)const;

	void link();

	//check compiled and linked programm for errors
	virtual void validate()throw(SimulatorException);

	// update uniforms and use the shader program:
	//pass a subobject, as the shader needs access to
	//	- the material for textures and other material dependent uniforms
	//	- the world object for non-instanced geometry to read the golabe transform (model matrix)
	//	- the instance manager for instanced geometry to bind the model matrix uniform buffer;
	virtual void use(SubObject* so)throw(SimulatorException)=0;
	//maybe make virtual later.. shouldn't be TOO hard to extract some switch statements
	//virtual void use(VisualMaterial* visMat)throw(SimulatorException)=0;

	inline const ShaderFeaturesLocal& getLocalShaderFeatures()const{return mLocalShaderFeatures;}

	//acessor for LightSourceManager to use the handle directly for its queries
	inline GLuint getGLProgramHandle()const{return mGLProgramHandle;}


protected:

	friend class ShaderManager;
	Shader(Path codeDirectory, Path specificShaderCodeSubFolderName, const ShaderFeaturesLocal& localShaderFeatures);

	//called by constructor
	virtual void build();
	//setup the context for template rendering:
	virtual void setupTemplateContext(TemplateContextMap& contextMap);
	void generateShaderStage(ShaderStageType shaderStageType, Grantlee::Engine* templateEngine, const TemplateContextMap& contextMap);


	//bind G-buffer textures to output fragments;
	//if the RenderTarget has no texture attached to with a semantics
	//wirtten to by the fragment shader, an exception is thrown;
	//throw also exception if texture type of attached textures isn't equal to the current renderTargetTextureType;
	//optional, only needed when using multiple render targets;
	//called by ShaderManager::setRenderingScenario
	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException)=0;


	void attachCompiledStage(ShaderStageType which);
	void detachCompiledStage(ShaderStageType which);


	//------- uniform setter routines, to be called by derived concrete shaders when appropriate

	//Calculates and sets all needed permutations of model/view/projection/lookAt/shadowMapLookup matrices
	//handles, if appropriate, buffer binding of:
	// 	- shadowmapmatrices buffer
	//	- instance-transformation-matrices buffer
	void setupTransformationUniforms(SubObject* so);
	//either binds the lightsoure uniform buffer or sets the non-buffer-single-lightsource uniforms directly
	void setupLightSourceUniforms();
	void setupMaterialUniforms(VisualMaterial* visMat);
	void setupTessellationParameters(VisualMaterial* visMat);


	void bindUniformBuffer(UniformBufferBindingPoint bindingPoint,String bufferNameInShader, GLuint bufferGLHandle);
	void bindMatrix4x4(String uniformName, const Matrix4x4& mat);
	void bindVector4D(String uniformName, const Vector4D& vec);
	void bindVector3D(String uniformName, const Vector3D& vec);
	void bindFloat(String uniformName, float val);
	void bindInt(String uniformName, int val);


	Path mCodeDirectory;
	Path mSpecificShaderCodeSubFolderName;

	ShaderFeaturesLocal mLocalShaderFeatures;

	GLuint mGLProgramHandle;
	ShaderStage* mShaderStages[__NUM_SHADER_STAGES__];

	//to be compared to rendertarget; Initial state: all -1;
	GLint mCurrentFragDataBindings[__NUM_TOTAL_SEMANTICS__];
};

}
