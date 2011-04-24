  /*
  
    Kernel to calculate the "macroscopic" attributes of the rigid bodies with help of the results
    from the previous particle integrations;
  
    Formulas are taken from the Diploma thesis of Thomas Steil, "Efficient Methods for Computational Fluid Dynamics and Interactions",
    §3.2.4 "Rigid bodies in particle system";
    
    IMPORTANT: pass numArraysToScanInParallel = 9 to grantlee, else not all attributes and dimensions will be summed up
    
  */

  
  #pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
  #pragma  OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
  #pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable    
      
    
  {% include "scan.cl" %}
  {% include "matrixMath.cl" %}
   
  
  #define NUM_WORK_ITEMS_PER_WORK_GROUP ( NUM_MAX_PARTICLES_PER_RIGID_BODY / 2 )
     
   

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
    __constant ObjectGenericFeatures* cObjectGenericFeatures,

    __global uint* gParticleIndexTable, //for finding the particles belonging to the current rigid body
        
        
 ###################       
    //TODO replace one of the buffers with relative rigid-body-only position buffers
    TODO TODO TODO IMPORTANT!!111
 #################   
    
    
    __global float4* gParticlePositionsOld, //the positions not refreshed by particle integration of current path,
                                            //hence (gParticlePositionsOld[index] - rigidBody.oldCentreOfMass) encodes the rotated 
                                            //relative position of a particle from the previous pass
    __global float4* gParticlePositionsNew, //read for new centre of mass computation and written 
                                            //for new-RB-transform-alligned particle world positions;

    __global float4* gParticleCorrectedVelocities,
    
    __global float4* gParticlePredictedVelocities,
        
    //numRigidBodies elements    
    __global ParticleRigidBody* gRigidBodies,
    //numRigidBodies * NUM_MAX_PARTICLES_PER_RIGID_BODY elements
    __global float4* gRigidBodyRelativePositions
       
  )
  {
    //used 9 times for a "half scan"
    __local float lScanArray[ PADDED_STRIDE( NUM_MACROSCOPIC_RB_PHYSICAL_QUANTITIES * NUM_MAX_PARTICLES_PER_RIGID_BODY ) ];
    //9 elements holding the total sums of world positions, linear and angular velocities after "multi-array-scan"
    __local float lMacroscopicPhysicalAttributes[ NUM_MACROSCOPIC_RB_PHYSICAL_QUANTITIES ];
    
    __local float16 finalRotationMatrix;
    __local float16 tempRotationMatrix;
    
    __local ParticleRigidBody lRigidBody;
    
                                           

    
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0); //group ID equvalent to rigid body ID in this kernel
    
    
    //{ grab rigid body to local memory in a hacky and hence hopefully efficient way ;) :
      if( lwiID < sizeof(ParticleRigidBody)/4 )
      {
        ( (__local uint* ) ( & ( lRigidBody ) ) )
              [ lwiID ]  
          =  
        ( (__global uint*) ( & ( gRigidBodies[ groupID ] ) ) )
              [ lwiID ] ;
      } 
    //}    
    
    //-------------------------------------------------------------------------------------------------------    
    uint particleLocalLowerIndex = CONFLICT_FREE_INDEX( lwiID );
    uint particleLocalHigherIndex = CONFLICT_FREE_INDEX( lwiID + (NUM_MAX_PARTICLES_PER_RIGID_BODY/2) );
    
    uint particleGlobalLowerIndex;
    uint particleGlobalHigherIndex;
    //-------------------------------------------------------------------------------------------------------
    
    //-------------------------------------------------------------------------------------------------------
    float4 particlePosLowerIndex;
    float4 particlePosHigherIndex;
    
    float4 particleVelLowerIndex;
    float4 particleVelHigherIndex;
    //-------------------------------------------------------------------------------------------------------
    
    
    if( lwiID < lRigidBody.numContainingParticles )
    {   
      //particleGlobalLowerIndex  = gParticleIndexTable[ groupID * NUM_MAX_PARTICLES_PER_RIGID_BODY
      particleGlobalLowerIndex  = gParticleIndexTable[
                                  //group ID equivalent to rigid body ID in this kernel 
          cObjectGenericFeatures[ groupID +  RIGID_BODY_OBJECT_START_INDEX ].offsetInIndexTableBuffer
          + lwiID 
        ];
      //particleGlobalHigherIndex = gParticleIndexTable[ groupID * NUM_MAX_PARTICLES_PER_RIGID_BODY                                                                     
      particleGlobalHigherIndex = gParticleIndexTable[
                                  //group ID equivalent to rigid body ID in this kernel 
          cObjectGenericFeatures[ groupID +  RIGID_BODY_OBJECT_START_INDEX ].offsetInIndexTableBuffer
          + lwiID + (NUM_MAX_PARTICLES_PER_RIGID_BODY/2) 
        ];
    
    
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
    
    //reorder new world positions into "multiscan array" for scanning each dimension seperately 
    //to yield the new centre of mass position; 
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particlePosLowerIndex.x;
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particlePosHigherIndex.x;
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particlePosLowerIndex.y;
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particlePosHigherIndex.y;
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particlePosLowerIndex.z;
    lScanArray[ PADDED_STRIDE( CENTRE_OF_MASS_POSITION_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particlePosHigherIndex.z;
      
    //reorder new corrected linear velocities into "multiscan array" for scanning each dimension seperately
    //to yield the new linear velocity; 
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.x;
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.x;
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.y;
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.y;
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.z;
    lScanArray[ PADDED_STRIDE( LINEAR_VELOCITY_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.z;  
       
    float4 particleRelativeOldPosLowerIndex;
    float4 particleRelativeOldPosHigherIndex;

    //we now need the relative and rotated particle positions from the previous pass and the angular velocities:
    if( lwiID < lRigidBody.numContainingParticles )
    {      
      //relative positions from last simulation pass (better do some on-the-fly calculations than wasting memory and bandwidth and
      //destroying particle element buffer symmetry^^)
      particlePosLowerIndex = gParticlePositionsOld[ particleGlobalLowerIndex ];
      particlePosHigherIndex = gParticlePositionsOld[ particleGlobalHigherIndex ];
      
      particleRelativeOldPosLowerIndex = particlePosLowerIndex  - lRigidBody.centreOfMassPosition ; 
      particleRelativeOldPosHigherIndex = particlePosHigherIndex - lRigidBody.centreOfMassPosition ; 
      
      
      //angular velocity of particle: relative position x linear velocity;
      particleVelLowerIndex = cross( particleRelativeOldPosLowerIndex, particleVelLowerIndex );
      particleVelHigherIndex = cross( particleRelativeOldPosHigherIndex, particleVelHigherIndex );
    } //no else here, stays zero ;)
    
    //reorder new corrected  angular velocities into "multiscan array" for scanning each dimension seperately
    //to yield the new angular velocity; 
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.x;
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_X * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.x;
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.y;
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_Y * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.y;
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalLowerIndex ] = 
      particleVelLowerIndex.z;
    lScanArray[ PADDED_STRIDE( ANGULAR_VELOCITY_Z * NUM_MAX_PARTICLES_PER_RIGID_BODY ) + particleLocalHigherIndex ] = 
      particleVelHigherIndex.z;  

    
    scanExclusive_upSweep(lScanArray, lMacroscopicPhysicalAttributes,  NUM_MAX_PARTICLES_PER_RIGID_BODY, lwiID);
    
    
    
   //----------------------------------------------------------------
   //We now have the macroscopic pos. and vel. values;
   //We have to construct a rotation matrix now to restore the old relative positions according to the new macroscpic features;
   //otherwise, the RB-particle cloud would dissipate just like the fluid particles;
   if( lwiID == 0 )
   {
     //no need for synch yet, as work item 0 has done all the "total sum" stuff;
     //note; instead of using 3 thread to parllize the below downscale, I rather save one if() statement;
     //and do alltransformations matrix building with one single work item;
     

     //overwrite the old values, we don't need them anymore, an save register usage this way;
     
     //newRBWorldPos = newCentreOfMassPos + translationalPart(correctiveTransformationMatrix);
     lRigidBody.centreOfMassPosition = (float4) (
       lMacroscopicPhysicalAttributes[ CENTRE_OF_MASS_POSITION_X ] * lRigidBody.inverseNumContainingParticles, 
       lMacroscopicPhysicalAttributes[ CENTRE_OF_MASS_POSITION_Y ] * lRigidBody.inverseNumContainingParticles, 
       lMacroscopicPhysicalAttributes[ CENTRE_OF_MASS_POSITION_Z ] * lRigidBody.inverseNumContainingParticles,
       0.0f
     )
     //add the translational part of the corrective matrix; see declaration comments for further info;
     + lRigidBody.correctiveTransformationMatrix.scdef;
     ;
     lRigidBody.linearVelocity = (float4) (
       lMacroscopicPhysicalAttributes[ LINEAR_VELOCITY_X ]  * lRigidBody.inverseNumContainingParticles, 
       lMacroscopicPhysicalAttributes[ LINEAR_VELOCITY_Y ]  * lRigidBody.inverseNumContainingParticles, 
       lMacroscopicPhysicalAttributes[ LINEAR_VELOCITY_Z ]  * lRigidBody.inverseNumContainingParticles,
       0.0f     
     );
    lRigidBody.angularVelocity = (float4) (
       lMacroscopicPhysicalAttributes[ ANGULAR_VELOCITY_X ]  * lRigidBody.inverseTotalSquaredDistancesFromCenterOfMass, 
       lMacroscopicPhysicalAttributes[ ANGULAR_VELOCITY_Y ]  * lRigidBody.inverseTotalSquaredDistancesFromCenterOfMass, 
       lMacroscopicPhysicalAttributes[ ANGULAR_VELOCITY_Z ]  * lRigidBody.inverseTotalSquaredDistancesFromCenterOfMass,
       0.0f     
     ); 
      
      
     float lengthAngVel= length(lRigidBody.angularVelocity);
     float4 rotationAxis = lRigidBody.angularVelocity / lengthAngVel;
     float rotationAngleRadians = lengthAngVel * cSimParams->timestep;
     //rotationMatrix =  rotationalPart(correctiveTransformationMatrix) * rotate(norm(angVel),length(angVel)*timestep);
     constructRotationMatrix( & tempRotationMatrix, rotationAxis, rotationAngleRadians );
     //take rotation only
     matrixMult3x3( & finalRotationMatrix, & lRigidBody.correctiveTransformationMatrix, & tempRotationMatrix );
     
     lRigidBody.direction = matrixMult3x3Vec(&finalRotationMatrix,lRigidBody.direction);
     lRigidBody.upVector =  matrixMult3x3Vec(&finalRotationMatrix,lRigidBody.upVector);
  }
      
  //synch so that every thread can read the updated RB values;
  barrier(CLK_LOCAL_MEM_FENCE);
  
     
  if( lwiID < lRigidBody.numContainingParticles )
  {    
    gParticleCorrectedVelocities[ particleGlobalLowerIndex ] = 
      lRigidBody.linearVelocity + cross(lRigidBody.angularVelocity, particleRelativeOldPosLowerIndex);
    gParticleCorrectedVelocities[ particleGlobalHigherIndex ] = 
      lRigidBody.linearVelocity + cross(lRigidBody.angularVelocity, particleRelativeOldPosHigherIndex);

    //pray for automatic register renaming, otherwise, we ahve the danger of spiiling to global mem!
    float4 particleNewPosLowerIndex =  
      lRigidBody.centreOfMassPosition + matrixMult3x3Vec(&finalRotationMatrix, particleRelativeOldPosLowerIndex);
    float4 particleNewPosHigherIndex =  
      lRigidBody.centreOfMassPosition + matrixMult3x3Vec(&finalRotationMatrix, particleRelativeOldPosHigherIndex);
      
    gParticlePositionsNew[ particleGlobalLowerIndex ] = particleNewPosLowerIndex;
    gParticlePositionsNew[ particleGlobalHigherIndex ] = particleNewPosHigherIndex;
   
    //update also predicted values; don't corrupt the integration scheme ;P
    gParticlePredictedVelocities[ particleGlobalLowerIndex ] = 
        ( particleNewPosLowerIndex - particlePosLowerIndex ) * cSimParams->inverseTimestep;
    gParticlePredictedVelocities[ particleGlobalHigherIndex ] = 
        ( particleNewPosHigherIndex - particlePosHigherIndex ) * cSimParams->inverseTimestep;
    
   }
    
    
   //{ upload rigid body to global memory in a hacky and hence hopefully efficient way ;) :
      if( lwiID < sizeof(ParticleRigidBody)/4 )
      {
        ( (__global uint*) ( & ( gRigidBodies[ groupID ] ) ) )
              [ lwiID ]  
          =  
        ( (__local uint* ) ( & ( lRigidBody ) ) )
              [ lwiID ] ;
      } 
   //}  
    
    
    
    
      
  }

