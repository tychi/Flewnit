/*
 * ParticleRigidBody.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#pragma once

#include "WorldObject/WorldObject.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
//include the OpenCL data structures to omit boilerplate and ensure synch,
//as the structures are subject to change
#include "MPP/OpenCLProgram/ProgramSources/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE


namespace Flewnit
{

class ParticleRigidBody
	: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	ParticleRigidBody(String name, const AmendedTransform& localtransform,
			//the created object has to register to this scene
			ParticleSceneRepresentation* particleSimulationDomain,

			//visual stuff
			std::vector<SubObject*>& visualSubObjects,
			//will be transformed into "mechanical domain" geometry with a position semantics attribute buffer
			Buffer* relativePositionsBuffer, unsigned int numParticles,
			ParticleRigidBodyMechMat* mechMat);
	virtual ~ParticleRigidBody();

	//assign mechanical transform to global scene graph transform;
	void synchronizeTransforms();

protected:

	unsigned int mNumParticles;

	//make friend to save getters and setters
	friend class ParticleSceneRepresentation;
		//assigned on registration at ParticleSceneRepresentation in order to associate itself
		//with GPU buffer indices etc;
		unsigned int mParticleRigidBodyID;
		unsigned int mOffsetInIndexTableBuffer;

    //to be synch'ed with scenegraph transform (maybe with a scale and centre of mass-position-offset)
    //after every simulation step
    AmendedTransform mMechanicalWorldTransform;

	Vector4D linearVelocity;
    Vector4D angularVelocity;


    //WorldObject::mGlobalTransform =
    //		MechanicalMaterial::mMechanicalWorldTransform * translate(- mLocalCentreOfMass);
    Vector4D mLocalCentreOfMass;
    //1/(sum( length(centreOfMassRelParticlePos)^2 )); Needed for angular vel. approximation;
    float mInverseTotalSquaredDistancesFromCenterOfMass;
};


}
