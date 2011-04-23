/*
 * SkyDomeMaterial.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#include "SkyDomeMaterial.h"

#include "Simulator/ParallelComputeManager.h"
#include "MPP/Shader/Shader.h"
#include "Buffer/Texture.h"


namespace Flewnit
{

SkyDomeMaterial::SkyDomeMaterial(String name, Texture2DCube* cubeTex)
: VisualMaterial(
		name,
		VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING,
		SHADING_FEATURE_CUBE_MAPPING,
		//must contain at least the textures used in the shader as samplers
		std::map<BufferSemantics, Texture*>{std::pair<BufferSemantics, Texture*>(ENVMAP_SEMANTICS, cubeTex)},
		VisualMaterialFlags(
			//NO shadow casting, NOT transparent,
			false,false,
			//YES shadable (actually not, but as functionality is integrated in ubershader and defaultlighting stage,
			//we have to set this flag in order to not mask out the sky dome),
			true,
			//NOT instanced,
			//YES dyncubemaprenderable,NOT custom (is special, but can still
			//be handled by generic lighting stages)
			true,false)
		)
{

}

SkyDomeMaterial::~SkyDomeMaterial()
{

}

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
bool SkyDomeMaterial::operator==(const Material& rhs) const
{
	return (
		dynamic_cast<const SkyDomeMaterial*>(&rhs)
		&&
		VisualMaterial::operator==(rhs)
	);
}

void SkyDomeMaterial::activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//GUARD(glDisable(GL_DEPTH_TEST));
	GUARD(glCullFace(GL_FRONT));

	getCurrentlyUsedShader()->use(currentUsingSuboject);
}
void SkyDomeMaterial::deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//GUARD(glEnable(GL_DEPTH_TEST));
	GUARD(glCullFace(GL_BACK));
}

}
