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


#include <grantlee/engine.h>
#include "Buffer/BufferHelperUtils.h"

//#include "GrantleeShaderFeaturesContext.h"

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

//	QVariant shaderFeaturesVariant= QVariant::fromValue(reinterpret_cast<QObject*>( mGrantleeShaderFeaturesContext));
//	contextMap.insert("shaderFeatures",shaderFeaturesVariant);
//	contextMap.insert("renderingTechniqueDefaultLighting",true);
//
//    QObject *object = new MyClass();
//    contextMap.insert("myObj", QVariant::fromValue(object));



    Grantlee::Context shaderTemplateContext(contextMap);
    String shaderSourceCode;

    //--------------------------------------------------------------------

    //generate vertex shader source code:
    //TODO uncomment when vertex shader stage is derived from needs of the frsagment shder stage;
    contextMap["geom2fragInterfaceSpecifier"] ="out";
    shaderTemplateContext = Grantlee::Context(contextMap);
//    Grantlee::Template vertexShaderTemplate = templateEngine->loadByName( "main.vert" );
//    shaderSourceCode = vertexShaderTemplate->render(&shaderTemplateContext).toStdString();
//
//    LOG<< DEBUG_LOG_LEVEL << "VERTEX SHADER CODE:\n"<<  shaderSourceCode;
//
//    assert(0 && "inspecting shader code, therefore stop ;) ");
//
//    //create the vertex shader:
//    mShaderStages[VERTEX_SHADER_STAGE] = new ShaderStage(VERTEX_SHADER_STAGE,shaderSourceCode);


    //--------------------------------------------------------------------
    //TODO derive a condition where we need a geometry shader:
    if(false)
    {
        //generate geom shader source code:

        contextMap["geom2fragInterfaceSpecifier"] ="out";
        shaderTemplateContext = Grantlee::Context(contextMap);
    	Grantlee::Template geomShaderTemplate = templateEngine->loadByName( "main.geom" );
        shaderSourceCode = geomShaderTemplate->render(&shaderTemplateContext).toStdString();

        LOG<< DEBUG_LOG_LEVEL << "GEOMETRY SHADER CODE:\n"<<  shaderSourceCode;
        assert(0 && "inspecting shader code, therefore stop ;) ");

        //create the geometry shader:
        mShaderStages[GEOMETRY_SHADER_STAGE] = new ShaderStage(GEOMETRY_SHADER_STAGE, shaderSourceCode);
    }


    //--------------------------------------------------------------------

    //generate fragment shader source code:

    contextMap["geom2fragInterfaceSpecifier"] ="in";
    shaderTemplateContext = Grantlee::Context(contextMap);

	Grantlee::Template fragmentShaderTemplate = templateEngine->loadByName( "main.frag" );
    shaderSourceCode = fragmentShaderTemplate->render(&shaderTemplateContext).toStdString();

    LOG<< DEBUG_LOG_LEVEL << "FRAGMENT SHADER CODE:\n"<<  shaderSourceCode;

    assert(0 && "inspecting shader code, therefore stop ;) ");

    //create the geometry shader:
    mShaderStages[GEOMETRY_SHADER_STAGE] = new ShaderStage(FRAGMENT_SHADER_STAGE, shaderSourceCode);




    //--------------------------------------------------------------------



	//TODO coninue

	//when do we need a geometry shader?
	//if we need to trender to a cubemap, an array texture or if wee need to render primitive IDs
}


void Shader::attachCompiledStage(ShaderStageType which)
{
	GUARD(glAttachShader(mGLProgramHandle, mShaderStages[which]->mGLShaderStageHandle));
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

	sfl.renderingTechnique= RENDERING_TECHNIQUE_SHADOWMAP_GENERATION;
	sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT;
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

	float tangensCamFov;
	float cameraFarClipPlane;

	if(sfl.renderingTechnique == RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
	{
		cameraFarClipPlane = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
				getLightSourceManager()->getLightSourceProjectionMatrixFarClipPlane();

		tangensCamFov = 1.0f; //tangens plays no role in shadow map generation;
	}
	else
	{
		cameraFarClipPlane = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
				getMainCamera()->getFarClipPlane();

		tangensCamFov =
			glm::tan(
				glm::radians(
					URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->
						getMainCamera()->getVerticalFOVAngle()
				)
			);
	}

	contextMap.insert("cameraFarClipPlane", cameraFarClipPlane);
	contextMap.insert("invCameraFarClipPlane", 1.0f / cameraFarClipPlane );

	contextMap.insert("tangensCamFov", tangensCamFov);
	contextMap.insert("cotangensCamFov", 1.0f / tangensCamFov );

}



void Shader::link()
{
	GUARD(glLinkProgram(mGLProgramHandle));
	validate();
}



//check compiled and linked programm for errors
void Shader::validate()throw(BufferException)
{
	//TODO
}




//----------------------------------------------------------------------------------------------


ShaderStage::ShaderStage(ShaderStageType shaderStageType, String sourceCode)
:
		mScourceCode(sourceCode)
		//mCodeDirectory(codeDirectory),
		//mShaderName(shaderName)
{
	GUARD( mGLShaderStageHandle = glCreateShader(mGLShaderStageIdentifiers[shaderStageType]));
}


ShaderStage::~ShaderStage()
{
	GUARD(glDeleteShader(mGLShaderStageHandle););
}




void ShaderStage::compile()
{
	GUARD(glCompileShader(mGLShaderStageHandle));
}


//for later inspection of the final code of a stage:
void ShaderStage::writeToDisk()
{
	//TODO
}


void ShaderStage::validate()throw(BufferException)
{

}


////throw exception if file for code does not exist or if the section does not apply
////to the shader type (e.g. customizable #defines cannot be loaded but only generated on the fly)
//void ShaderStage::loadCodeSection(ShaderCodeSectionID which )throw(SimulatorException)
//{
//
//}
//
//
//void ShaderStage::propagateLoadedSourceToGL()
//{
//
//}

}
