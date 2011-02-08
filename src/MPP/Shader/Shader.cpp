/*
 * Shader.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "Shader.h"

#include "Simulator/OpenCL_Manager.h"
#include "MPP/Shader/ShaderManager.h"

#include "Util/Log/Log.h"



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
    //TODO uncomment when vertex shader stage is derived from needs of the frsagment shder stage;

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
    	Grantlee::Template geomShaderTemplate = templateEngine->loadByName( "main.geom" );
        shaderSourceCode = geomShaderTemplate->render(&shaderTemplateContext).toStdString();

        LOG<< DEBUG_LOG_LEVEL << "GEOMETRY SHADER CODE:\n"<<  shaderSourceCode;
        assert(0 && "inspecting shader code, therefore stop ;) ");

        //create the geometry shader:
        mShaderStages[GEOMETRY_SHADER_STAGE] = new ShaderStage(GEOMETRY_SHADER_STAGE, shaderSourceCode);
    }


    //--------------------------------------------------------------------

    //generate fragment shader source code:
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




void Shader::setupTemplateContext(TemplateContextMap& context)
{
	//test values; TODO setup according to shaderfeatures struct
	context.insert("name", "Wayne intressierts");
	context.insert("renderingTechniqueDefaultLighting",true);
	context.insert("renderingTechniqueGBufferFill",true);

	context.insert("shadingFeatureDecalTexturing",true);
	context.insert("shadingFeatureNormalMapping",true);
	context.insert("shadingFeatureCubeMapping",true);

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
