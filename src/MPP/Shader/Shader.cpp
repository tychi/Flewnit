/*
 * Shader.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "Shader.h"

#include "Simulator/OpenCL_Manager.h"
#include "MPP/Shader/ShaderManager.h"

//#include <grantlee_core.h>
#include <grantlee/engine.h>
#include "Util/Log/Log.h"

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

	//create the vertex shader:
	mShaderStages[VERTEX_SHADER_STAGE] = new ShaderStage(VERTEX_SHADER_STAGE,mCodeDirectory,mShaderName);

    Grantlee::Engine *engine = new Grantlee::Engine(  );
    Grantlee::Template t = engine->newTemplate("My name is {{ name }}.", "my_template_name");
    QVariantHash mapping;
    mapping.insert("name", "Grainne");
    Grantlee::Context c(mapping);
    LOG<< DEBUG_LOG_LEVEL << t->render(&c).toStdString(); // Returns "My name is Grainne."


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




void Shader::generateCustomDefines()
{

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


ShaderStage::ShaderStage(ShaderStageType shaderStageType, Path codeDirectory, Path shaderName)
:
		mCodeDirectory(codeDirectory),
		mShaderName(shaderName)
{
	GUARD( mGLShaderStageHandle = glCreateShader(mGLShaderStageIdentifiers[shaderStageType]));
}


ShaderStage::~ShaderStage()
{
	GUARD(glDeleteShader(mGLShaderStageHandle););
}


//throw exception if file for code does not exist or if the section does not apply
//to the shader type (e.g. customizable #defines cannot be loaded but only generated on the fly)
void ShaderStage::loadCodeSection(ShaderCodeSectionID which )throw(SimulatorException)
{

}


void ShaderStage::propagateLoadedSourceToGL()
{

}


void ShaderStage::compile()
{
	GUARD(glCompileShader(mGLShaderStageHandle));
}


//for later debugging of the final code of a stage:
void ShaderStage::writeToDisk()
{

}


void ShaderStage::validate()throw(BufferException)
{

}



}
