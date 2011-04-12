

#ifndef FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD
#define FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD

  //{ uniform grid definitions
    //default: 64
    #define NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ numUniGridCellsPerDimension }} )
    //default: 6
    #define LOG2_NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ log2NumUniGridCellsPerDimension }} )
  //} end uniform grid definitions
  
  
  
  //{ SPH definitions; refer to "Particle-Based Fluid Simulation for Interactive Applications" by Matthias Mueller et. al.
  //    for further info;

    //must be <= uniGridCellSize; we mass it as makro so that some SPH kernel terms can be precomputed
    #define SPH_SUPPORT_RADIUS ( {{ SPHsupportRadius }} )
    #define SQUARED_SPH_SUPPORT_RADIUS ( SPH_SUPPORT_RADIUS * SPH_SUPPORT_RADIUS )
    //constant terms of used SPH kernels, precomputed by app in order to ensure that the values are truely precomputed
    //(instead of just relying on the ability of the compiler to precompute constant terms ;( )
    //( 315.0f / ( 64.0f * PI * pown(SPH_SUPPORT_RADIUS, 9) ) )
    #define POLY6_CONSTANT_TERM ( {{ poly6ConstantTerm }} )
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) <-- 3 * SPIKY_CONSTANT_TERM; non-negative bcause it is the derivative 
    //"in the direction towards the center", hence coming from positive infinity going to origin, the gradient is positive
    //as the values become greater towards the origin;
    #define GRAD_SPIKY_CONSTANT_TERM ( {{ gradSpikyConstantTerm }} )
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) <-- that this is the same value as GRAD_SPIKY_CONSTANT_TERM is
    //a "coincidence"; the true laplacian of the viscosity kernel would be 
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) * (h-r  - ((h^4)/(r^3)) ) instead of
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) * (h-r)                   denoted in the paper; a reason wasn't given;
    //I can only speculate that this is a reasonable simplification to trade speed for accuracy;
    #define LAPLACIAN_VISCOSITY_CONSTANT_TERM( {{ laplacianViscosityConstantTerm }}) 

  //} end SPH definitions
  
  
  //{ rigid body definitions
    
    //may be at mos (1<<15)=32k; reason: rigid body ID is encoded into a short, we need an "invalid ID indicator value" 
    //which is greater than the max. allowed value, and we wanna stick to our base 2- paradigm;
    //Note: This thesis program is not designed for massive rigid body simulation; It is rather a funny gimmick;
    //So use much less rigid bodies in practice than 32k ;(.
    //#define NUM_RIGID_BODIES ( {{numRigidBodies}} )
    //2048; reason: this is the maximum number of elements we can scan in parallel within a single work group
    //without any "tricks"; we need the total sums of positions and velocities
    #define NUM_MAX_PARTICLES_PER_RIGID_BODY (1<<11)
    #define INVALID_RIGID_BODY_ID ( (ushort) (-1) )
    #define IS_RIGID_BODY_PARTICLE ( particleRigidBodyInfo ) ( (particleRigidBodyInfo.rigidBodyID) != INVALID_RIGID_BODY_ID )
    
  //} end rigid body definitions
  
  
#endif //FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD
