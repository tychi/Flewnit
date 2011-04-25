/*
 * ParticleMechanicsStage.h
 *
 * Mechanical Simulations on particle-based objects are performed here
 * with OpenCL;
 *
 * The CL PRograms have been optimized to my best knowlegde,
 * except in matters of memory footprint and bandwidth;
 * To optimized those two domains, values with no direct semantic reference have to get packed
 * together, alignment and numerical stability has to give way to tight packing;
 * This stuff causes headaches, and hence has not been performed so far, as there shall be no elevated
 * risk of blowing the sim for numerical reasons while still developing;
 *
 * Theoretical presumption:
 * At least on non-high end devices, the performance of this simulation is bandwidth-limited;
 * Solely the acquisition of neighbor particles without any management overhead costs
 * around
 * (27 grid cells * 32 particles per cell  *12 floats per particle *4 Bytes per float *
 *   (2^18/8)non-empty Grid cells ) = 1.36 Gigabyte per single SPH step;
 *
 * Including the rest of the attribute transfers, the maintainance overhead (uniform grid lookup etc),
 * the many radix sort passes and phases (usually 9), the many scattered reads during the reorder phase,
 * and finally the grpahical rendering, we may come close or even exceed the 2 GB/frame bandwidth usage;
 *
 * Given that this thesis is coded on a notebook with a Geforce GT 435m GPU having 25.6 GB/s
 * memory bandwith, there will be no framerate higher than 25.6/2 = 128 fps solely for bandwith reasons;
 * After eastern, I'm gonna buy a GTX570 with 6 times the bandwidth (152 GB/s), I'm curious about its
 * performance;
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"


namespace Flewnit
{

class ParticleMechanicsStage
	: public SimulationPipelineStage
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	ParticleMechanicsStage(ConfigStructNode* simConfigNode);
	virtual ~ParticleMechanicsStage();

	virtual bool stepSimulation() throw(SimulatorException);
	virtual bool initStage()throw(SimulatorException);
	virtual bool validateStage()throw(SimulatorException);


	inline ParticleSceneRepresentation* getParticleSceneRepresentation()const{return mParticleSceneRepresentation;}

private:

	//{ Buffers flushed to CL device at beginning of eacht simulations step
	Buffer* mSimulationParametersBuffer;

	uint mNumMaxUserForceControlPoints;
	Buffer* mUserForceControlPointBuffer;
	//}


	//There could come to one's mind to manage Scene representations and accell.
	//structures other than the classic scene graph also centrally by the
	//SimulationResourceManager;
	//Pros: - every class can access stuff conveniently
	//Cons: - Scene maintaining code in SimulationResourceManager gets more complex;
	//		- There could be several scene representations and acc. structures
	//		  of same type but in different usage combinations;
	//		  This would become way too complicated and it won't be used anyway in the
	//		  near future;
	//--> This stage maintains its own scene representation
	ParticleSceneRepresentation* mParticleSceneRepresentation;

	//{
		UniformGrid* mParticleUniformGrid;
		//for later ;)
		//UniformGrid* mStaticTriangleUniformGrid;
		UniformGridBufferSet* mSplitAndCompactedUniformGridCells;
		//value implying how many work groups we need for SPH particle physics simulation Kernels;
		unsigned int mNumCurrentSplitAndCompactedUniformGridCells;
	//}



};

}

