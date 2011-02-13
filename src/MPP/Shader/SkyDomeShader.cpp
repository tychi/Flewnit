/*
 * SkyDomeShader.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#include "SkyDomeShader.h"

namespace Flewnit {


SkyDomeShader::SkyDomeShader(Path codeDirectory, TextureType renderTargetTextureType)
:
		Shader(codeDirectory, Path("SkyDomeShader"),
				ShaderFeaturesLocal(
						RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
						renderTargetTextureType,
						VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING,
						SHADING_FEATURE_CUBE_MAPPING,
						false)
		)
{
	//nothing to assert, as the restricted constructor parameters assure that the user
	//cannot abuse this shadertype via strange parameters;

}

SkyDomeShader::~SkyDomeShader()
{
	//do nothing
}

//virtual bool operator==(const Shader& rhs)const;

void SkyDomeShader::use(SubObject* so)throw(SimulatorException)
{
	//TODO implement
	assert(0 &&  "SkyDomeShader::use; implement me :P");
}

//virtual void generateCustomDefines();
void SkyDomeShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
	assert(0 &&  "SkyDomeShader::bindFragDataLocations; implement me :P");
}


}
