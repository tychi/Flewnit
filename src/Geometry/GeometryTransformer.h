/*
 * GeometryTransformer.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */


#pragma once


#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"

namespace Flewnit
{

class GeometryTransformer
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:

	GeometryTransformer();

	virtual ~GeometryTransformer();

	/*
	 *  input: Geometry, should be a triangle mesh with position vertices and
	 *  	   index buffer;
	 *
	 *  output: ParticleizedRigidBody containing a buffer containing
	 *  		mNumMaxParticlesPerRigidBody Vector4D's,
	 *  		where the first numParticles vectors represent positions of the non-empty
	 *  		voxels, and the rest of the vectors is ware zero;
	 *
	 *	Note:
	 *		- parameters like represented volumes of a RB particle are
	 *		  global params, because we wanna omit both oversampling and sponges;
	 *		- no scaling suported to stay as simple as possible;
	 *		  scale geometry position values directly (and compensate by scene node scale)
	 *		  before calling this routine if you need to deviate from the default mesh scale;
	 *
	 *	Tip:
	 *		  In a "physically based unified rendering engine", decoupling a scale from actual
	 *		  physical properties causes headaches;
	 *  	  Omit any scaling via transforms everywhere if possible, above all after initializing
	 *  	  the mechanical components; Apply all transform scales to the position buffers
	 *		  (as this framework allows only consistent scale over all dimensions, the normals
	 *		  won't be harmed); Of course you have to make sure that objects with different scales
	 *		  do not use the same buffer, and that a scale is applied only once per shared buffer;
	 *
	 *	NOTE: not implemented yet (april 21, 2011) TODO
	 *
	 *
	 *	IMPORTANT FOR IMPLEMENTATION :
	 *		Because updateRigidBody.cl performs parallel scans and hence each work item treats two particles,
	 *	 	the number of paticles MUST be even!! the side effects of one "forgotten" or even worse - missread
	 *		particles could blow up the wohl simulation; the control flow to catch this in a kernel is way too costly
	 *		(at least three more if()'s), so ensure that the particleization handles this, e.g. by ignoring the last particle
	 *		if its index is even... this could result in a slightly "odd" (what a  word wit ;( ) simulation behaviour,
	 *		but won't blow up!
	 *
	 * */
	ParticleRigidBody* particleizeGeometry(
			Geometry* geomToParticleize, //usually a VertexBasedGeometry to be voxelized
										//and then transformed in to a particle cloud
			float massPerParticle,
			//caution: read out this value  from
			//ParticleSceneRepresentation::mVoxelSideLengthRepresentedByRigidBodyParticle,
			//because this must be the same for all particleized rigid objects
			//for stability and performance reasons;
			float sideLengthOfOneVoxel,
			uint numMaxParticles
	 	 ) throw(BufferException);


	/*
	 *	TODO
	 * */
	BufferInterface* voxelizeGeometry(
		Geometry* geomToVoxelize, //usually a VertexBasedGeometry to be voxelized
								 	//and then transformed in to a particle cloud
		//caution: read out this value  from
		//ParticleSceneRepresentation::mVoxelSideLengthRepresentedByRigidBodyParticle,
		//because this must be the same for all particleized rigid objects
		//for stability and performance reasons;
		float sideLengthOfOneVoxel,
		//out param
		Vector3Dui& voxelGridDimensions
		) throw(BufferException);

	/*
	 *	TODO
	 * */
	//void createUniformGridTriangleLists(...);


	/*
	 * "procedural" box generator to test rigid body stuff without working voxelization of arbitrary
	 * watertight meshes;
	 * */
	ParticleRigidBody* createBoxParticleizedRigidBody(
			BoxGeometry* boxGeomToParticleize, //usually a VertexBasedGeometry to be voxelized
										//and then transformed in to a particle cloud

			VisualMaterial* visMat,
			uint numMaxParticles,
			ParticleRigidBodyMechMat* ptclRBMechMat,
			//caution: read out this value  from
			//ParticleSceneRepresentation::mVoxelSideLengthRepresentedByRigidBodyParticle,
			//because this must be the same for all particleized rigid objects
			//for stability and performance reasons;
			float sideLengthOfOneVoxel
	 	 ) throw(BufferException);

private:

	//TODO some render targets etc..

};


}
