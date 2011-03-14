/*
 * Shader.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "Shader.h"

#include "URE.h"

#include "Simulator/OpenCL_Manager.h"
#include "MPP/Shader/ShaderManager.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"

#include "Simulator/LightingSimulator/LightingSimulator.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"


#include "Util/Log/Log.h"


#include "Buffer/BufferHelperUtils.h"
#include "Buffer/Buffer.h"
#include "Buffer/Texture.h"

#include "WorldObject/InstanceManager.h"
#include "WorldObject/SubObject.h"
#include "Geometry/InstancedGeometry.h"


#include <grantlee/engine.h>
#include "UserInterface/WindowManager/WindowManager.h"
#include "Util/HelperFunctions.h"

typedef QVariantHash TemplateContextMap;



namespace Flewnit
{


//static member init:
GLuint ShaderStage::mGLShaderStageIdentifiers[__NUM_SHADER_STAGES__] =
	{
			GL_VERTEX_SHADER,
			GL_TESS_CONTROL_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_GEOMETRY_SHADER,
			GL_FRAGMENT_SHADER
	};



Shader::Shader(Path codeDirectory, Path specificShaderCodeSubFolderName, const ShaderFeaturesLocal& localShaderFeatures)
:
		//generate a unigque name:
		MPP(	specificShaderCodeSubFolderName.string() + localShaderFeatures.stringify(),
				//String(shaderName.string()).append(localShaderFeatures.stringify()),
				//String (shaderName.string() + localShaderFeatures.stringify() ),
				VISUAL_SIM_DOMAIN
		),
		mCodeDirectory(ShaderManager::getInstance().getShaderCodeDirectory()),
		mSpecificShaderCodeSubFolderName(specificShaderCodeSubFolderName),
		mLocalShaderFeatures(localShaderFeatures)
//		mGrantleeShaderFeaturesContext(
//				new GrantleeShaderFeaturesContext(
//					mLocalShaderFeatures,
//					ShaderManager::getInstance().getGlobalShaderFeatures()
//				)
//		)
{

	for(int i=0; i< __NUM_SHADER_STAGES__; i++)
	{
		mShaderStages[i]=0;
	}
	build();

}



Shader::~Shader()
{
	for(int i=0; i< __NUM_SHADER_STAGES__; i++)
	{
		//detachCompiledStage(ShaderStageType(i));
		delete mShaderStages[i];
	}
	GUARD(glDeleteProgram(mGLProgramHandle));
}



//called by constructor
void Shader::build()
{
	GUARD( mGLProgramHandle = glCreateProgram() );

    Grantlee::Engine *templateEngine = new Grantlee::Engine();
    Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );
    String shaderDirectory=	(mCodeDirectory / mSpecificShaderCodeSubFolderName).string() ;
    String commonCodeSnippetsDirectory = (mCodeDirectory / Path("Common")).string();
    loader->setTemplateDirs( QStringList() << shaderDirectory.c_str() << commonCodeSnippetsDirectory.c_str());
    templateEngine->addTemplateLoader(loader);

    //setup the context to delegate template rendering according to the shaderFeatures (both local and global):
    TemplateContextMap contextMap;
    setupTemplateContext(contextMap);

    //--------------------------------------------------------------------

    generateShaderStage(VERTEX_SHADER_STAGE,templateEngine,contextMap);

    if((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_TESSELATION ) != 0 )
    {
    	assert(WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4);
    	generateShaderStage(TESSELATION_CONTROL_SHADER_STAGE,templateEngine,contextMap);
    	generateShaderStage(TESSELATION_EVALUATION_SHADER_STAGE,templateEngine,contextMap);
    }

    if(ShaderManager::getInstance().currentRenderingScenarioNeedsGeometryShader())
    {
		//when do we need a geometry shader?
		//if we need to trender to a cubemap, an array texture or if wee need to render primitive IDs
    	generateShaderStage(GEOMETRY_SHADER_STAGE,templateEngine,contextMap);
    }

    if(ShaderManager::getInstance().currentRenderingScenarioNeedsFragmentShader())
    {
    	generateShaderStage(FRAGMENT_SHADER_STAGE,templateEngine,contextMap);
    }

    link();


}

void Shader::generateShaderStage(ShaderStageType shaderStageType,  Grantlee::Engine* templateEngine, const TemplateContextMap& contextMap)
{
    //generate shader stage source code:
	Grantlee::Template shaderTemplate = templateEngine->loadByName(
			String(String("main.") + shaderStageFileEndings[shaderStageType]).c_str()
	);
	Grantlee::Context shaderTemplateContext(contextMap);
    String shaderSourceCode = shaderTemplate->render(&shaderTemplateContext).toStdString();
	writeToDisk(shaderSourceCode, shaderStageType);

    //create the fragment shader:
    mShaderStages[shaderStageType] = new ShaderStage(
    		shaderStageType, shaderSourceCode, mCodeDirectory, mSpecificShaderCodeSubFolderName,this);
    attachCompiledStage(shaderStageType);
}


//for later inspection of the final code of a stage:
void Shader::writeToDisk(String sourceCode, ShaderStageType type)
{
	String shaderDirectory=
		(	mCodeDirectory  / Path("__generated") /
			  Path( mSpecificShaderCodeSubFolderName.string()+ mLocalShaderFeatures.stringify() + String(".")+  shaderStageFileEndings[type] )
		).string() ;
	std::fstream fileStream;
	fileStream.open(shaderDirectory.c_str(), std::ios::out);
	fileStream << sourceCode;
	fileStream.close();
}


void Shader::attachCompiledStage(ShaderStageType which)
{
	GUARD(glAttachShader(mGLProgramHandle, mShaderStages[which]->mGLShaderStageHandle));
	//GUARD(mShaderStages[which]->compile());
}

void Shader::detachCompiledStage(ShaderStageType which)
{
	GUARD(glDetachShader(mGLProgramHandle, mShaderStages[which]->mGLShaderStageHandle));
}





//check for equality in order to check if a shader with the desired properties
//(shader feature set) already exists in the ResourceManager;
//compares path and mLocalShaderFeatures
bool Shader::operator==(const Shader& rhs)const
{
	return
		mCodeDirectory == rhs.mCodeDirectory &&
		mSpecificShaderCodeSubFolderName == rhs.mSpecificShaderCodeSubFolderName &&
		mLocalShaderFeatures == rhs.mLocalShaderFeatures;
}




void Shader::setupTemplateContext(TemplateContextMap& contextMap)
{
	//test values; TODO setup according to shaderfeatures struct



	//QVariant shaderFeaturesVariant= QVariant::fromValue(mGrantleeShaderFeaturesContext);
	//context.insert("shaderFeatures",shaderFeaturesVariant);

	ShaderFeaturesGlobal sfg(ShaderManager::getInstance().getGlobalShaderFeatures());
	ShaderFeaturesLocal sfl (mLocalShaderFeatures);

	//DEBUG mod the features in order to check template rendering
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_DIFFUSE_TEXTURING );
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_DETAIL_TEXTURING);
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_NORMAL_MAPPING );
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_CUBE_MAPPING);

	//sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT;

	//sfl.renderingTechnique= RENDERING_TECHNIQUE_SHADOWMAP_GENERATION;
	//sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT;
	//sfl.visualMaterialType = VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY;

	//END DEBUG

	contextMap.insert("GL_MAYOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().x);
	contextMap.insert("GL_MINOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().y);

	contextMap.insert("layeredRendering",ShaderManager::getInstance().currentRenderingScenarioPerformsLayeredRendering());
	contextMap.insert("worldSpaceTransform",ShaderManager::getInstance().shaderNeedsWorldSpaceTransform());

	bool shaderPerformsColorCalculations =
			(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFAULT_LIGHTING)
			||
			(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)
			||
			(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING)
			;
	contextMap.insert("shaderPerformsColorCalculations",shaderPerformsColorCalculations);

	bool shadeSpacePositionNeeded =
			shaderPerformsColorCalculations
			||
			ShaderManager::getInstance().currentRenderingScenarioPerformsLayeredRendering()
			||
			((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_TESSELATION) != 0)
			;
	contextMap.insert("shadeSpacePositionNeeded",shadeSpacePositionNeeded);

	bool texCoordsNeeded=
		((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_TESSELATION) != 0)
	||  ((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_DIFFUSE_TEXTURING) != 0)
	||  ((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_DETAIL_TEXTURING) != 0)
	||  ((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_NORMAL_MAPPING) != 0)
			;
	contextMap.insert("texCoordsNeeded",texCoordsNeeded);

	bool depthImageOrPointLightSMGen =
			(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION)
			||
			(
				(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
				&&
				(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
					== LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT)
			)
			;
	contextMap.insert("depthImageOrPointLightSMGen",depthImageOrPointLightSMGen);




	for(unsigned int i = 0; i < __NUM_TOTAL_SEMANTICS__;i++)
	{
		//add the numeric value of the semantics enum in order to define the layout qualifier for the vertex shader
		contextMap.insert(
				BufferHelper::BufferSemanticsToString(BufferSemantics(i)).c_str(),
				i
		);
	}


	for(unsigned int i = 0; i < __NUM_VISUAL_MATERIAL_TYPES__;i++)
	{
		contextMap.insert(
				VisualMaterialTypeStrings[i].c_str(),
				(bool) ( (unsigned int)(sfl.visualMaterialType) == i)
		);
	}


	for(unsigned int i = 0; i < __NUM_RENDERING_TECHNIQUES__;i++)
	{
		contextMap.insert(
				RenderingTechniqueStrings[i].c_str(),
				(bool) ( (unsigned int)(sfl.renderingTechnique) == i)
		);
	}

	//propagete RT and G-Buffer texture type to template
	for(unsigned int i = 0; i < __NUM_TEXTURE_TYPES__;i++)
	{
		contextMap.insert(
				(String("RENDER_TARGET_") + TextureTypeStrings[i]).c_str(),
				(bool) ( (unsigned int)(sfl.renderTargetTextureType) == i)
		);

		contextMap.insert(
				(String("G_BUFFER_") + TextureTypeStrings[i]).c_str(),
				(bool) ( (unsigned int)(sfg.GBufferType) == i)
		);
	}



	for(unsigned int i = 0; i < __NUM_SHADING_FEATURES__;i++)
	{
		contextMap.insert(
				ShadingFeatureStrings[i].c_str(),
				(bool) (
					 (
						(unsigned int)(sfl.shadingFeatures) & (1<<i)
					 ) != 0
				)
		);
	}



	for(unsigned int i = 0; i < __NUM_LIGHT_SOURCES_LIGHTING_FEATURES__;i++)
	{
		contextMap.insert(
				LightSourcesLightingFeatureStrings[i].c_str(),
				(bool) ( (unsigned int)(sfg.lightSourcesLightingFeature) == i)
		);
	}

	for(unsigned int i = 0; i < __NUM_LIGHT_SOURCES_SHADOW_FEATURES__;i++)
	{
		contextMap.insert(
				LightSourcesShadowFeatureStrings[i].c_str(),
				(bool) ( (unsigned int)(sfg.lightSourcesShadowFeature) == i)
		);
	}


	for(unsigned int i = 0; i < __NUM_SHADOW_TECHNIQUES__;i++)
	{
		contextMap.insert(
				ShadowTechniqueStrings[i].c_str(),
				(bool) ( (unsigned int)(sfg.shadowTechnique) == i)
		);
	}


	contextMap.insert("instancedRendering", sfl.instancedRendering);
	contextMap.insert("numMaxInstancesRenderable", sfg.numMaxInstancesRenderable);
	contextMap.insert("invNumMaxInstancesRenderable", 1.0f /sfg.numMaxInstancesRenderable);


	contextMap.insert("numMaxLightSources", sfg.numMaxLightSources);
	contextMap.insert("invNumMaxLightSources", 1.0f /sfg.numMaxLightSources);

	contextMap.insert("numMaxShadowCasters", sfg.numMaxShadowCasters);
	contextMap.insert("invNumMaxShadowCasters", 1.0f /sfg.numMaxShadowCasters);


	contextMap.insert("numMultiSamples", sfg.numMultiSamples);
	contextMap.insert("invNumMultiSamples", 1.0f /sfg.numMultiSamples);


	contextMap.insert("invLightSourceFarClipPlane",
			1.0f / LightSourceManager::getInstance().getLightSourceProjectionMatrixFarClipPlane());


	float tangensCamFovHorizontal;
	float tangensCamFovVertical;
	float cameraFarClipPlane;

	if(sfl.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
	{
		cameraFarClipPlane = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
				getLightSourceManager()->getLightSourceProjectionMatrixFarClipPlane();

		tangensCamFovHorizontal = 1.0f; //tangens plays no role in shadow map generation;
		tangensCamFovVertical= 1.0f; //tangens plays no role in shadow map generation;
	}
	else
	{
		cameraFarClipPlane = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
				getMainCamera()->getFarClipPlane();


		float camFOVVertRadians =
				glm::radians(
						URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
						getMainCamera()->getVerticalFOVAngle()
				);
		float aspect = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
							getMainCamera()->getAspectRatioXtoY();

		tangensCamFovHorizontal = 	glm::tan( camFOVVertRadians * aspect );
		tangensCamFovVertical = 	glm::tan( camFOVVertRadians );


	}

	contextMap.insert("cameraFarClipPlane", cameraFarClipPlane);
	contextMap.insert("invCameraFarClipPlane", 1.0f / cameraFarClipPlane );

	contextMap.insert("tangensCamFovHorizontal", tangensCamFovHorizontal);
	contextMap.insert("tangensCamFovVertical", tangensCamFovVertical);
	contextMap.insert("cotangensCamFovHorizontal", 1.0f /  tangensCamFovHorizontal);
	contextMap.insert("cotangensCamFovVertical", 1.0f / tangensCamFovVertical );


}



void Shader::link()
{
	GUARD(glLinkProgram(mGLProgramHandle));
	validate();
}



//check compiled and linked programm for errors
void Shader::validate()throw(BufferException)
{
	const int buffSize = 1000000; //yes, one million ;P
	int shaderInfoLogSize=0;
	GLchar logBuffer[buffSize];

	GUARD( glValidateProgram(mGLProgramHandle) );

	GUARD(glGetProgramInfoLog(mGLProgramHandle,buffSize,&shaderInfoLogSize,logBuffer));

	if(shaderInfoLogSize >0)
	{
		//something went wrong, write info to disk for easier inspection
		String logFileName=
				mSpecificShaderCodeSubFolderName.string() + mLocalShaderFeatures.stringify() +
				String("_LOG_PROGRAM.txt");

		Path logFilePath= mCodeDirectory  / Path("__generated") / Path(logFileName);
		std::fstream fileStream;
		fileStream.open(logFilePath.string().c_str(), std::ios::out);
		fileStream << logBuffer;
		fileStream.close();
	}

}



//----------------------------------------------------------------------------------------------


ShaderStage::ShaderStage(ShaderStageType shaderStageType,
		String sourceCode, Path codeDirectory, Path shaderName,
		Shader* owningShader)
:
		mType(shaderStageType),
		mSourceCode(sourceCode),
		mCodeDirectory(codeDirectory),
		mSpecificShaderCodeSubFolderName(shaderName),
		mOwningShader(owningShader)
{
	GUARD( mGLShaderStageHandle = glCreateShader(mGLShaderStageIdentifiers[shaderStageType]));
	setSource(sourceCode);
	compile();
	validate();
}


ShaderStage::~ShaderStage()
{
	GUARD(glDeleteShader(mGLShaderStageHandle););
}


void ShaderStage::setSource(String sourceCode)
{
	mSourceCode = sourceCode;

	const GLchar* codeAsChar = mSourceCode.c_str();
	GLint codeLength = mSourceCode.length();

	GUARD(
		glShaderSource(
			mGLShaderStageHandle,
			1,
			& codeAsChar,
			& codeLength
			//static_cast<const GLchar**>( & reinterpret_cast<const GLchar*>( sourceCode.c_str() ) ),
			//static_cast<const GLchar**>( &( sourceCode.c_str() ) ),
			//& static_cast<const GLint>( (sourceCode.length()) )
		)

	);
}



void ShaderStage::compile()
{
	GUARD(glCompileShader(mGLShaderStageHandle));
}




void ShaderStage::validate()throw(BufferException)
{
	const int buffSize = 1000000; //yes, one million ;P
	int shaderInfoLogSize=0;
	static GLchar logBuffer[buffSize];
	GUARD(glGetShaderInfoLog(mGLShaderStageHandle,buffSize,&shaderInfoLogSize,logBuffer));

	if(shaderInfoLogSize >0)
	{
		//something went wrong, write info to disk for easier inspection
		String logFileName=
				mSpecificShaderCodeSubFolderName.string() +
				mOwningShader->getLocalShaderFeatures().stringify() +
				String(".")+
				shaderStageFileEndings[mType]+
				String("_LOG_STAGE")+
				String(".txt");

		Path logFilePath= mCodeDirectory / Path("__generated") / Path(logFileName);


		std::fstream fileStream;
		fileStream.open(logFilePath.string().c_str(), std::ios::out);
		fileStream << logBuffer;
		fileStream.close();
	}
}

//============ uniform variables logic to come ==========================================================================

void Shader::bindUniformBuffer(UniformBufferBindingPoint bindingPoint, String bufferNameInShader, GLuint bufferGLHandle)
{
	GUARD(
		glBindBufferBase(GL_UNIFORM_BUFFER,
			static_cast<GLuint>(bindingPoint),
			bufferGLHandle);
	);
	GUARD (GLuint uniformBlockIndex =  glGetUniformBlockIndex(mGLProgramHandle, bufferNameInShader.c_str()));
	assert(uniformBlockIndex != GL_INVALID_INDEX);
	GUARD(glUniformBlockBinding(
			mGLProgramHandle,
			uniformBlockIndex,
			static_cast<GLuint>(bindingPoint)
			)
	);
}


void Shader::bindMatrix4x4(String uniformName, const Matrix4x4& mat)
{
    GUARD(
    	glUniformMatrix4fv(
    		glGetUniformLocation(mGLProgramHandle,uniformName.c_str()),
	    	1,
	    	GL_FALSE,
	    	&(mat[0][0])
	    )
	);
}


void Shader::bindVector4D(String uniformName, const Vector4D& vec)
{
    GUARD(
		glUniform4fv(
			glGetUniformLocation(mGLProgramHandle,uniformName.c_str()),
			1,
			&( vec[0])
		)
	);
}

void Shader::bindVector3D(String uniformName, const Vector3D& vec)
{
	GUARD(
		glUniform3fv(
			glGetUniformLocation(mGLProgramHandle,uniformName.c_str()),
			1,
			&( vec[0])
		)
	);
}

void Shader::bindFloat(String uniformName, float val)
{
	GUARD(
		glUniform1f(
			glGetUniformLocation(mGLProgramHandle,uniformName.c_str()),
			val
		)
	);
}

void Shader::bindInt(String uniformName, int val)
{
	GUARD(
		glUniform1i(
			glGetUniformLocation(mGLProgramHandle,uniformName.c_str()),
			val
		)
	);

}

//--------------------------------------------------

//Calculates and sets all needed permutations of model/view/projection/lookAt/shadowMapLookup matrices
//handles, if appropriate, buffer binding of:
// 	- shadowmapmatrices buffer
//	- instance-transformation-matrices buffer
void Shader::setupTransformationUniforms(SubObject* so)
{

	VisualMaterial* visMat = dynamic_cast<VisualMaterial*>(so->getMaterial());
	assert(visMat);
	Camera *cam = URE_INSTANCE->getCurrentlyActiveCamera();

	//afaik, trying to set a non existing uniform is no tragic thing;
	//maybe I should "bruteforce" try to set everything..? TODO checkout when stable

	//+++++++++++++ step 1: grab the standard model- independent matrices: view, proj, viewProj +++++++++++++++++++++++++
	//unused identity matrices if they aren't set below:
	Matrix4x4 viewMatrix(1.0f);
	Matrix4x4 projMatrix(1.0f);
	Matrix4x4 viewProjMatrix(1.0f);
	if (mLocalShaderFeatures.renderingTechnique==RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
	{
		if(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature ==
				LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
		{
			SpotLight* spot = dynamic_cast<SpotLight*> (LightSourceManager::getInstance().getFirstShadowCaster());
			assert("in this scenario, a shadow caster must be a spotlight" && spot);
			viewMatrix = spot->getViewMatrix(); //unneeded, but whatever.. :P
			projMatrix = spot->getProjectionMatrix();
			viewProjMatrix = spot->getViewProjectionMatrix();
		}
	}
	else
	{
		//assert(cam && "camera must be passed in non- shadow map generation passes");
		viewMatrix = cam->getGlobalTransform().getLookAtMatrix();
		projMatrix = cam->getProjectionMatrix();
		viewProjMatrix = cam->getProjectionMatrix() * viewMatrix;
	}


	//+++++++++++++ step 2: bind view, proj, viewProj to shader +++++++++++++++++++++++++

	//check if we render to a special render target which will make geometry shader delegation
	//necessary and hence the main cam's view matrix not only obsolete but maybe even name
	//conflicting with other variables
	if(	! ShaderManager::getInstance().currentRenderingScenarioPerformsLayeredRendering() )
	{
		bindMatrix4x4("viewMatrix",viewMatrix);
		bindMatrix4x4("projectionMatrix",projMatrix);
		bindMatrix4x4("viewProjectionMatrix",viewProjMatrix);
	}
	else
	{
		if(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
		{
			PointLight* pointLight=0; 	//declare it here as else come some strange compiler warnings
										//when declaring within switch statement;
			switch(mLocalShaderFeatures.renderTargetTextureType)
			{
			case TEXTURE_TYPE_2D_CUBE_DEPTH:
				for(int i=0; i<6; i++)
				{
					pointLight = dynamic_cast<PointLight*>(LightSourceManager::getInstance().getFirstShadowCaster());
					assert("pointlight shadowcaster must exist" && pointLight);
					bindMatrix4x4(
						String("cubeMapCameraViewMatrices[") + HelperFunctions::toString(i) + String("]"),
						pointLight->getViewMatrix(i));
					bindMatrix4x4(
							String("cubeMapCameraViewProjectionMatrices[") + HelperFunctions::toString(i)+String("]"),
							pointLight->getViewProjectionMatrix(i));
				}
				break;
			case TEXTURE_TYPE_2D_ARRAY_DEPTH:
				bindUniformBuffer(
					SHADOW_CAMERA_TRANSFORM_BUFFER_BINDING_POINT,
					String("ShadowCameraTransformBuffer"),
					LightSourceManager::getInstance().getShadowCameraTransformBuffer()->getGraphicsBufferHandle()
				);
				break;
			default: assert(0 && "no other layered rendering targets currently supported for shadow map generation;"
					"this is mainly a gl3 framework, i.e. stuff like layered cube map rendering won't be implemented too soon ;(");
			}
		}
		else
		{
			assert(mLocalShaderFeatures.renderTargetTextureType ==  TEXTURE_TYPE_2D_CUBE);

			//TODO implement
			assert(0 && "TODO: grab position from DynamicCubeMapGenerationStage and fill the cubeMapCameraViewMatrices");
		}
	}


	//+++++++++++++ step 3: bind model-related transforms to shader +++++++++++++++++++++++++

	//instanced rendering or not?
	if( visMat->isInstanced())
	{
		//setup the instance uniform buffer; n.b.: this is a valid instanced draw call, otherwise,
		//the VisualMaterial wouldn't have called the "Shader::use()" routine;
		assert(mLocalShaderFeatures.instancedRendering);
		InstancedGeometry* instancedGeo = dynamic_cast<InstancedGeometry*>(so->getGeometry());
		assert(instancedGeo);
		bindUniformBuffer(INSTANCE_TRANSFORM_BUFFER_BINDING_POINT,
				String("InstanceTransformBuffer"),
				instancedGeo->getInstanceManager()->getInstanceTransformationInfoUniformBuffer()->getGraphicsBufferHandle()
		);
	}
	else
	{
		//setup default "model" related transform uniforms
		Matrix4x4 modelMatrix(1.0f);
		if(mLocalShaderFeatures.visualMaterialType == VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING)
		{   //special case: neither world nor view space transform: set the sky box into centre of view, but don't rotate according to view direction
			modelMatrix = glm::translate(modelMatrix,  URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getPosition());
		}else{
			modelMatrix = so->getOwningWorldObject()->getGlobalTransform().getTotalTransform();
		}
		 bindMatrix4x4("modelMatrix",modelMatrix);

		 //note: when rendering to tex array or cube map, MV and MVP are unused;
		//but it doesn't matter to set them anyway; keep business logic as simple as possible ;(
	    //{
		Matrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
	    Matrix4x4 modelViewProjMatrix = viewProjMatrix *  modelMatrix;
	    		//cam->getProjectionMatrix() * modelViewMatrix; <-- BUG: wrong if shadowmapgeneration ;(
	    bindMatrix4x4("modelViewMatrix",modelViewMatrix);
	    bindMatrix4x4("modelViewProjectionMatrix",modelViewProjMatrix);
	    //}
	}


	// done in setupLightSourceUniforms();
//	//+++++++++++++ step 4: bind shadow map lookup transforms to shader +++++++++++++++++++++++++
//	LightSourcesShadowFeature lssf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature;
//	if(
//		//shadow stuff gloablly enabled?
//		( lssf != LIGHT_SOURCES_SHADOW_FEATURE_NONE)
//		&&
//		(	//do we actually do some lighting? (shadowing without lighting makes no sense...
//			//where light is, there is shadow... unless the programmer's too stupid to implement it ;(  )
//			((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_DIRECT_LIGHTING) != 0 )
//			||
//			((mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_GLOBAL_LIGHTING) != 0 )
//		)
//	)
//	{
//		SpotLight* sl=0;
//		PointLight* pl=0;
//		switch (lssf) {
//			case  LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT:
//				sl = dynamic_cast<SpotLight*>( LightSourceManager::getInstance().getFirstShadowCaster() );
//				assert( "we need a spotlight shadow caster" && sl);
//					TODO
//				break;
//			case  LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT:
//				pl = dynamic_cast<PointLight*>( LightSourceManager::getInstance().getFirstShadowCaster() );
//								assert( "we need a pointlight shadow caster" && sl);
//			TODO
//				break;
//			case  LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS:
//			TODO
//				break;
//			default:
//				assert(0 && "wtf");
//				break;
//		}
//
//	}
}



void Shader::setupLightSourceUniforms()
{
	const ShaderFeaturesGlobal& sfg = ShaderManager::getInstance().getGlobalShaderFeatures();
	Camera *cam = URE_INSTANCE->getCurrentlyActiveCamera();

	if(sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_NONE){return;}

	bindInt("numCurrentlyActiveLightSources",LightSourceManager::getInstance().getNumCurrentlyActiveLightingLightSources());
	bindFloat("invNumCurrentlyActiveLightSources",1.0f / static_cast<float>( LightSourceManager::getInstance().getNumCurrentlyActiveLightingLightSources()));

	//-----------------------------------------------------------------------------------

	if( (sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT)
	 ||	(sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
	)
	{
		const LightSourceShaderStruct & lsStruct = LightSourceManager::getInstance().getLightSource(0)->getdata();

	    Vector4D lightPos = Vector4D( LightSourceManager::getInstance().getLightSource(0)
	    		->getGlobalTransform().getPosition(),1.0f) ;
	    Vector4D lightDir = Vector4D( LightSourceManager::getInstance().getLightSource(0)
	    		->getGlobalTransform().getDirection(), 0.0f );
		if ( ! ShaderManager::getInstance().shaderNeedsWorldSpaceTransform())
		{
			//transform to view space
			lightPos =	cam->getGlobalTransform().getLookAtMatrix()	* lightPos;
			lightDir =	cam->getGlobalTransform().getLookAtMatrix()	* lightDir;
		}

	   bindVector4D("lightSource.position",lightPos);
	   bindVector4D("lightSource.diffuseColor",lsStruct.diffuseColor);
	   bindVector4D("lightSource.specularColor",lsStruct.specularColor);
	   bindVector4D("lightSource.direction",lightDir);

	   bindFloat("lightSource.innerSpotCutOff_Radians",lsStruct.innerSpotCutOff_Radians);
	   bindFloat("lightSource.outerSpotCutOff_Radians",lsStruct.outerSpotCutOff_Radians);
	   bindFloat("lightSource.spotExponent",lsStruct.spotExponent);
	   bindFloat("lightSource.shadowMapLayer",lsStruct.shadowMapLayer);
	} //endif "only one light source for lighting"
	else
	{
		//setup the uniform buffer as we have several light sources
		bindUniformBuffer(LIGHT_SOURCE_BUFFER_BINDING_POINT,
				String("LightSourceBuffer"),
				LightSourceManager::getInstance().getLightSourceUniformBuffer()->getGraphicsBufferHandle()
		);
	}


	//shadow map matrices -----------------------------------------------
	SpotLight* spot;
	PointLight* pointLight;
	switch(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature)
	{
	case LIGHT_SOURCES_SHADOW_FEATURE_NONE:
		//do nothing ;)
		break;
	case LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT:
		spot = dynamic_cast<SpotLight*>(
				LightSourceManager::getInstance().getFirstShadowCaster() );
		assert("in this scenario, a shadow caster must exist and must be a spotlight" && spot);
		if(ShaderManager::getInstance().shaderNeedsWorldSpaceTransform())
		{
			bindMatrix4x4("shadowMapLookupMatrix", spot->getBiasedViewProjectionMatrix());
		}
		else
		{
			bindMatrix4x4("shadowMapLookupMatrix", spot->getViewSpaceShadowMapLookupMatrix(cam));
		}
		break;

	case LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT:
		pointLight = dynamic_cast<PointLight*> (LightSourceManager::getInstance().getFirstShadowCaster());
				assert("in this scenario, a shadow caster must exist and must be a point light" && pointLight);
		if(ShaderManager::getInstance().shaderNeedsWorldSpaceTransform())
		{
			bindFloat("invLightSourceFarClipPlane", 1.0f / LightSourceManager::getInstance().getLightSourceProjectionMatrixFarClipPlane());
		}
		else
		{
			//remember: viewToPointShadowMapMatrix=
			//	invLightSourceFarClipPlane * inversepointLightTranslation * (camView)⁻1
			Matrix4x4 viewToPointLightShadowMapMatrix =
				( 1.0f / LightSourceManager::getInstance().getLightSourceProjectionMatrixFarClipPlane() )
				* glm::translate(Matrix4x4(), (-1.0f) * pointLight->getGlobalTransform().getPosition() )
				* glm::inverse( cam->getViewMatrix() )
			;
			bindMatrix4x4("viewToPointLightShadowMapMatrix",viewToPointLightShadowMapMatrix);
		}
		break;

	case LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS:
		bindUniformBuffer(
			SHADOW_CAMERA_TRANSFORM_BUFFER_BINDING_POINT,
			String("ShadowCameraTransformBuffer"),
			LightSourceManager::getInstance().getShadowCameraTransformBuffer()->getGraphicsBufferHandle()
		);
		break;
	default:
		assert(0 && "no other layered rendering targets currently supported;"
			"this is mainly a gl3 framework, i.e. stuff like layered cube map rendering won't be implemented too soon ;(");
		break;
	}

}




void Shader::setupMaterialUniforms(VisualMaterial* visMat)
{
	if( (mLocalShaderFeatures.renderingTechnique== RENDERING_TECHNIQUE_DEFAULT_LIGHTING) ||
		(mLocalShaderFeatures.renderingTechnique== RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL))
	{
		assert(visMat);

		bindFloat("shininess",visMat->getShininess());
		bindFloat("reflectivity",visMat->getReflectivity());

		if( ((visMat->getShadingFeatures() & SHADING_FEATURE_DIFFUSE_TEXTURING ) !=0 )
			&& (ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_DIFFUSE_TEXTURING))
		)
		{
			glActiveTexture(GL_TEXTURE0 + DIFFUSE_COLOR_SEMANTICS);
			visMat->getTexture(DIFFUSE_COLOR_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
			bindInt("decalTexture",DIFFUSE_COLOR_SEMANTICS);
		}
		if(	((visMat->getShadingFeatures() & SHADING_FEATURE_DETAIL_TEXTURING ) !=0 )
			&& (ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_DETAIL_TEXTURING))
		)
		{
			glActiveTexture(GL_TEXTURE0 + DETAIL_TEXTURE_SEMANTICS);
			visMat->getTexture(DETAIL_TEXTURE_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
			bindInt("detailTexture",DETAIL_TEXTURE_SEMANTICS);
		}
		if(	((visMat->getShadingFeatures() & SHADING_FEATURE_NORMAL_MAPPING) !=0 )
			&& (ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_NORMAL_MAPPING))
		)
		{
			glActiveTexture(GL_TEXTURE0 + NORMAL_SEMANTICS);
			visMat->getTexture(NORMAL_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
			bindInt("normalMap",NORMAL_SEMANTICS);
		}
		if( ((visMat->getShadingFeatures() & SHADING_FEATURE_CUBE_MAPPING ) !=0 )
			&& (ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_CUBE_MAPPING))
		)
		{
			glActiveTexture(GL_TEXTURE0 + ENVMAP_SEMANTICS);
			visMat->getTexture(ENVMAP_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
			bindInt("cubeMap",ENVMAP_SEMANTICS);
		}

		if(	((visMat->getShadingFeatures() & SHADING_FEATURE_AMBIENT_OCCLUSION ) !=0 )
			&& (ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_AMBIENT_OCCLUSION))
		)
		{
			//note: bind DEPTH_BUFFER_SEMANTICS instead of AMBIENT_OCCLUSION_SEMANTICS, as AMBIENT_OCCLUSION_SEMANTICS
			//contains the AO result and not the raw image for computation
			assert(0&& "sry AO not implemented yet"); //TODO
			glActiveTexture(GL_TEXTURE0 + DEPTH_BUFFER_SEMANTICS);
			visMat->getTexture(DEPTH_BUFFER_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
			bindInt("depthBufferForAO",DEPTH_BUFFER_SEMANTICS);
		}

		//setupTessellationParameters() returns doing nothing if tess is not enablled globally or in this mat
		//if( ((visMat->getShadingFeatures() & SHADING_FEATURE_TESSELATION) !=0 )
		//	&& ShaderManager::getInstance().tesselationIsEnabled()
		//)
		//{
			setupTessellationParameters(visMat);
		//}
	}
}


void Shader::setupTessellationParameters(VisualMaterial* visMat)
{

	if(
		( (visMat->getShadingFeatures() & SHADING_FEATURE_TESSELATION) == 0 )
		|| ( ! ShaderManager::getInstance().tesselationIsEnabled() )
	)
	{
		return; //tess is not active
	}

	//assert(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_TESSELATION) !=0 ));

	if( visMat->getTexture(DISPLACEMENT_SEMANTICS) )
	{
		glActiveTexture(GL_TEXTURE0 + DISPLACEMENT_SEMANTICS);
		visMat->getTexture(DISPLACEMENT_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("displacementMap",DISPLACEMENT_SEMANTICS);
	}


	bindFloat("numScreenPixels", static_cast<float>(WindowManager::getInstance().getWindowResolution().x));


	if(
		(mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
		&&
		( (mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_TESSELATION) != 0 )
	)
	{
	    //as every user varyings in the vertex shader are in world space, we have to write view space pos
		//OF THE SPECTOTOR CAM to gl_Position in case tess is active for SM gen, so that the tess control
		//shader can perform its view space dynamic LOD calculations; Even for shadow map generation,
		//the tesslevels should be performed in SPECTATOR cam space in order to omit
	    //artifacts due to different SM-generation- and comparison-geometry;

		Camera *cam = URE_INSTANCE->getCurrentlyActiveCamera();

		bindMatrix4x4("spectatorCamViewMatrix",  cam->getViewMatrix());
	}


	//rest still hardcoded in shader: later todo in faaar future ;)
//	"tessQualityFactor"
//	"distanceToBeginWithTesselation"
//	"distanceUntilFullSubdivision"
//	"minTessLevel"
//	"maxTessLevel"
//	"displacementIntensity"
}


}
