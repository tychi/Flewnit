/*
 * ParticleLiquidVisualMaterial.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once


#include "Material/VisualMaterial.h"

//HAXX
#include "Material/DebugDrawVisualMaterial.h"

namespace Flewnit
{

class ParticleLiquidVisualMaterial
	//OMG HAXX: due to time pressure, thie material is at the moment just a renamed debug draw mat;
	//sophisitcated rendering comes when the mech. basics work;
	:public DebugDrawVisualMaterial
	//TODO restore
	//: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	ParticleLiquidVisualMaterial(String name,
			const Vector4D & liquidColor, float particleDrawRadius,
			const Vector4D & foamColor, float foamGenerationAcceleration,
			float refractivity, //"brechungsindex", for Fresnel effect
			float shininess,
			float reflectivity,
			unsigned int numCurvatureFlowRelaxationSteps
			//Texture* noiseTexture, <-- perlin noise is procedurally generated.. isn't it?
			//Texture2DCube* envmap <-- grab globally managed one
			);
	virtual ~ParticleLiquidVisualMaterial();

	//HAXX let the debug draw mat do all the work ;(

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	//virtual bool operator==(const Material& rhs) const;

	//	virtual void activate(
	//			SimulationPipelineStage* currentStage,
	//			SubObject* currentUsingSuboject) throw(SimulatorException);
	//	virtual void deactivate(SimulationPipelineStage* currentStage,
	//			SubObject* currentUsingSuboject) throw(SimulatorException);

};

}


