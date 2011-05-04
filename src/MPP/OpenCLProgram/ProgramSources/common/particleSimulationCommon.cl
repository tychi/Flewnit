  
#ifndef FLEWNIT_CL_PROGRAMS_PARTICLE_SIMULATION_COMMON_GUARD
#define FLEWNIT_CL_PROGRAMS_PARTICLE_SIMULATION_COMMON_GUARD

  {% include "common.cl" %}
  {% include "uniformGridCommon.cl" %}

  {% include "physicsDataStructures.cl" %}
  
  //-------------fluid simulation macros ------------------------------------------- 
  //{ Fluid and Rigid Body related definitions
    /*
      Hardware dependent size:
      We need to perform nine "half scans" on the belonging particle attributes
      to achieve the total sum to get the mean values, namely 
              - three on positions (one per dimension), to get the new centre of mass
              - six on corrected velocities: 
                - one per dimension for linear vel
                - one per dim. for angular vel
      Because of this heavy need for scans, we must comupte them most efficiently, and this is why we shouldn't have a need to split
      them up; With a bit of coding, like already done in radixSort.cl (see the for(NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY)-loop), 
      the extension to bigger particle counts per work group is relativley easy;
      But at first, I wanna keep the Rigid Body stuff as simple as possible;   
            
      default: 
        fermi: 48kB local memory available; 9 "half scans" to do; we need 
              PADDED_STRIDE( NUM_MAX_PARTICLES_PER_RIGID_BODY * 4 * 9) bytes for this "multiple scan array";
              with NUM_MAX_PARTICLES_PER_RIGID_BODY == 1024, this yields 38016 bytes <-- fits;
        GT200 and below: 16kB local memory available; 9 "half scans" to do; we need 
              PADDED_STRIDE( NUM_MAX_PARTICLES_PER_RIGID_BODY * 4 * 9) bytes for this "multiple scan array";
              with NUM_MAX_PARTICLES_PER_RIGID_BODY == 256, this yields 9792 bytes <-- fits;
              
    */
    #define NUM_MAX_PARTICLES_PER_RIGID_BODY ( {{ numMaxParticlesPerRigidBody }} )
    
    
    #define RIGID_BODY_OBJECT_START_INDEX ( {{ numFluidObjects }} + 1 )
    //ID of those particles idling at the end of the rigid buffer segments; during voxelization,
    //there will most likely not a power of two number of RB particles be generated; hence, there is
    //unused space in the buffer we have to mask out, in the best case without any branching;
    //the solution: 
    //    cObjectGenericFeatures[INVALID_OBJECT_ID].massPerParticle = 0.0f;
    //    cObjectGenericFeatures[INVALID_OBJECT_ID].restDensity= HUGE_FLOAT;
    //--> densitiy computation: mass == 0 --> no contribution in SPH calcs --> masked without branching
    //--> pressure and visc. compuation: mass/dens == 0/infinity== 0 --> no contribution in SPH calcs --> masked without branching
    //--> integrate: ->forces non-zero, mass is zero --> singularity --> has to be masked via branching;
    //                but this branch is only done once per particle and frame and not 27*32*x times like the SPH compuations;
    #define INVALID_OBJECT_ID ( {{ numFluidObjects }} )
         
    #define IS_FLUID_PARTICLE( particleObjectID )       ( ( particleObjectID ) <  INVALID_OBJECT_ID )
    #define BELONGS_TO_FLUID( particleObjectID )        ( ( particleObjectID ) <  INVALID_OBJECT_ID )

    #define IS_RIGID_BODY_PARTICLE( particleObjectID ) ( ( particleObjectID ) >= RIGID_BODY_OBJECT_START_INDEX )                       
    #define BELONGS_TO_RIGID_BODY( particleObjectID )  ( ( particleObjectID ) >= RIGID_BODY_OBJECT_START_INDEX )
    
    #define IS_INVALID_PARTICLE( particleObjectID )       ( ( particleObjectID ) ==  INVALID_OBJECT_ID )
    #define BELONGS_TO_INVALID_OBJECT( particleObjectID )        ( ( particleObjectID ) ==  INVALID_OBJECT_ID )  
    
                             
    //convenience macros to handle offset stuff for rigid bodies
    //be sure to use this makro only if IS_RIGID_BODY_PARTICLE() returns true!
     //#define GET_RIGID_BODY_ID ( particleObjectInfo ) ( ( GET_OBJECT_ID(particleObjectInfo) ) - RIGID_BODY_OFFSET )
     #define GET_RIGID_BODY_ID( particleObjectID ) ( ( particleObjectID ) - RIGID_BODY_OBJECT_START_INDEX )  
  //}



  
#endif //FLEWNIT_CL_PROGRAMS_PARTICLE_SIMULATION_COMMON_GUARD
  
