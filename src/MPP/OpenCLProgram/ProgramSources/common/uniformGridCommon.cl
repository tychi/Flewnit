

#ifndef FLEWNIT_CL_PROGRAMS_UNIFORM_GRID_COMMON_GUARD
#define FLEWNIT_CL_PROGRAMS_UNIFORM_GRID_COMMON_GUARD

  
  //{ uniform grid definitions
    //default: 64
    #define NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ numUniGridCellsPerDimension }} )
    //seemingly unneded in kernel :
      //default: 6 
      //#define LOG2_NUM_UNIGRID_CELLS_PER_DIMENSION ( was a template var ;(--> log2NumUniGridCellsPerDimension  )
      //default: 64^3 = 2^18 = 256k
      //#define NUM_TOTAL_GRID_CELLS ( NUM_UNIGRID_CELLS_PER_DIMENSION * NUM_UNIGRID_CELLS_PER_DIMENSION * NUM_UNIGRID_CELLS_PER_DIMENSION  )
    
    //default: 32; reason:
    //  < 32  --> some threads in warp idle
    //  > 32  --> double number of simulation work groups, many threads idle in split up cells;
    #define NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP ( {{ numMaxElementsPerSimulationWorkGroup }} )
    //default log2(32) = 5;
    #define LOG2_NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP ( {{ log2NumMaxElementsPerSimulationWorkGroup }} )
    
    //    0      residing particles --> 0 simulation work groups
    //  [ 1..32] residing particles --> 1 simulation work group
    //  [33..64] residing particles --> 2 simulation work groups etc. )
    #define GET_NUM_SIM_WORK_GROUPS_OF_CELL( numResidingElements ) \
      (  ( ( numResidingElements ) + ( NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP - 1 ) )    \
         >> LOG2_NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP                        )  
  //} end uniform grid definitions
  //--------------------------------------------------------
  
  


#endif //FLEWNIT_CL_PROGRAMS_UNIFORM_GRID_COMMON_GUARD
