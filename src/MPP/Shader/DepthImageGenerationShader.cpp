/*
 * DepthImageGenerationShader.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#include "DepthImageGenerationShader.h"

namespace Flewnit {




DepthImageGenerationShader::DepthImageGenerationShader(
		Path codeDirectory,
		RenderingTechnique renderingTechnique,
		TextureType renderTargetTextureType,
		bool instancedRendering)
:
		Shader(codeDirectory, Path("DepthImageGenerationShader"),
				ShaderFeaturesLocal(
						renderingTechnique,
						renderTargetTextureType,
						VISUAL_MATERIAL_TYPE_NONE,SHADING_FEATURE_NONE,
						instancedRendering)
		)
{
	//nothing to assert, as the restricted constructor parameters assure that the user
	//cannot abuse this shadertype via strange parameters;

}

DepthImageGenerationShader::~DepthImageGenerationShader()
{
	//do nothing
}

//virtual bool operator==(const Shader& rhs)const;

void DepthImageGenerationShader::use(SubObject* so)throw(SimulatorException)
{
	//TODO implement
	assert(0 &&  "DepthImageGenerationShader::use; implement me :P");
}

//virtual void generateCustomDefines();
void DepthImageGenerationShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
	assert(0 &&  "DepthImageGenerationShader::bindFragDataLocations; implement me :P");
}

}
