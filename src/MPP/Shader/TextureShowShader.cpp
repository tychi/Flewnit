/*
 * TextureShowShader.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */

#include "TextureShowShader.h"

#include "MPP/Shader/ShaderManager.h"
#include "Simulator/ParallelComputeManager.h"
#include "Buffer/Texture.h"
#include "MPP/MPP.h"
#include "Buffer/BufferHelperUtils.h"

#include <grantlee/engine.h>
#include "UserInterface/WindowManager/WindowManager.h"



namespace Flewnit
{

	TextureShowShader::TextureShowShader(TextureType texType)
	:
	  Shader(ShaderManager::getInstance().getShaderCodeDirectory(), Path("TextureShow"),
			ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,texType,VISUAL_MATERIAL_TYPE_NONE,SHADING_FEATURE_NONE,false)
	  ),
	  mTextureType(texType)
	{

	}

	TextureShowShader::~TextureShowShader()
	{

	}




	void TextureShowShader::build()
	{
		initBuild();

		//TODO
		//assert(0&&"TextureShowShader::build(): implement me");

		//build vert and frag shader
		generateShaderStage(VERTEX_SHADER_STAGE,mTemplateEngine,*mTemplateContextMap);
		generateShaderStage(FRAGMENT_SHADER_STAGE,mTemplateEngine,*mTemplateContextMap);

		finishBuild();

	}

	void TextureShowShader::setupTemplateContext(TemplateContextMap& contextMap)
	{
		contextMap.insert("GL_MAYOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().x);
		contextMap.insert("GL_MINOR_VERSION", WindowManager::getInstance().getAvailableOpenGLVersion().y);

		for(unsigned int i = 0; i < __NUM_TOTAL_SEMANTICS__;i++)
		{
			//add the numeric value of the semantics enum in order to define the layout qualifier for the vertex shader
			contextMap.insert(
					BufferHelper::BufferSemanticsToString(BufferSemantics(i)).c_str(),
					i
			);
		}


		//TODO make shader source and context dependent from texture type to show

		for(unsigned int i = 0; i < __NUM_TEXTURE_TYPES__;i++)
		{
			contextMap.insert(
					(String("TEXTURE_SHOW_TYPE_") + TextureTypeStrings[i]).c_str(),
					(bool) ( (unsigned int)(mTextureType) == i)
			);
		}


	}

	void TextureShowShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
	{

	}


	void TextureShowShader::use(Texture* tex, float layerOrTexcoord)
	{
		//assert(0&&"TextureShowShader::build not implemented yet, hence must fail; TODO");
		assert("TextureShowShader::use(): type of passed Texture and type of initially defined must fit"
				&& ( mTextureType == tex->getTextureType()) );

		GUARD(glUseProgram(mGLProgramHandle));

		//bind texture to sampler
		GUARD(glActiveTexture(GL_TEXTURE0 ));
		GUARD(tex->bind(OPEN_GL_CONTEXT_TYPE));
		bindInt("textureToShow",0);

		//setup orthogonal projection matrix to unit cube:
		//(model and view matrix are the identity here and hence can be ignored)
		bindMatrix4x4("modelViewProjectionMatrix",
				glm::gtc::matrix_transform::ortho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f));


	}

}
