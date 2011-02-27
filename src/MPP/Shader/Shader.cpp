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

typedef QVariantHash TemplateContextMap;



namespace Flewnit
{


//static member init:
GLuint ShaderStage::mGLShaderStageIdentifiers[__NUM_SHADER_STAGES__] =
	{
			GL_VERTEX_SHADER,
			GL_GEOMETRY_SHADER,
			GL_TESS_CONTROL_SHADER,
			GL_TESS_EVALUATION_SHADER,
			GL_FRAGMENT_SHADER
	};



Shader::Shader(Path codeDirectory, Path shaderName, const ShaderFeaturesLocal& localShaderFeatures)
:
		//generate a unigque name:
		MPP(	shaderName.string() + localShaderFeatures.stringify(),
				//String(shaderName.string()).append(localShaderFeatures.stringify()),
				//String (shaderName.string() + localShaderFeatures.stringify() ),
				VISUAL_SIM_DOMAIN
		),
		mCodeDirectory(ShaderManager::getInstance().getShaderCodeDirectory()),
		mShaderName(shaderName),
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
    String shaderDirectory=	(mCodeDirectory / mShaderName).string() ;
    String commonCodeSnippetsDirectory = (mCodeDirectory / Path("Common")).string();
    loader->setTemplateDirs( QStringList() << shaderDirectory.c_str() << commonCodeSnippetsDirectory.c_str());
    templateEngine->addTemplateLoader(loader);

    //setup the context to delegate template rendering according to the shaderFeatures (both local and global):
    TemplateContextMap contextMap;

    setupTemplateContext(contextMap);



    Grantlee::Context shaderTemplateContext(contextMap);
    String shaderSourceCode;

    //--------------------------------------------------------------------

    //generate vertex shader source code:

    Grantlee::Template vertexShaderTemplate = templateEngine->loadByName( "main.vert" );
    shaderSourceCode = vertexShaderTemplate->render(&shaderTemplateContext).toStdString();

    LOG<< DEBUG_LOG_LEVEL << "VERTEX SHADER CODE:\n"<<  shaderSourceCode;
    writeToDisk(shaderSourceCode, VERTEX_SHADER_STAGE);

    //assert(0 && "inspecting shader code, therefore stop ;) ");

    //create the vertex shader:
    mShaderStages[VERTEX_SHADER_STAGE] =
    	new ShaderStage(VERTEX_SHADER_STAGE,shaderSourceCode,
   					mCodeDirectory, mShaderName);
    attachCompiledStage(VERTEX_SHADER_STAGE);


    //--------------------------------------------------------------------
    //TODO derive a condition where we need a geometry shader:
	//when do we need a geometry shader?
	//if we need to trender to a cubemap, an array texture or if wee need to render primitive IDs
    if(false)
    {
        //generate geom shader source code:

        contextMap["geom2fragInterfaceSpecifier"] ="out";
        shaderTemplateContext = Grantlee::Context(contextMap);
    	Grantlee::Template geomShaderTemplate = templateEngine->loadByName( "main.geom" );
        shaderSourceCode = geomShaderTemplate->render(&shaderTemplateContext).toStdString();

        LOG<< DEBUG_LOG_LEVEL << "GEOMETRY SHADER CODE:\n"<<  shaderSourceCode;
        writeToDisk(shaderSourceCode, GEOMETRY_SHADER_STAGE);
        //assert(0 && "inspecting shader code, therefore stop ;) ");

        //create the geometry shader:
        mShaderStages[GEOMETRY_SHADER_STAGE] =
        	new ShaderStage(GEOMETRY_SHADER_STAGE, shaderSourceCode,
        					mCodeDirectory, mShaderName);
        attachCompiledStage(GEOMETRY_SHADER_STAGE);
    }


    //--------------------------------------------------------------------

    //generate fragment shader source code:

    contextMap["geom2fragInterfaceSpecifier"] ="in";
    shaderTemplateContext = Grantlee::Context(contextMap);

	Grantlee::Template fragmentShaderTemplate = templateEngine->loadByName( "main.frag" );
    shaderSourceCode = fragmentShaderTemplate->render(&shaderTemplateContext).toStdString();

    LOG<< DEBUG_LOG_LEVEL << "FRAGMENT SHADER CODE:\n"<<  shaderSourceCode;
	writeToDisk(shaderSourceCode, FRAGMENT_SHADER_STAGE);
    //create the fragment shader:
    mShaderStages[FRAGMENT_SHADER_STAGE] =
    		new ShaderStage(FRAGMENT_SHADER_STAGE, shaderSourceCode,
    						mCodeDirectory, mShaderName);
    attachCompiledStage(FRAGMENT_SHADER_STAGE);

    //assert(0 && "inspecting shader code, therefore stop ;) ");

    //--------------------------------------------------------------------

    link();


}


//for later inspection of the final code of a stage:
void Shader::writeToDisk(String sourceCode, ShaderStageType type)
{
	String shaderDirectory=
		(	mCodeDirectory /
			  Path( mShaderName.string()+ String("_GENERATED.") + ShaderStageFileEndings[type] )
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
		mShaderName == rhs.mShaderName &&
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
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_DECAL_TEXTURING );
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_DETAIL_TEXTURING);
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_NORMAL_MAPPING );
	//sfl.shadingFeatures = ShadingFeatures( sfl.shadingFeatures | SHADING_FEATURE_CUBE_MAPPING);

	//sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT;

	//sfl.renderingTechnique= RENDERING_TECHNIQUE_SHADOWMAP_GENERATION;
	//sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT;
	//sfl.visualMaterialType = VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY;

	//END DEBUG


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


	contextMap.insert("inverse_lightSourcesFarClipPlane",
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
	static GLchar logBuffer[buffSize];

	GUARD( glValidateProgram(mGLProgramHandle) );

	GUARD(glGetProgramInfoLog(mGLProgramHandle,buffSize,&shaderInfoLogSize,logBuffer));


	String logFileName=
			mShaderName.string() +
			String("_shaderProgramInfoLog.txt");

	Path logFilePath= mCodeDirectory / Path(logFileName);
	std::fstream fileStream;
	fileStream.open(logFilePath.string().c_str(), std::ios::out);
	fileStream << logBuffer;
	fileStream.close();
}



//----------------------------------------------------------------------------------------------


ShaderStage::ShaderStage(ShaderStageType shaderStageType, String sourceCode, Path codeDirectory, Path shaderName)
:
		mType(shaderStageType),
		mSourceCode(sourceCode),
		mCodeDirectory(codeDirectory),
		mShaderName(shaderName)
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


	String logFileName=
			mShaderName.string() +
			String("_GENERATED_")+
			ShaderStageFileEndings[mType]+
			String("_shaderStageInfoLog.txt");

	Path logFilePath= mCodeDirectory / Path(logFileName);


	std::fstream fileStream;
	fileStream.open(logFilePath.string().c_str(), std::ios::out);
	fileStream << logBuffer;
	fileStream.close();
}

//============ uniform variables logic to come ==========================================================================


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
void Shader::setupMatrixUniforms(Camera *mainCam, SubObject* so)
{
	VisualMaterial* visMat = dynamic_cast<VisualMaterial*>(so->getMaterial());
	assert(visMat);

	/*
	    if( ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
		   != LIGHT_SOURCES_SHADOW_FEATURE_NONE )
	 * */

	Matrix4x4 viewMatrix = mainCam->getGlobalTransform().getLookAtMatrix();
	Matrix4x4 viewProjMatrix = mainCam->getProjectionMatrix() * viewMatrix;

	//check if we render to a special render target which will make geometry shader delegation
	//necessary and hence the main cam's view matrix obsolete
	//EDIT: afaik, trying to set a non existing uniform is no tragic thing;
	//maybe I should "bruteforce" try to set everything..? TODO checkout when stable
	if(
		! (
			//dyn. envmap rendering?
			  (mLocalShaderFeatures.renderTargetTextureType == TEXTURE_TYPE_2D_CUBE)
			//pointlight shadow map gen?
			||(mLocalShaderFeatures.renderTargetTextureType == TEXTURE_TYPE_2D_CUBE_DEPTH)
			//multiple spotlight shadow map gen?
			||(mLocalShaderFeatures.renderTargetTextureType == TEXTURE_TYPE_2D_ARRAY_DEPTH)
		)
	)
	{
		bindMatrix4x4("viewMatrix",viewMatrix);
		bindMatrix4x4("viewProjectionMatrix",viewProjMatrix);
	}
	else
	{

	}


	//instanced rendering or not?
	if( visMat->isInstanced())
	{
		assert(mLocalShaderFeatures.instancedRendering);
		InstancedGeometry* instancedGeo = dynamic_cast<InstancedGeometry*>(so->getGeometry());
		assert(instancedGeo);

		//setup the instance uniform buffer
		GUARD(
			glBindBufferBase(GL_UNIFORM_BUFFER,
				static_cast<GLuint>(INSTANCE_TRANSFORMATION_MATRICES_BUFFER_BINDING_POINT),
				instancedGeo->getInstanceManager()->getInstanceTransformationInfoUniformBuffer()->getGraphicsBufferHandle());
		);
		GUARD (GLuint uniformBlockIndex =  glGetUniformBlockIndex(mGLProgramHandle, "InstanceTransformInfoBuffer"));
		assert(uniformBlockIndex != GL_INVALID_INDEX);
		GUARD(glUniformBlockBinding(
				mGLProgramHandle,
				uniformBlockIndex,
				static_cast<GLuint>(INSTANCE_TRANSFORMATION_MATRICES_BUFFER_BINDING_POINT)
				)
		);
	}
	else
	{
		//note: when rendering to tex array or cube map, MV and MVP are unused;
		//but it doesn't matter to set them anyway; keep business logic as simple as possible ;(

		//setup default "model" related transform uniforms
		Matrix4x4 modelMatrix = so->getOwningWorldObject()->getGlobalTransform().getTotalTransform();
	    Matrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
	    Matrix4x4 modelViewProjMatrix = mainCam->getProjectionMatrix() * modelViewMatrix;

	    bindMatrix4x4("modelMatrix",modelMatrix);
	    bindMatrix4x4("modelViewMatrix",modelViewMatrix);
	    bindMatrix4x4("modelViewProjectionMatrix",modelViewProjMatrix);
	}
}



void Shader::setupLightSourceUniforms(Camera *mainCam)
{
	const ShaderFeaturesGlobal& sfg = ShaderManager::getInstance().getGlobalShaderFeatures();

	if(sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_NONE){return;}

	bindInt("numCurrentlyActiveLightSources",LightSourceManager::getInstance().getNumCurrentlyActiveLightingLightSources());
	bindFloat("invNumCurrentlyActiveLightSources",1.0f / static_cast<float>( LightSourceManager::getInstance().getNumCurrentlyActiveLightingLightSources()));

	//-----------------------------------------------------------------------------------

	if( (sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT)
	 ||	(sfg.lightSourcesLightingFeature == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
	)
	{
		const LightSourceShaderStruct & lsStruct = LightSourceManager::getInstance().getLightSource(0)->getdata();

	    Vector4D lightPosViewSpace =
	    		mainCam->getGlobalTransform().getLookAtMatrix()
	    		* Vector4D( LightSourceManager::getInstance().getLightSource(0)->getGlobalTransform().getPosition(),1.0f) ;
	   Vector4D lightDirViewSpace =
			   mainCam->getGlobalTransform().getLookAtMatrix()
			   * Vector4D( LightSourceManager::getInstance().getLightSource(0)->getGlobalTransform().getDirection(), 0.0f );

	   bindVector4D("lightSource.position",lightPosViewSpace);
	   bindVector4D("lightSource.diffuseColor",lsStruct.diffuseColor);
	   bindVector4D("lightSource.specularColor",lsStruct.specularColor);
	   bindVector4D("lightSource.direction",lightDirViewSpace);

	   bindFloat("lightSource.innerSpotCutOff_Radians",lsStruct.innerSpotCutOff_Radians);
	   bindFloat("lightSource.outerSpotCutOff_Radians",lsStruct.outerSpotCutOff_Radians);
	   bindFloat("lightSource.spotExponent",lsStruct.spotExponent);
	   bindFloat("lightSource.shadowMapLayer",lsStruct.shadowMapLayer);
	} //endif "only one light source for lighting"
	else
	{
		//setup the uniform buffer as we have several light sources
		GUARD(glBindBufferBase(GL_UNIFORM_BUFFER,
				static_cast<GLuint>(LIGHT_SOURCES_BUFFER_BINDING_POINT),
				LightSourceManager::getInstance().getLightSourceUniformBuffer()->getGraphicsBufferHandle());
		);
		GUARD (GLuint uniformBlockIndex =  glGetUniformBlockIndex(mGLProgramHandle, "LightSourceBuffer"));
		assert(uniformBlockIndex != GL_INVALID_INDEX);
		GUARD(glUniformBlockBinding(
				mGLProgramHandle,
				uniformBlockIndex,
				static_cast<GLuint>(LIGHT_SOURCES_BUFFER_BINDING_POINT)
				)
		);
	}
}



void Shader::setupMaterialUniforms(VisualMaterial* visMat)
{
	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_DECAL_TEXTURING ) !=0 ))
	{
		glActiveTexture(GL_TEXTURE0 + DECAL_COLOR_SEMANTICS);
		visMat->getTexture(DECAL_COLOR_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("decalTexture",DECAL_COLOR_SEMANTICS);
	}
	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_DETAIL_TEXTURING ) !=0 ))
	{
		glActiveTexture(GL_TEXTURE0 + DETAIL_TEXTURE_SEMANTICS);
		visMat->getTexture(DETAIL_TEXTURE_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("detailTexture",DETAIL_TEXTURE_SEMANTICS);
	}
	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_NORMAL_MAPPING) !=0 ))
	{
		glActiveTexture(GL_TEXTURE0 + NORMAL_SEMANTICS);
		visMat->getTexture(NORMAL_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("normalMap",NORMAL_SEMANTICS);
	}
	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_CUBE_MAPPING ) !=0 ))
	{
		glActiveTexture(GL_TEXTURE0 + ENVMAP_SEMANTICS);
		visMat->getTexture(ENVMAP_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("cubeMap",ENVMAP_SEMANTICS);
	}

	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_AMBIENT_OCCLUSION ) !=0 ))
	{
		//note: bind DEPTH_BUFFER_SEMANTICS instead of AMBIENT_OCCLUSION_SEMANTICS, as AMBIENT_OCCLUSION_SEMANTICS
		//contains the AO result and not the raw image for computation
		assert(0&& "sry AO not implemented yet"); //TODO
		glActiveTexture(GL_TEXTURE0 + DEPTH_BUFFER_SEMANTICS);
		visMat->getTexture(DEPTH_BUFFER_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);
		bindInt("depthBufferForAO",DEPTH_BUFFER_SEMANTICS);
	}
}


}
