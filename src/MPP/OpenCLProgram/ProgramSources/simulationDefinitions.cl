

#ifndef FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD
#define FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD

  //{ uniform grid definitions
    //default: 64
    #define NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ numUniGridCellsPerDimension }} )
    //default: 6
    #define LOG2_NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ log2NumUniGridCellsPerDimension }} )
    
    //default: 32; reason:
    //  < 32  --> some threads in warp idle
    //  > 32  --> double number of simulation work groups, many threads idle in split up cells;
    #define NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP ( {{ numMaxParticlesPerSimulationWorkGroup }}
    //default log2(32) = 5;
    #define LOG2_NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP ( {{ log2NumMaxParticlesPerSimulationWorkGroup }}
    
    //    0      residing particles --> 0 simulation work groups
    //  [ 1..32] residing particles --> 1 simulation work group
    //  [33..64] residing particles --> 2 simulation work groups etc. )
    #define GET_NUM_SIM_WORK_GROUPS_OF_CELL( numResidingParticles ) ( ( (numResidingParticles) + (NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP-1) ) >> LOG2_NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP )  
  
    
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
  
  

  
  
#endif //FLEWNIT_CL_PROGRAMS_SIMULATION_DEFINITIONS_GUARD

