
{% extends "scan_localPar_globalSeq.cl" %}

  {% block documentHeader %} 
    /**
      
      splitAndCompactUniformGrid.cl
      
      Specialization of scan_localPar_globalSeq.cl; Defines the appropriate tabulation function;
      
      The generated code will do a nearly-complete scan on the "simulation group count" of a uniform grid cell; 

    */  
  {% endblock documentHeader %} 


  {% block specialDefinitions %} 
  
  
    {% include "uniformGridCommon.cl" %}  
    
    
    uint tabulate(
      uint globalNonCompatcedCellIndex, 
      __global uint* gUniGridCells_ElementStartIndex,
      __global uint* gUniGridCells_ElementEndIndexPlus1 
    )
    {
      uint endIndexPlus1 =  gUniGridCells_ElementEndIndexPlus1[ globalNonCompatcedCellIndex ];
    
      //to be valid, the value must be at least 1, as its name indicates for non-negative numbers;
      //an invalid end indx indicates an empty cell; Reason: in updateUniformGrid.cl, the cell entries have not been written,
      //because there was no element responsible for this cell; as the buffer is clreard to zero every frame, zero indicates
      //the emptyness      
      if(endIndexPlus1 == 0) {return 0;}
      
      uint numResidingElements = endIndexPlus1 - gUniGridCells_ElementStartIndex[ globalNonCompatcedCellIndex ];
      //do the fix for the optimization in updateUniformGrid.cl: assign the "true" number of elements: 
      gUniGridCells_ElementEndIndexPlus1[ globalNonCompatcedCellIndex ] = numResidingElements;
      return GET_NUM_SIM_WORK_GROUPS_OF_CELL( numResidingElements );
    }
  
  {% endblock specialDefinitions %} 
  
  
  {% block tabulationArgs%} 
    __global uint* gUniGridCells_ElementStartIndex, //NUM_UNIGRID_CELLS_PER_DIMENSION ^3 elements
    __global uint* gUniGridCells_ElementEndIndexPlus1, //NUM_UNIGRID_CELLS_PER_DIMENSION ^3 elements
                                               //see updateUniformGrid.cl argument list for further information about this 
                                               //"strange" encoding of the number of elements in a cell
  {% endblock tabulationArgs %} 
  
  
  {% block tabulation %} 
    //tabulate global values and store them to local mem for scan
    lLocallyScannedTabulatedValues[ paddedLocalLowerIndex  ] = 
      tabulate( globalLowerIndex, 
                gUniGridCells_ElementStartIndex, 
                gUniGridCells_ElementEndIndexPlus1  );
    lLocallyScannedTabulatedValues[ paddedLocalHigherIndex ] =    
      tabulate( globalHigherIndex, 
                gUniGridCells_ElementStartIndex, 
                gUniGridCells_ElementEndIndexPlus1  );
  {% endblock tabulation %}
  
  
  {% block furtherImplementation %}
    //=====================================================================================
    /*
      Kernel to do a stream compaction on the base of the scan of the number of work groups per cell;
      The total count of simulation work groups to be launched for the following physics simulation phases
      will be written to gSumsOfPartialGlobalScans[NUM_COMPUTE_UNITS_BASE2_CEILED];
      
      Note that in contrast to kernel_scan_localPar_globalSeq, in this kernel, no sequentialization will be performed,
      because here, it wouldn't save a kernel invocation;
      
    */

    __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
    void kernel_splitAndCompactUniformGrid
    (
      __global uint* gUniGridCells_ElementStartIndex, //NUM_UNIGRID_CELLS_PER_DIMENSION ^3 elements; to be split and compacted,too
      __global uint* gUniGridCells_NumElements, //NUM_UNIGRID_CELLS_PER_DIMENSION ^3  elements, to be tabulated again for split; costs extra calculations,
                                                 //but saves memory and bandwidth;
      
       //UniformGridBufferSet to be compacted;
       //In the physics simulation phase, only "total count of simulation work groups" elements of these Buffers will be used;              
      __global uint* gCompactedUniGridCells_ElementStartIndex,
      __global uint* gCompactedUniGridCells_NumElements, 

      __global uint* gLocallyScannedTabulatedValues, //gLocallyScannedSimWorkGroupCount, NUM_TOTAL_ELEMENTS_TO_SCAN  elements 
      __global uint* gPartiallyGloballyScannedTabulatedValues, //NUM_ELEMENTS__GLOBAL_SCAN elements
      __global uint* gSumsOfPartialGlobalScans   //at least NUM_COMPUTE_UNITS_BASE2_CEILED + 1  elements;
                                                 //+1 because this kernel writes out the total sim work group count                                                 
    )
    {
      __local uint lScannedSumsOfPartialGlobalScans [ PADDED_STRIDE ( NUM_COMPUTE_UNITS_BASE2_CEILED )];
      __local uint lTotalSimWorkGroupCount;
      
      uint lwiID = get_local_id(0); // short for "local work item ID"
      uint gwiID = get_global_id(0); // short for "global work item ID"
      uint groupID =  get_group_id(0);
      
      //check if we actually have more than one compute units, otherwise the scan would not be called and hence the first element
      //in lSumsOfPartialScansOfSumsOfGlobalRadixCounts wouldn't be zero and corrupt the offsetting; we have to catch this situation.
      #if NUM_COMPUTE_UNITS_BASE2_CEILED > 1
      
        if(lwiID < (NUM_COMPUTE_UNITS_BASE2_CEILED) )
        { 
          //copy the tiny array to be scanned in order to yield the final offset and total count to local memory:
          uint paddedLocalIndex = CONFLICT_FREE_INDEX( lwiID );
          lScannedSumsOfPartialGlobalScans[ paddedLocalIndex ] =  gSumsOfPartialGlobalScans[ lwiID ];
          //not necessary for very small arrays, but I wanna be ready for systems with a higher multiprocessor count;
          //TODO surround with an ifdef and research reliable info about implicit synchronization level 
          //(warp/halfwarp for fermi, warp/halfwarp/quarterwarp for GT200 ? )
        }
        barrier(CLK_LOCAL_MEM_FENCE); //ensure everything is in local mem
        
        //scan the coarsest granularity   
        //default for GT435M 2/2=1; yes ,really, it is not worth such an invocation on such a device 
        //(compared to a simple sequential scan), but i wanna stay general and not optimize for a single graphics card ;)
        
        scanExclusive(lScannedSumsOfPartialGlobalScans, & lTotalSimWorkGroupCount, NUM_COMPUTE_UNITS_BASE2_CEILED, lwiID );
          
        if(lwiID == 0)
        {
          //write the total count to global memory
          gSumsOfPartialGlobalScans[NUM_COMPUTE_UNITS_BASE2_CEILED] = lTotalSimWorkGroupCount;
        }
        

      #else
      
        if(lwiID == 0)
        {
          lScannedSumsOfPartialGlobalScans[ 0 ] =  0;
          //write the total count to global memory
          gSumsOfPartialGlobalScans[1] = gSumsOfPartialGlobalScans[ 0 ];
        }
        
      #endif

      //because we have twice as much elements as work items (due to the scan stuff),
      //we have also to compact two elements per work item:
      //this is why we cannot just take the gloabel work item id as index; 
      uint globalNonCompatcedCellIndex =  groupID * NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN + lwiID ;
           
      uint groupCompactionOffset = 
           //default values:
           // Geforce GT  435 M: [0..128]/64=[0.. 1]
           // Geforce GTX 280  : [0..128]/ 4=[0..32]
           // Geforce GTX 570  : [0..128]/ 8=[0..16]
          lScannedSumsOfPartialGlobalScans[ groupID / NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY ]
          +
          gPartiallyGloballyScannedTabulatedValues[ groupID ];

      //TODO check if unroll amortizes or not
      //#pragma unroll
      for(uint i=0 ; i < 2; i++ )
      {      
        uint currentNumResidentElements =  gUniGridCells_NumElements[  globalNonCompatcedCellIndex ];
        
        //write out only non-empty grid cells!
        //not that the buffer entries beyond lTotalSimWorkGroupCount aren't written and are hence undefined;
        //but thats not a problem, becaus we don't use them!
        if(currentNumResidentElements > 0)
        {
          uint currentElementStartIndex = gUniGridCells_ElementStartIndex[ globalNonCompatcedCellIndex ];
          uint numSimWorkGroupsOfThisCell = GET_NUM_SIM_WORK_GROUPS_OF_CELL( currentNumResidentElements );
          uint compactedIndex = groupCompactionOffset + gLocallyScannedTabulatedValues[ globalNonCompatcedCellIndex ];
           
          //split
          for(uint simGroupRunner=0; simGroupRunner < numSimWorkGroupsOfThisCell; simGroupRunner++ )
          {
            //compact
            gCompactedUniGridCells_NumElements[compactedIndex + simGroupRunner] = 
              ( 
                ( simGroupRunner < (numSimWorkGroupsOfThisCell - 1 ) )
                //for all but the last group, there are 32 elements in each sim work group
                ? (NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP)
                //the last sim work group gets the rest of the elements, namely currentNumResidentElements%32
                  //: ( BASE_2_MODULO( currentNumResidentElements, NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP) ) 
                : ( currentNumResidentElements % NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP ) 
              ); 
              
            gCompactedUniGridCells_ElementStartIndex[ compactedIndex + simGroupRunner ] = 
              currentElementStartIndex
              //increment start index by intervals of 32
              + (simGroupRunner << LOG2_NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP);    
          }  
           
        }//endif(currentNumResidentElements >0)
        
        globalNonCompatcedCellIndex += (NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN/2);
      }
        
    
    }
  {% endblock furtherImplementation %}
