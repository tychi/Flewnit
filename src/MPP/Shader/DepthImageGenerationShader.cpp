/*
 * DepthImageGenerationShader.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#include "DepthImageGenerationShader.h"
#include "Simulator/ParallelComputeManager.h"
#include "WorldObject/SubObject.h"
#include "Material/VisualMaterial.h"

namespace Flewnit {




DepthImageGenerationShader::DepthImageGenerationShader(
		Path codeDirectory,
		const ShaderFeaturesLocal& sfl
//
//		RenderingTechnique renderingTechnique,
//		VisualMaterialType visMatType,
//		TextureType renderTargetTextureType,
//		bool forTessellation,
//		bool instancedRendering
		)
:
		Shader(codeDirectory,
				//even the shadow map gen is included in the uber shader:
				Path("GenericLightingUberShader"),
				//Path("DepthImageGenerationShader"),
				sfl,
				String("DepthImageGenerationShader")
		)
{
	build();

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
	GUARD(glUseProgram(mGLProgramHandle));

	//---------------------------------------------------------
	setupTransformationUniforms(so);
	//----------------------------------------------------------
	VisualMaterial * visMat =  dynamic_cast<VisualMaterial*>(so->getMaterial());
	assert(visMat);
	//setupTessellationParameters() returns doing nothing if tess is not enablled globally or in this mat
	setupTessellationParameters(visMat);
	//---------- uncategorized uniforms to come ------------------------------------------------
	//bindVector3D("eyePositionW",URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getPosition());

	//TODO check if the upper impl. works ;(
}

//virtual void generateCustomDefines();
void DepthImageGenerationShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	assert(0 &&  "DepthImageGenerationShader::bindFragDataLocations: no frag data locs to bind as rendering only to depth buffers ;(");
}

}
