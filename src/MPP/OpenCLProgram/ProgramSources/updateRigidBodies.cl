  /*
  
    Kernel to calculate the "macroscopic" attributes of the rigid bodies with help of the results
    from the previous particle integrations;
  
    Formulas are taken from the Diploma thesis of Thomas Steil, "Efficient Methods for Computational Fluid Dynamics and Interactions",
    §3.2.4 "Rigid bodies in particle system";
    
  */

  
  #pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
  #pragma  OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
  #pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable    
      
    
   {% include "scan.cl" %}
   
  //hardware dependent memory amounts determine how many radix counters a work group can own
  {% ifequal nvidiaComputeCapabilityMajor "2" %}
    //reason: 48kB local memory available; 9 "half scans" to do; we need 
    //PADDED_STRIDE( NUM_MAX_PARTICLES_PER_RIGID_BODY * 4 * 9) bytes for this "multiple scan array";
    //with NUM_MAX_PARTICLES_PER_RIGID_BODY == 1024, this yields 38016 bytes <-- fits;
    #define NUM_MAX_PARTICLES_PER_RIGID_BODY (1024)
  {% endifequal %}
  {% ifequal nvidiaComputeCapabilityMajor "1" %}
    //reason: 16kB local memory available; 9 "half scans" to do; we need 
    //PADDED_STRIDE( NUM_MAX_PARTICLES_PER_RIGID_BODY * 4 * 9) bytes for this "multiple scan array";
    //with NUM_MAX_PARTICLES_PER_RIGID_BODY == 256, this yields 9792 bytes <-- fits;
     #define NUM_MAX_PARTICLES_PER_RIGID_BODY (256)
  {% endifequal %}
  #define NUM_WORK_ITEMS_PER_WORK_GROUP ( NUM_MAX_PARTICLES_PER_RIGID_BODY / 2 )
     
   
   
   #define CALCULATE_TOTAL_VECTOR_SUM( localSumVecAsArray, particleAttribLowerIndex, particleAttribHigherIndex ) \
      lScanArray[ particleLocalLowerIndex ] = particleAttribLowerIndex.x; \
      lScanArray[ particleLocalHigherIndex ] = particleAttribHigherIndex.x; \
      scanExclusive_upSweep(lScanArray, &(localSumVecAsArray[0]),  NUM_MAX_PARTICLES_PER_RIGID_BODY, lwiID); \
      lScanArray[ particleLocalLowerIndex ] = particleAttribLowerIndex.y; \
      lScanArray[ particleLocalHigherIndex ] = particleAttribHigherIndex.y; \
      scanExclusive_upSweep(lScanArray, &(localSumVecAsArray[1]),  NUM_MAX_PARTICLES_PER_RIGID_BODY, lwiID); \
      lScanArray[ particleLocalLowerIndex ] = particleAttribLowerIndex.z; \
      lScanArray[ particleLocalHigherIndex ] = particleAttribHigherIndex.z; \
      scanExclusive_upSweep(lScanArray, &(localSumVecAsArray[2]),  NUM_MAX_PARTICLES_PER_RIGID_BODY, lwiID)
      
  enum MacroscopicRBFeatures
  {
    CENTRE_OF_MASS_POSITION_X,
    CENTRE_OF_MASS_POSITION_Y,
    CENTRE_OF_MASS_POSITION_Z,
    LINEAR_VELOCITY_X,
    LINEAR_VELOCITY_Y,
    LINEAR_VELOCITY_Z,
    ANGULAR_VELOCITY_X,
    ANGULAR_VELOCITY_Y,
    ANGULAR_VELOCITY_Z,
    NUM_MACROSCOPIC_RB_PHYSICAL_QUANTITIES
  };
      
      
  __kernel
  __attribute__((reqd_work_group_size( NUM_WORK_ITEMS_PER_WORK_GROUP ,1,1)))  
  void
  updateRigidBodies
  (
    __constant SimulationParameters* cSimParams,
        
    __global float4* gParticlePositionsOld, //the positions not refreshed by particle integration of current path,
                                            //hence (gParticlePositionsOld[index] - rigidBody.oldCentreOfMass) encodes the rotated 
                                            //relative position of a particle from the previous pass
    __global float4* gParticlePositionsNew, //read for new centre of mass computation and written 
                                            //for new-RB-transform-alligned particle world positions;
    __global uint* gZindicesNew,
        
    __global float4* gParticleCorrectedVelocities,
    __global float4* gParticlePredictedVelocities,
        
        
    __global RigidBody* gRigidBodies,
    __global uint* gRigidBodyParticleIndexTable, //numRigidBodies * NUM_MAX_PARTICLES_PER_RIGID_BODY elements
       
  )
  {
    //used 9 times for a "half scan"
    __local float lScanArray[ PADDED_STRIDE( NUM_MACROSCOPIC_RB_PHYSICAL_QUANTITIES * NUM_MAX_PARTICLES_PER_RIGID_BODY ) ];
    
    __local float lMacroscopicPhysicalAttributes[ NUM_MACROSCOPIC_RB_PHYSICAL_QUANTITIES ];
    
    __local RigidBody lRigidBody;
    
    //__local float lCentreOfMassPositionNew[4]; //to be filled by the total sums of the position scans, 
                                             //then divided by particle count of the RB;
    //__local float lLinearVelocityNew[4];       //to be filled by the total sums of the position scans, 
                                             //then divided by particle count of the RB;
    //__local float lAngularVelocityNew[4];       //to be filled by the total sums of the position scans, 
                                             //then divided by particle count of the RB;                                             

    
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0);
    
    
    //{ grab rigid body to local memory in a hacky and hence hopefully efficient way ;) :
      TODO
    //}    
    
    uint particleGlobalLowerIndex  = gRigidBodyParticleIndexTable[ groupID * NUM_MAX_PARTICLES_PER_RIGID_BODY 
                                                                   + lwiID ];
    uint particleGlobalHigherIndex = gRigidBodyParticleIndexTable[ groupID * NUM_MAX_PARTICLES_PER_RIGID_BODY 
                                                                   lwiID + (NUM_MAX_PARTICLES_PER_RIGID_BODY/2) ];
    
    uint particleLocalLowerIndex = CONFLICT_FREE_INDEX( lwiID );
    uint particleLocalHigherIndex = CONFLICT_FREE_INDEX( lwiID + (NUM_MAX_PARTICLES_PER_RIGID_BODY/2) );

    
    //Because we have only up to 2^15=32k 32bit-registers per multiprocessor (even on fermi devices),
    //There are at most 2^15/groupSize= 2^15/2^10=32 registers available per work item;
    //Therefore, we have to reuse registers as much as possible to omit register spiilling to global memory,
    //which would completely ruin performance; Alone tho below 4 varibles consume 16 registers, i.e. at least the half
    //of all available space; So despit my usual style to name variables as precisely as possible and rely on the compiler
    //to rename registers automatically, I design the algorithm for explicit reusal, as this is supposedly the bottlenck of this kernel;
    float4 particlePosLowerIndex;
    float4 particlePosHigherIndex;
    float4 particleVelLowerIndex;
    float4 particleVelHigherIndex;
    
    if( lwiID < lRigidBody.numContainingParticles )
    {   
      //grab the new positions resulting from particle integration to get the new centre of mass   
      particlePosLowerIndex = gParticlePositionsNew[ particleGlobalLowerIndex ]; 
      particlePosHigherIndex = gParticlePositionsNew[ particleGlobalHigherIndex ]; 
      
      particleVelLowerIndex = gParticleCorrectedVelocities[ particleGlobalLowerIndex ]; 
      particleVelHigherIndex = gParticleCorrectedVelocities[ particleGlobalHigherIndex ]; 
    }
    else
    {
      //particle with this index doesn't exist for this rigid body, but we need to operate on base2 arrays,
      //hence init the rest to zero; 
      particlePosLowerIndex   = (float4) (0.0f,0.0f,0.0f,1.0f);
      particlePosHigherIndex  = (float4) (0.0f,0.0f,0.0f,1.0f);
      particleVelLowerIndex  = (float4) (0.0f,0.0f,0.0f,0.0f);
      particleVelHigherIndex = (float4) (0.0f,0.0f,0.0f,0.0f);
    }
    
    //{ calculate total position and velocity sums for centre of mass position and new linear velocity of rigid body 
    //(division by particle count comes later to save an "if")
      CALCULATE_TOTAL_VECTOR_SUM( lCentreOfMassPositionNew, particlePosLowerIndex, particlePosHigherIndex );
      CALCULATE_TOTAL_VECTOR_SUM( lLinearVelocityNew      , particleVelLowerIndex, particleVelHigherIndex );
    //} 
    
    
    //we now need the relatively rotated particle positions and the angular velocities:
    if( lwiID < lRigidBody.numContainingParticles )
    {      
      //relative positions from last simulation pass (better do some on-the-fly calculations than wasting memory and bandwidth and
      //destroying particle element buffer symmetry^^)
      particlePosLowerIndex = gParticlePositionsOld[ particleGlobalLowerIndex ] - lRigidBody.centreOfMassPosition ; 
      particlePosHigherIndex = gParticlePositionsOld[ particleGlobalHigherIndex ] - lRigidBody.centreOfMassPosition ; 
      //angular velocity of particle: relative position x linear velocity;
      particleVelLowerIndex = cross( particlePosLowerIndex, particleVelLowerIndex );
      particleVelHigherIndex = cross( particlePosHigherIndex, particleVelHigherIndex );
    } //no else here, stays zero ;)
    
    //{ calculate total angular velocity sums for new angular velocity of rigid body 
    //(division by particle count comes later)
      CALCULATE_TOTAL_VECTOR_SUM( lAngularVelocityNew, particleVelLowerIndex, particleVelHigherIndex );
    //} end calculate new centre of mass position
      
    
      if( lwiID < 3 )
      {
        //divide sum of world positions and linear velocites by the sum of all particles belonging to the RB;
        lCentreOfMassPositionNew[ lwiID ] *= lRigidBody.inverseNumContainingParticles;        
        lLinearVelocityNew[ lwiID ]       *= lRigidBody.inverseNumContainingParticles;
        // lAngularVelocityNew = sumAllAngVels / (sum( length(centreOfMassRelParticlePos)^2 ));
        //TODO in host from time to time read back particle buffers and reset particle positions according to 
        //"original distance to centre of mass", because numeric errors 
        //(by rotations, additionas and subtractions from different centres of masses etc) will most probably accumulate
        //and cause particle cloud and the macroscopic body, i.e the render model the to get out of synch;
        //plus, the particle cloud will deform itself more and more;
        lAngularVelocityNew[ lwiID ]      *= lRigidBody.inverseTotalSquaredDistancesFromCenterOfMass;
      }      
      //synch so that every thread can read the updated RB values;
      barrier(CLK_LOCAL_MEM_FENCE);  
      
      
      //----------------------------------------------------------------
      //We now have the macroscopic pos. and vel. values;
      //We have to construct a rotation matrix now to restore the old relative positions according to the new macroscpic features;
      //otherwise, the RB-particle cloud would dissipate just like the fluid particles;
      if( lwiID == 0 )
      {
        float lengthAngVel= length(lAngularVelocityNew);
float4 angVelNorm= angularVelocity/lengthAngVel;
float rotationAngleRadians = lengthAngVel * timeStep;
transformationMatrix= constructTransfPormationMatrix(
centreOfMassPosNew, normalize(angularVelocity), length(angularVelocity)*timeStep );
directionNew,  = transformationMatrix * directionOld;
float4 particleRealitveWorldPosOld =  (float4)((particleWorldPosOld.xyz - centreOfMassPosOld.xyz), 1.0f);
particleWorldposNew = transformationMatrix * ( particleRealitveWorldPosOld );
      
      
  }

