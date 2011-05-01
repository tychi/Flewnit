/*
 * ParticleLiquidVisualMaterial.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#include "ParticleLiquidVisualMaterial.h"
#include "Util/Log/Log.h"

namespace Flewnit {

ParticleLiquidVisualMaterial::ParticleLiquidVisualMaterial(String name,
		const Vector4D & liquidColor, float particleDrawRadius,
		const Vector4D & foamColor, float foamGenerationAcceleration,
		float shininess,
		float reflectivity,
		float refractivity, //"brechungsindex", for Fresnel effect
		unsigned int numCurvatureFlowRelaxationSteps
//		Texture* noiseTexture,
//		Texture2DCube* envmap
		)
	//HAXX
	: DebugDrawVisualMaterial(
			name,
			liquidColor,
			false, /*not instanced!*/
			VERTEX_BASED_POINT_CLOUD )

{
	LOG<<WARNING_LOG_LEVEL<<"Instanced haxxored faked  ParticleLiquidVisualMaterial as DebugDrawVisualMaterial!!1 "
			"TODO replace by real sophisitcated shading! \n";
}


ParticleLiquidVisualMaterial::~ParticleLiquidVisualMaterial()
{

}


//check for equality in order to check if a material with the desired properties
//(shader feature set and textures) already exists in the ResourceManager;
//bool ParticleLiquidVisualMaterial::operator==(const Material& rhs) const
//{
//
//}


//void ParticleLiquidVisualMaterial::activate(
//		SimulationPipelineStage* currentStage,
//		SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//	assert(0 && "TODO implement");
//}
//
//void ParticleLiquidVisualMaterial::deactivate(SimulationPipelineStage* currentStage,
//		SubObject* currentUsingSuboject) throw(SimulatorException)
//{
//	assert(0 && "TODO implement");
//}


}
