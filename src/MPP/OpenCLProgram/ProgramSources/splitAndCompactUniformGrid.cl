
{% extends scan_localPar_globalSeq.cl %}

  {% block documentHeader %} 
    /**
      
      splitAndCompactUniformGrid.cl
      
      Specialization of scan_localPar_globalSeq.cl; Defines the appropriate tabulation function;
      
      The generated code will do a nearly-complete scan on the "simulation group count" of a uniform grid cell; 

    */  
  {% endblock documentHeader %} 


  {% block specialDefinitions %}   
    
    uint tabulate(uint globalIndex,  __global uint* gUniGridCells_ParticleStartIndex, __global uint* gUniGridCells_ParticleEndIndexPlus1)
    {
      uint endIndexPlus1 =  gUniGridCells_ParticleEndIndexPlus1[ globalIndex ];
    
      //to be valid, the value must be at least 1, as its name indicates for non-negative numbers;
      //an invalid end indx indicates an empty cell; Reason: in updateUniformGrid.cl, the cell entries have not been written,
      //because there was no particle responsible for this cell; as the buffer is clreard to zero every frame, zero indicates
      //the emptyness      
      if(endIndexPlus1 == 0) {return 0;}
      
      uint numResidingParticles = endIndexPlus1 - gUniGridCells_ParticleStartIndex[ globalIndex ];
      //do the fix for the optimization in updateUniformGrid.cl: assign the "true" number of particles: 
      gUniGridCells_ParticleEndIndexPlus1[ globalIndex ] = numResidingParticles;
      return GET_NUM_SIM_WORK_GROUPS_OF_CELL( numResidingParticles );
    }
  
  {% endblock specialDefinitions %} 
  
  
  {% block tabulationArgs%} 
    __global uint* gUniGridCells_ParticleStartIndex, //NUM_TOTAL_GRID_CELLS elements
    __global uint* gUniGridCells_ParticleEndIndexPlus1, //NUM_TOTAL_GRID_CELLS elements
                                               //see updateUniformGrid.cl argument list for further information about this 
                                               //"strange" encoding of the number of particles in a cell
  {% endblock tabulationArgs %} 
  
  
  {% block tabulation %} 
    //tabulate global values and store them to local mem for scan
    lLocallyScannedTabulatedValues[ paddedLocalLowerIndex  ] = 
      tabulate( globalLowerIndex, 
                gUniGridCells_ParticleStartIndex, 
                gUniGridCells_ParticleEndIndexPlus1  );
    lLocallyScannedTabulatedValues[ paddedLocalHigherIndex ] =    
      tabulate( globalHigherIndex, 
                gUniGridCells_ParticleStartIndex, 
                gUniGridCells_ParticleEndIndexPlus1  );
  {% endblock tabulation %}
  
  
  {% block furtherImplementation %}
    //=====================================================================================
    /*
      Kernel to do a stream compaction on the base of the scan of the number of work groups per cell;
      The total count of simulation work groups to be launched for the following physics simulation phases
      will be written to gSumsOfPartialGlobalScans[NUM_BASE2_CEILED_COMPUTE_UNITS];
      
      Note that in contrast to kernel_scan_localPar_globalSeq, in this kernel, no sequentialization will be performed,
      because here, it wouldn't save a kernel invocation;
      
    */
    
    __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
    void kernel_splitAndCompactUniformGrid(
      __global uint* gUniGridCells_ParticleStartIndex, //NUM_TOTAL_GRID_CELLS elements; to be split and compacted,too
      __global uint* gUniGridCells_NumParticles, //NUM_TOTAL_GRID_CELLS  elements, to be tabulated again for split; costs extra calculations,
                                                 //but saves memory and bandwidth;
      
       //ping pong components of gUniGridCells_ParticleStartIndex and gUniGridCells_NumParticles;
       //In the physics simulation phase, only "total count of simulation work groups" elements will be used;                     
      __global uint* gCompactedUniGridCells_ParticleStartIndex
      __global uint* gCompactedUniGridCells_NumParticles, 

      __global uint* gLocallyScannedTabulatedValues, //gLocallyScannedSimWorkGroupCount, NUM_TOTAL_ELEMENTS  elements 
      __global uint* gPartiallyGloballyScannedTabulatedValues, //NUM_ELEMENTS__GLOBAL_SCAN elements
      __global uint* gSumsOfPartialGlobalScans,  //at least NUM_BASE2_CEILED_COMPUTE_UNITS + 1  elements;
                                                 //+1 because this kernel writes out the total sim work group count                                                 
    )
    {
      __local uint lScannedSumsOfPartialGlobalScans [ PADDED_STRIDE ( NUM_BASE2_CEILED_COMPUTE_UNITS )];
      
      uint lwiID = get_local_id(0); // short for "local work item ID"
      uint gwiID = get_global_id(0); // short for "global work item ID"
      uint groupID =  get_group_id(0);
      
      //check if we actually have more than one compute units, otherwise the scan would not be called and hence the first element
      //in lSumsOfPartialScansOfSumsOfGlobalRadixCounts wouldn't be zero and corrupt the offsetting; we have to catch this situation.
      #if NUM_BASE2_CEILED_COMPUTE_UNITS > 1
        if(lwiID < (NUM_BASE2_CEILED_COMPUTE_UNITS) )
        { 
          //copy the tiny array to be scanned in order to yield the final offset and total count to local memory:
          uint paddedLocalIndex = CONFLICT_FREE_INDEX( get_local_id(0) );
          lScannedSumsOfPartialGlobalScans[ paddedLocalIndex ] =  gSumsOfPartialGlobalScans[ lwiID ];
          //not necessary for very small arrays, bu i wanna be ready for systems with a higher multiprocessor count
          //TODO surround with an ifdef and research reliable info about implicit synchronization level 
          //(warp/halfwarp for fermi, warp/halfwarp/quarterwarp for GT200 ? )
          barrier(CLK_LOCAL_MEM_FENCE);
        }
        //scan the coarsest granularity   
        //default for GT435M 2/2=1; yes ,really, it is not worth such an invocation on such a device, but i wanna stay general
        //and not optimize for a single graphics card ;)
        if(lwiID < (NUM_BASE2_CEILED_COMPUTE_UNITS/2) )
        {        
          uint totalSimWorkGroupCount = scanExclusive(lScannedSumsOfPartialGlobalScans,NUM_BASE2_CEILED_COMPUTE_UNITS, lwiID );
          if(lwiID == 0)
          {
            //write the total count to global memory
            gSumsOfPartialGlobalScans[NUM_BASE2_CEILED_COMPUTE_UNITS] = totalSimWorkGroupCount;
          }
        }
      #else
        if(lwiID == 0)
        {
          lScannedSumsOfPartialGlobalScans[ 0 ] =  0;
          //write the total count to global memory
          gSumsOfPartialGlobalScans[1] = gSumsOfPartialGlobalScans[ 0 ];
        }
      #endif

      //because we have twice as much elements as work items (due to the scan stuff), we have also to compact two elements per work item:
      //TODO check if unroll amortizes or not
          
      uint globalIndex =  groupID * NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN + lwiID ;
      uint groupOffset = 
           //default values:
           // Geforce GT  435 M: [0..128]/64=[0.. 1]
           // Geforce GTX 280  : [0..128]/ 4=[0..32]
           // Geforce GTX 570  : [0..128]/ 8=[0..16]
          lScannedSumsOfPartialGlobalScans[ groupID / NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY ]
          +
          gPartiallyGloballyScannedTabulatedValues[ groupID ];
      #pragma unroll
      for(uint i=0 ; i < 2; i++ )
      {      
        uint currentNumResidentParticles =  gUniGridCells_NumParticles[  globalIndex ];
        
        //write out only non-empty grid cells!
        if(currentNumResidentParticles > 0)
        {
          uint currentParticleStartIndex = gUniGridCells_ParticleStartIndex[ globalIndex ];
          uint numSimWorkGroupsOfThisCell = GET_NUM_SIM_WORK_GROUPS_OF_CELL( currentNumResidentParticles );
          uint compactedIndex = groupOffset + gLocallyScannedTabulatedValues[ globalIndex ];
           
          //split
          for(uint simGroupRunner=0; simGroupRunner < numSimWorkGroupsOfThisCell; simGroupRunner++ )
          {
            //compact
            gCompactedUniGridCells_NumParticles[compactedIndex + simGroupRunner] = 
              ( 
                ( simGroupRunner < (numSimWorkGroupsOfThisCell - 1 ) )
                //for all but the last group, there are 32 particles in each sim work group
                ? (NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP)
                //the last sim work group gets the rest of the particles, namely currentNumResidentParticles%32
                : ( BASE_2_MODULO( currentNumResidentParticles, NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP) ) 
              ); 
              
            gCompactedUniGridCells_ParticleStartIndex[ compactedIndex + simGroupRunner ] = 
              currentParticleStartIndex
              //increment start index by intervals of 32
              + (simGroupRunner << LOG2_NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP);    
          }  
           
        }//endif(currentNumResidentParticles >0)
        
        globalIndex += (NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN/2);
      }
        
    
    }
  {% endblock furtherImplementation %}
