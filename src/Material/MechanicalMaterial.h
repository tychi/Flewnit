/*
 * MechanicalMaterial.h
 *
 * Base class for all mechanical materials;
 *
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once


#include "Material/Material.h"
#include "Common/AmendedTransform.h"



namespace Flewnit
{

class MechanicalMaterial
	: public Material
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	MechanicalMaterial(String name, float mass);
	MechanicalMaterial(String name, float mass, float friction, float restitution);

	virtual ~MechanicalMaterial(String name, float mass);

	virtual bool operator==(const Material& rhs) const = 0;

	//keep it pure
	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException)=0;
	//undoing stuff, like re-enable depth test etc.
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException)=0;

protected:

	float mMass;


    float mFriction; //unused atm (april 2011), but keep in mind;
    float mRestitution; //bumpyness; small: inelastic collision; big: elastic collision; unused atm (april 2011), but keep in mind;

};


}



	//TODO delete legacy code
    //because centre of mass most often is not the same as the origin of the to-be-visualized geometry,
    //and because there might be a scale factor there is an offset transform computed
    //for any mechanical representaion of a WorldObject:
    // WorldObject::mGlobalTransform =
    //		MechanicalMaterial::mSceneGraphOffsetTransform *
    //		MechanicalMaterial::mMechanicalWorldTransform
    // --> mSceneGraphOffsetTransform = AmendedTransform(
    //			initialCentreOfMass, Vector3D(0.0f,.0f,-1.0f), 1.0f/particlei
    //AmendedTransform mSceneGraphOffsetTransform;




