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


class QString;
class QVariant;
template <class Key, class T>class QHash;
typedef QHash<QString, QVariant> QVariantHash;
typedef QVariantHash TemplateContextMap;


//class GrantleeShaderFeaturesContext;

namespace Flewnit
{




enum ShaderStageType
{
	VERTEX_SHADER_STAGE,
	TESSELATION_CONTROL_SHADER_STAGE,
	TESSELATION_EVALUATE_SHADER_STAGE,
	GEOMETRY_SHADER_STAGE,
	FRAGMENT_SHADER_STAGE,
	__NUM_SHADER_STAGES__
};

const String ShaderStageFileEndings[] =
{
	"vert",
	"tessCtrl",
	"tessEval",
	"geom",
	"frag"
};

//define binding points for the several potentially used uniform buffers;
//this way, every buffer has its own index and we don't need to track
enum UniformBufferBindingPoints
{
	INSTANCE_TRANSFORMATION_MATRICES_BUFFER_BINDING_POINT =0,
	LIGHT_SOURCES_BUFFER_BINDING_POINT =1,
	SHADOW_MAP_LOOKUP_MATRICES_BUFFER_BINDING_POINT = 2
};


//enum ShaderCodeSectionID
//{
//	VERSION_TAG,
//	PRECISION_TAG,
//	PERSISTENT_DEFINES,
//	CUSTOMIZABLE_DEFINES,
//
//	DATA_TYPES,
//	MATERIAL_SAMPLERS,
//	SHADOWMAP_SAMPLERS,
//	GBUFFER_SAMPLERS,
//	UNIFORMS,
//	INPUT,
//	OUTPUT,
//
//	SUBROUTINE_GET_DISTANCE_ATTENUATION,
//	SUBROUTINE_GET_NORMAL,
//	SUBROUTINE_GET_SHADOW_ATTENUATION,
//	SUBROUTINE_GET_SPOTLIGHT_ATTENUATION,
//
//	MAIN_ROUTINE,
//
//	__NUM_SHADER_CODE_SECTIONS__
//};

class ShaderStage
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

private:

	friend class Shader;

	ShaderStage(ShaderStageType shaderStageType, String sourceCode,
				Path codeDirectory, Path shaderName);
	~ShaderStage();

	//throw exceptionif the section does not apply to the shader type
	//(e.g. customizable #defines cannot be loaded but only generated on the fly)
	//if file for code does not exist, it is assumed that this is on purpose as the code
	//snippet is not needed for this stage (like shadow attenuation for geometry, for example ;))
	//void loadCodeSection(ShaderCodeSectionID which )throw(SimulatorException);
	//void propagateLoadedSourceToGL();

	void setSource(String sourceCode);
	void compile();

	void validate()throw(BufferException);


	ShaderStageType mType;

	GLuint mGLShaderStageHandle;


	String mSourceCode;

	Path mCodeDirectory;
	Path mShaderName;

	static GLuint mGLShaderStageIdentifiers[__NUM_SHADER_STAGES__];

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


	//void generateShaderSources();

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
	//setup the context for template rendering:
	void setupTemplateContext(TemplateContextMap& contextMap);
	//for later debugging of the final code of a stage:
	void writeToDisk(String sourceCode, ShaderStageType type);

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
	void setupMatrixUniforms(Camera *mainCam, SubObject* so);
	//either binds the lightsoure uniform buffer or sets the non-buffer-single-lightsource uniforms directly
	void setupLightSourceUniforms(Camera *mainCam);
//public:	//public for InstanceManager:
//	void bindInstanceTransformationInfoUniformBuffer(Buffer* uniformBuffer);
//protected:
	//---------------------------------------------------------------------------------------


	Path mCodeDirectory;
	Path mShaderName;

	ShaderFeaturesLocal mLocalShaderFeatures;


	//filled by generateCustomDefines(), used for straig forward #define-string generation OR for
	//parsing a template
	//std::set<String> mDefinitionsSet[__NUM_CUSTOM_SHADER_DEFINITIONS__];

	GLuint mGLProgramHandle;
	ShaderStage* mShaderStages[__NUM_SHADER_STAGES__];

	//to be compared to rendertarget; Initial state: all -1;
	GLint mCurrentFragDataBindings[__NUM_TOTAL_SEMANTICS__];

	//GrantleeShaderFeaturesContext* mGrantleeShaderFeaturesContext;
};

}
