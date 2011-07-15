/*
 * SkyDomeShader.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#include "SkyDomeShader.h"

#include "Simulator/ParallelComputeManager.h"
#include "Material/VisualMaterial.h"
#include "WorldObject/SubObject.h"
#include "URE.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"

namespace Flewnit {


SkyDomeShader::SkyDomeShader(Path codeDirectory,
		const ShaderFeaturesLocal & sfl
		//, TextureType renderTargetTextureType
		)
:
		Shader(
			codeDirectory,
			Path("GenericLightingUberShader"), //yes, the sky dome rending is also included in the ubershader
			sfl,
			String("SkyDomeShader")
		)

//	ShaderFeaturesLocal(
//		RENDERING_TECHNIQUE_DEFAULT_LIGHTING,
//		renderTargetTextureType,
//		VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING,
//		SHADING_FEATURE_CUBE_MAPPING,
//		false)
{
	build();
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
	GUARD(glUseProgram(mGLProgramHandle));
	setupTransformationUniforms(so);

	VisualMaterial * visMat =  dynamic_cast<VisualMaterial*>(so->getMaterial());
	assert(visMat);
	setupMaterialUniforms(visMat);
	//---------- uncategorized uniforms to come ------------------------------------------------
	bindVector3D("eyePositionW",URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getPosition());
}

//virtual void generateCustomDefines();
void SkyDomeShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
	assert(0 &&  "SkyDomeShader::bindFragDataLocations; implement me :P");
}


}
