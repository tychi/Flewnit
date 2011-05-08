
/*

  (Hopefully) highly optimized radix sort:
  Basic algorithm ideas are taken from http://http.developer.nvidia.com/GPUGems3/gpugems3_ch32.html
  and http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html;
  
  Though, there seem to be some mistakes an unnecessary bottle necks (doumentation follows);
  
  Furthermore, the parallel radix sort in the paper relies on the assumption that the radix counter arrays are that small
  that they can be scanned by one work group (i.e. max 2048 elements @ 1024 work items, for two elements).
  This can - without altering the algorithm - be only realized via either short element arrays (32k elements are max in the paper,
  but i need 256k) or via great serialization of key element count to yield short radix counter length 
  (serialization factor of 16 for 32k ky elements @ max. 1024 work items/work group --> for 256k keys, 8*16=128 elements would have 
  to be serialized; This is NOT an option!);
  
  Hence, I had to adopt some elements of the algorithms;
    - use of atomic_inc() instead of explicit serialization
    - local scan of the local radix counters already in the tabulation phase, so that the "global scan" in phase 2
      reduces its element count to NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE

  
  I don't try to enforce template usage at the moment; It rather confuses than helps me with structuring at the moment; 

  The goal of this thesis is to simulate 256k particles as efficently as possible;
  Due to severe time pressure, the early implementation might contain some "hardcodes" to achieve this
  goal asap; But I try to stay as generic as possible...
  
*/

  //####################################################################################
  //-------------------------------------------------------------------------------------
  {% include "scan.cl" %}
  //-------------------------------------------------------------------------------------
  //####################################################################################
  

  //Possible default  values: 
  //  for particles (sorted by radix sort) target in this thesis: 2^18 = 256k;
  //  for uniform grid cells (compacted by stream compaction) target in this thesis: 64^3 = 2^18 = 256k; 
  // -->  both numParticles and numUniGridCells are usually equal to 256k; this is a coincidence and does NOT mean that
  //      one shall try to reuse or merge some kernels just because they work on the same number of elements; This is a special case that will
  //      NOT be abused for any efforts of optimization
  #define NUM_TOTAL_ELEMENTS_TO_SCAN ( {{ numTotalElementsToScan }} )



  
  //--------------------------------------------------------------------------------------
    //default: 3 * log2(uniGridNumCellsPerDimension) = 18
    #define NUM_BITS_PER_KEY_TO_SORT ( {{numBitsPerKey}} )
    //goal: sort 6 bits per pass, i.e. we need 2^6=64 radices at once per pass so that we need only three passes for 18 bits
    #define NUM_RADICES_PER_PASS ( {{numRadicesPerPass}} )
    //default: log2(64)=6
    #define LOG2_NUM_RADICES_PER_PASS ( {{log2NumRadicesPerPass}} )
    //default: 18/6 = 3
    #define NUM_RADIX_SORT_PASSES ( NUM_BITS_PER_KEY_TO_SORT / LOG2_NUM_RADICES_PER_PASS )
  //-------------------------------------------------------------------------------------
 
  
  //-------------------------------------------------------------------------------------
    //"serialization amount" in order to
    //	- reduce memory needs and/or
    //  - reduce the length of the radix counter arrays to be scanned
    //	- do more work per item for small kernels to compensate kernel invocation/management overhead (doesn't apply to radix sort)
    // default proposal (fermi): 4; 
    // default proposal (GT200): <8>; 
    //this way, we need...
    #define NUM_KEY_ELEMENTS_PER_RADIX_COUNTER ( {{ numElementsPerRadixCounter }} )
    //... "only" 256k / 4<16> = 64k<16k> radix counters per radix;
    #define NUM_TOTAL_RADIX_COUNTERS_PER_RADIX ( NUM_TOTAL_ELEMENTS_TO_SCAN / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER )
  //-------------------------------------------------------------------------------------                                                                                        
   
           
                                                                                                                                                                            
  //-------------------------------------------------------------------------------------
  //hardware dependent memory amounts determine how many radix counters a work group can own
     // new defined by app ;(
     // { ifequal nvidiaComputeCapabilityMajor "2" }
     //   //take 32 kB of 48kB available
     //   #define LOCAL_MEM_FOR_RADIX_COUNTERS (32768)
     // { endifequal }
     // { ifequal nvidiaComputeCapabilityMajor "1" }
     //   //take 8kB of 16kB available
     //   #define LOCAL_MEM_FOR_RADIX_COUNTERS (8192)
     // { endifequal }
    #define LOCAL_MEM_FOR_RADIX_COUNTERS ( {{ localMemForRadixCounters }} )
    //for fermi architectures, this should be 2^15 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^7 = 128
    //for GT200 architectures, this should be 2^13 Bytes / (2^2 BytesPerCounter * 2^6 radix counter arrays) = 2^5 =  32
    #define NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ( LOCAL_MEM_FOR_RADIX_COUNTERS / ( 4 * NUM_RADICES_PER_PASS ) )
    //number of elements in local radix counter array (needed for initial clearing to zero)
    //default (fermi) : 128*64 = 8192
    //default (GT200) :  32*64 = 2048
    #define NUM_LOCAL_RADIX_COUNTER_ELEMENTS (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_RADICES_PER_PASS)
    
    //default (fermi): (128+128/32) * 64 = (128+4) * 64 = 8448
    //#define RADIX_COUNTERS_PADDED_STRIDE ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP + NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP/NUM_BANKS ) 
    //default (fermi): (128+32) * 64 =160
    //#define NUM_LOCAL_PADDED_RADIX_COUNTER_ELEMENTS ( RADIX_COUNTERS_PADDED_STRIDE * NUM_RADICES_PER_PASS )
  //-------------------------------------------------------------------------------------
  
  //-------------------------------------------------------------------------------------
    //one work item per key element, to that each work item can copy a key to __local memory
    //default (fermi): 128 radix counters per radix * 4 elements per radix counter = 512
    //default (GT200):  32 radix counters per radix * 8 elements per radix counter = 256
    #define NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE \
      ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER )
    //define explicitely the numbers of elements per tabulation work group: default: same as number of work items per work group
    #define NUM_ELEMENTS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE \
      ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER )
    //default (fermi) : 2^18/( 2^7 * 2^2) = 2^9 = 512
    //default (GT200) : 2^18/( 2^5 * 2^3) = 2^10 = 1024
    #define NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE ( NUM_TOTAL_ELEMENTS_TO_SCAN  / (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER) )
    
    //one local radix counter array can be scanned by half of its lenghtwork items;
    //example: local radix counter array lenght : 128 ; can be scanned by 128/2 = 64 elements
    //we have 64 of those arrays; we have 512 work items we can scan 512 / ( 128/2 ) = 8 arrays in parallel
    #define NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL                              \
      (                                                                               \
        NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE /               \
        ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP / 2 )                           \
      ) 
      
    //BULLSHIT! :
    //#define NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL (                          \
    // (NUM_LOCAL_RADIX_COUNTER_ELEMENTS)                                             \
    //  /                                                                             \
    //  ( 2 * (NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE) ) )
    //#define NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL ( (2*NUM_ELEMENTS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE ) / NUM_LOCAL_RADIX_COUNTER_ELEMENTS )
  //-------------------------------------------------------------------------------------





  

  //-------------------------------------------------------------------------------------
  uint getRadix(uint key, uint numPass)
  {
    //shift relevant radix to LSB, mask higher bits
    return ( key >> (numPass * LOG2_NUM_RADICES_PER_PASS ) ) & (NUM_RADICES_PER_PASS -1) ;
  }
  //-------------------------------------------------------------------------------------




  //=====================================================================================
  //phase one out of three in a radix sort pass: tabulate and local scan
  __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE,1,1))) 
  void kernel_radixSort_tabulate_localScan_Phase(
    __global uint* gKeysToSort,     //NUM_TOTAL_ELEMENTS_TO_SCAN elements
    __global uint* gLocallyScannedRadixCounters,  //NUM_RADICES_PER_PASS * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX elements; (e.g. 64*64k);
                                    //is logically an array NUM_RADICES_PER_PASS radix counter arrays;
                                    //In phase 1 (tabulation and local scan), the results of local scans of the counters 
                                    //  are written to this array;
                                    //In phase 3, it serves as "local" offset for the own radix 
    __global uint* gSumsOfLocalRadixCounts, // NUM_RADICES_PER_PASS * NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE  elements (e.g. 64 * 512);
                                    //In phase 1, the sums of the local scans of the radix counters will be written to it;
                                    //In phase 2, each of these NUM_RADICES_PER_PASS counter arrays will be scanned
                                    //  to yield the "own radix offsets" to be added to the final reorder-offset
                                    //  SUGGESTION: for read coalescing purposes in phase 3, the scan results of this array 
                                    //              should be written back TRANSPOSED in phase 2 
                                    //              (i.e. logically, it would not be an array of NUM_RADICES_PER_PASS counter arrays 
                                    //              anymore, but an array of NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE counter set arrays).
                                    //              But as a matrix transpose memory access patterns are only efficient if there
                                    //              are at least as many coloumns as the cache line is big, and as due to big row lenghts
                                    //              there don't fit many columns into the local memory, a transposition seems not a good option;
                                    //              In a far future ;), one could try to use two 2d images intead of a standard buffer for this purpose;
                                    //              But this optimazation is not THAT urgent, because for nearly-sorted key arrays, adjacent elements
                                    //              are likely to have the same radix; hence, the worst case of NUM_RADICES_PER_PASS reads of 
                                    //              128 byte cache lines where we need only NUM_RADICES_PER_PASS*4 byte is not that likely to happen 
                                    //              during simulation
                                    //In phase 3, it serves as "global" offset for the own radix 
    
    uint numPass
  )
  {
    //(e.g. 64*(128+4)=8448 elements for fermi. 64*(32+2)=2176 elements for GT200 );
    //is logically an array of NUM_RADICES_PER_PASS padded radix counter arrays;
    __local uint lRadixCounters[  PADDED_STRIDE( NUM_RADICES_PER_PASS * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ) ]; 
                                   
    __local uint lTotalLocalRadixSums[ NUM_RADICES_PER_PASS ];
  
    uint radix = getRadix( gKeysToSort[get_global_id(0)] ,numPass);
  
    uint lwiID = get_local_id(0); // short for "local work item ID"
    
    uint localCounterIndex = lwiID / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER;
    //localPaddedCounterIndex += CONFLICT_FREE_OFFSET(localCounterIndex );
    //uint localPaddedCounterIndex = CONFLICT_FREE_OFFSET(localCounterIndex );
   
    //clear to local radix counters to zero
    //will be unrolled to be executed e.g.  8192*(1+1/32)/(512*(1+1/32))=8448/528=16 times without bank conflicts
    //#pragma unroll
    /*
    for ( uint offset =0; 
          offset < PADDED_STRIDE(NUM_LOCAL_RADIX_COUNTER_ELEMENTS); 
          offset += PADDED_STRIDE(NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE)
    )
    {
      lRadixCounters[offset + localPaddedCounterIndex] = 0;
    }
    */
    
    for ( uint offset =0; 
          offset < NUM_LOCAL_RADIX_COUNTER_ELEMENTS;
          offset += NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE
    )
    {
        lRadixCounters[ CONFLICT_FREE_INDEX( offset + lwiID ) ] = 0;
        //lRadixCounters[ CONFLICT_FREE_INDEX( offset + lwiID ) ] = 1;
    }
    
    //synchronize, because the tabulation of an element is not necessaryly performed by the same work item having done the copy
    //there is no way to circumvent the synch withour causing bank conflicts, i.e. "transposing" the pseudo 2d array
    //increases the danger of bank conflicts
    barrier(CLK_LOCAL_MEM_FENCE);
   

    atom_inc( 
      &(
        lRadixCounters[
          //select the counter array for the radix of the current value
          CONFLICT_FREE_INDEX( 
            radix * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  +
            //select the appropriate counter
            localCounterIndex 
          )
        ]
      )
    );



/*
    atom_inc( 
      &(
        lRadixCounters[
          //select the counter array for the radix of the current value
          CONFLICT_FREE_INDEX( 
             localCounterIndex * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  +
             //select the appropriate counter
             radix
          )
        ]
      )
    );
*/

    
    
    //------------------------------------------------------------------------------------------
    
      //divide work items in several sets with interval [0..half length of one radix counter array], 
      //as each work item can scan two radix counter elements
      //default (fermi): [0..511] % (128/2) --> 8 occurences of interval [0..63]
      //default (GT200): [0..256] % (32/2) --> 16 occurences of interval [0..15] <-- on half warp scans another array than the other.. 
      //                                                                              but the control flow should not diverge worse 
      //                                                                              than when scanning only one array with few elements...                                                      
      uint workItemIDForScanOfOneCounterArray = 
       ( lwiID ) % ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP / 2 );

                              
      uint radixOffsetWhereWorkItemParticipates =
        ( lwiID ) / ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP / 2 ); 
    
    
    //do the "local" radix scan on every radix, partially in parallel to use all work items:
    //TODO check if an unroll yields better performance 
    for(uint i= 0; i < ( NUM_RADICES_PER_PASS / NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL ) ; i++)
    {

      //radix in whose scan the curren work item will participate = 
      //default (fermi): [0..7] *  8 + (2* [0..511])/128 --> [0..7] *  8 + [0 .. 7]
      //default (GT200): [0..4] * 16 + (2* [0..255])/ 32 --> [0..7] *  8 + [0 ..15]
      uint radixToScan = 
        //big loop offset 
        i * NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL 
        //+ work item id / (half size of one local radix counter array)
        + radixOffsetWhereWorkItemParticipates;
/*
      uint radixToScan = 
        i * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP /  NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL 
        + ( lwiID ) / ( NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL  / 2 ); 
*/
    

     

        scanExclusive(
          //calculate the pointer to the first element in the "array of radix counter arrays"
          //lRadixCounters +  PADDED_STRIDE( radixToScan * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  ),
          &(
            lRadixCounters[ PADDED_STRIDE( radixToScan * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ) ]
          ), 
          //grab the total sum
          & ( lTotalLocalRadixSums[ radixToScan ] ),
          //as many elements to scan as we have radix counter elements per radix
          NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP, 
          //communicate the stride [0.. half size of one radix counter array] to the scan function so that it can operate
          //indepentently of its actual work item id
          workItemIDForScanOfOneCounterArray
        );


      
      //write the total sum of the local counter array to global memory
      if( workItemIDForScanOfOneCounterArray == 0)
      {
        gSumsOfLocalRadixCounts[
          //select the counter array for the recently scanned radix counte array
          radixToScan * NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE
          //entry in counter array corresponds to work group
          + get_group_id(0)
        ] = lTotalLocalRadixSums[ radixToScan ];

/*
        gSumsOfLocalRadixCounts[
          //select the counter array for the recently scanned radix counte array
          get_group_id(0) * NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE
          //entry in counter array corresponds to work group
          + radixToScan
        ] = lTotalLocalRadixSums[ radixToScan ];
*/
      }
      
        
      //write the scan results to global memory: as one work item scans two counters, it also has to write out two of them
      //TODO precompute some index stuff? would save calculations but disturb superscalarity... subject to experimentation
      //#pragma unroll
      for(int i=0; i<2; i++)
      {
        gLocallyScannedRadixCounters[ 
          //select "radix row" in pseudo 2d array
          radixToScan * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX 
          + get_group_id(0) * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP
          //write the i'th half of the current array with the  NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP/2 work items
            + i * (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP >> 1)
            + workItemIDForScanOfOneCounterArray
        ] 
        //= get_local_id(0);

        = lRadixCounters[
          //select "radix row" in pseudo 2d array taking padding into account
           CONFLICT_FREE_INDEX( 
              radixToScan * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP
             //write the i'th half of the current array with the  NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP/2 work items
              + i * (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP >> 1) + 
              workItemIDForScanOfOneCounterArray 
           )
        ];


      }

    } //endfor sequential part of scanning the NUM_RADICES_PER_PASS radix counters
      
   

   
  }
  //=====================================================================================
  
  
  
  
  //=====================================================================================
  //phase two out of three in a radix sort pass: global scan
  
 
  //Elements to scan in phase 2 is the same as there were work groups in phase 1
  //reason:   In phase 2, the results of the local radix counter scans of phase 1 are scanned, so we have to sum up 
  //          one element per phase 1 work group;
   #define NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE ( NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE )
  //half as many work items per work group as there are elements to scan
  #define NUM_WORK_ITEMS_PER_WORK_GROUP__GLOBAL_SCAN_PHASE ( NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE >> 1 )
  //Because the structure of the algorithm demands that the global scan can be performed by a single work group,
  //theoretically, phase 2 can be completely performed by a single work group; But this approach woudn't scale
  //with the number of multiprocessors in a device; Hence, for perfect load balancing, we take 
  //pow2(ceil(log2(CL_DEVICE_MAX_COMPUTE_UNITS))) work groups (value calced by app), 
  //i.e. the number of multiprocessors, rounded to the next power of two
  #define NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE  ( NUM_COMPUTE_UNITS_BASE2_CEILED )
  //default value for Geforce GT  435M (fermi) : (64 / pow2(ceil(log2( 2))) = 32
  //default value for Geforce GTX 280  (GT200) : (64 / pow2(ceil(log2(30))) = 2
  //default value for Geforce GTX 570  (fermi) : (64 / pow2(ceil(log2(15))) = 4
  //default value for Geforce GTX 580  (fermi) : (64 / pow2(ceil(log2(16))) = 4
  #define NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE (NUM_RADICES_PER_PASS / NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE )
  
  __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP__GLOBAL_SCAN_PHASE,1,1))) 
  void kernel_radixSort_globalScan_Phase(
    __global uint* gSumsOfLocalRadixCountsToBeScanned, //is the gSumsOfLocalRadixCounts from phase 1;
                                    //NUM_RADICES_PER_PASS * NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE elements (e.g. 64*512);
                                    //In phase 1, the sums of the local scans of the radix counters will be written to it;
                                    //In phase 2, each of these NUM_RADICES_PER_PASS counter arrays will be scanned
                                    //  to yield the "own radix offsets" to be added to the final reorder-offset
                                    //  SUGGESTION: for read coalescing purposes in phase 3, the scan results of this array 
                                    //              should be written back TRANSPOSED in phase 2 
                                    //              (i.e. logically, it would not be an array of NUM_RADICES_PER_PASS counter arrays 
                                    //              anymore, but an array of NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE counter set arrays).
                                    //              But as a matrix transpose memory access patterns are only efficient if there
                                    //              are at least as many coloumns as the cache line is big, and as due to big row lenghts
                                    //              there don't fit many columns into the local memory, a transposition seems not a good option;
                                    //              In a far future ;), one could try to use two 2d images intead of a standard buffer for this purpose;
                                    //              But this optimazation is not THAT urgent, because for nearly-sorted key arrays, adjacent elements
                                    //              are likely to have the same radix; hence, the worst case of NUM_RADICES_PER_PASS reads of 
                                    //              128 byte cache lines where we need only NUM_RADICES_PER_PASS*4 byte is not that likely to happen 
                                    //              during simulation
                                    //In phase 3, it serves as "global" offset for the own radix 
    __global uint* gPartiallyScannedSumsOfGlobalRadixCounts, //NUM_RADICES_PER_PASS elements (e.g. 64); 
                                    //In phase 2, the total sum of each radix counter array will be calculated,
                                    //  then a partial scan (NUM_RADICES_PER_PASS / NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE), e.g. (64/2)=32
                                    //  will be performed by the NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE and uploaded to this array
                                    //In phase 3, this array is completely read to shared memory, and "the rest" of the scan is performed,
                                    //  namely NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE are scanned
   __global uint* gSumsOfPartialScansOfSumsOfGlobalRadixCounts, //NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE elements used (e.g. 2 for geforce GT 435M);
                                    //Naming of all those buffers for  scans of scans of scans becomes weird at the latest now; 
                                    //This buffer represents the fourth and most coarse layer of "counter" granularity; 
                                    //Each work group of phase 2 sets 
                                    //  gSumsOfPartialScansOfSumsOfGlobalRadixCounts[groupID]= totalSumOfPartialScan;
                                    //In phase 3, this very small array *** is scanned and the final reorderd position is
                                    //calculated from all those offset values;
                                                                  
                                    // *** (worst case: size 32 for GTX280 with its 30 multiprocessors,
                                    //Fermi devices have at most 16 multiprocessors (GTX580), instead 4 to 6 times the 
                                    //execution units count per Multiprocessor)
   
    uint numPass
  )
  {
     //One padded array to scan must fit in;
     // example size (fermi) :  512 *(1+1/32) =  528 elements
     // example size (GT200) : 1024 *(1+1/16) = 1088 elements
     __local uint lSumsOfLocalRadixCounts[ PADDED_STRIDE( NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE ) ];
     
     // (e.g. 33 elements for Geforce GT435M);
     //Although we need to comupte and store only NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE, in order
     //to keep control flow and business logic simple (i.e. to not corrupt the structure of the
     //_ex_clusive scan), one value past the actual array length will be written; this saves a
     //costly if-statement within the scan loop.
     //A _sequential_ scan will be performed, as the different radix counter arrays are scanned
     //consecutively; one work item has to grab the total sum of each global radix scan,
     //add the sums of the previous global radix scans and write the result to the appropriate element
     //in the local array; The final result will be a partial scan on the total radix sums on
     //which one work group has worked on. (The final scan will be done in phase 3, as global synch is needed)
     //Note: because of the sequential scan, this array needs no padding (besides the one additinal element), 
     //becaus no bank conflicts can occur.  
     __local uint  lSumsOfGlobalRadixCounts[ NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE + 1 ];
  
      //__local uint lTotalRadixSum;
      //grab the total sum frome the global scan of ech radix;
      //this value is needed to create the partial scan of the total sum of each radix counter array 
      __local uint lTempTotalSumOfCurrentGlobalRadixCounterArray;
  
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint groupID = get_group_id(0);
    
    uint radixStart =  groupID     *   NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE;
    uint radixEnd   = (groupID +1) *   NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE - 1;
    
    if(lwiID == 0)
    {
      //init start to zero (we do exclusive scans throughout the whole simulation)
      lSumsOfGlobalRadixCounts[0] = 0;
    }
    
    //barrier(CLK_LOCAL_MEM_FENCE); //barrier so that lSumsOfGlobalRadixCounts is zero for every work item
    
    uint paddedLocalLowerIndex  = CONFLICT_FREE_INDEX( lwiID );
    uint paddedLocalHigherIndex = CONFLICT_FREE_INDEX( lwiID + (NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE/2) );
    

    //do not unroll, too much code
    for(int currentRadix= radixStart; currentRadix <= radixEnd; currentRadix++ ) //smaller or EQUAL
    {   
    
      //{ index calculation for global and local element arrays:   
      uint globalLowerIndex = 
          //select the counter array for the recently scanned radix counter array
          currentRadix * NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE
          //entry in counter array corresponds to work item
          + lwiID;
      //TODO optimize for super scalarity: better repeat calculations instead of provoḱing a read-after-write-hazard?
      //uint globalHigherIndex = globalLowerIndex + (NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE/2);
      uint globalHigherIndex =  
          currentRadix * NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE
          //entry in counter array corresponds to work item + half the size of array to scan
          + lwiID + (NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE/2);
      
      //}
     
      //read coalesced from global memory
      lSumsOfLocalRadixCounts[ paddedLocalLowerIndex  ] =   gSumsOfLocalRadixCountsToBeScanned[ globalLowerIndex  ];
      lSumsOfLocalRadixCounts[ paddedLocalHigherIndex ] =   gSumsOfLocalRadixCountsToBeScanned[ globalHigherIndex ];

      barrier(CLK_LOCAL_MEM_FENCE); //ensure all values are read from global mem;
      
        scanExclusive(
          lSumsOfLocalRadixCounts,
          & lTempTotalSumOfCurrentGlobalRadixCounterArray, 
          //as many elements to scan as we have radix counter elements per radix
          NUM_ELEMENTS_TO_SCAN_PER_WORK_GROUP__GLOBAL_SCAN_PHASE, 
          lwiID
        );
        
      //barrier(CLK_LOCAL_MEM_FENCE); //TODO remove, this should be completely unneccessary because the last instruction in 
                                    //scanExclusive is ab barrier; but to be save from compiler bugs and 
                                    //error in reasoning, in case of doubt, do a barrier;
        
      if(lwiID == 0)
      {
        uint localIndexSumsOfGlobalRadix = currentRadix - radixStart;
        //sequential scan of the total count of each radix ocurrence;
        lSumsOfGlobalRadixCounts[localIndexSumsOfGlobalRadix + 1 ] = 
            lSumsOfGlobalRadixCounts[ localIndexSumsOfGlobalRadix ] + lTempTotalSumOfCurrentGlobalRadixCounterArray;
      }
      
      //write back coalesced to global memory
      gSumsOfLocalRadixCountsToBeScanned[ globalLowerIndex  ] =   lSumsOfLocalRadixCounts[ paddedLocalLowerIndex  ];
      gSumsOfLocalRadixCountsToBeScanned[ globalHigherIndex ] =   lSumsOfLocalRadixCounts[ paddedLocalHigherIndex ];
    
      
      barrier(CLK_LOCAL_MEM_FENCE); //ensure all values are written to global mem;
      //barrier(CLK_GLOBAL_MEM_FENCE); //TODO delete semes unneccessary to me ;(; but to be sure...
      
      
    } //end sequential "scan of parallel scans" ;)
   
     barrier(CLK_LOCAL_MEM_FENCE); //TODO is it necessary?
    
    //in the end, write the results of the scan of the work-group-assigned interval of the total radix sums
    //back to global memory:
    if(lwiID < NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE)
    {
      gPartiallyScannedSumsOfGlobalRadixCounts[radixStart + lwiID] = lSumsOfGlobalRadixCounts[lwiID];
    }
    
    if(lwiID == 0)
    {
      //set the total sum of the sequential scan of the total sums of the global radix counters
      //(is in the last element of the [NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE +1]-elements lSumsOfGlobalRadixCounts array)
      gSumsOfPartialScansOfSumsOfGlobalRadixCounts[ groupID ] = 
        lSumsOfGlobalRadixCounts[NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE];
    }
    
  }
  //=====================================================================================
  
  
  
  
  //=====================================================================================
  //phase three out of three in a radix sort pass: reorder
  //number of work items corremponds to that of phase one, so this is no mistake:
  __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE,1,1))) 
  void kernel_radixSort_reorder_Phase(
    //following key ping pong buffers: (We need ping pong buffers, because although input and output indices are a perfect permutation
    //of each other, read from and write to the same buffer would need global synchronization)
    __global uint* gKeysToSort,     //NUM_TOTAL_ELEMENTS_TO_SCAN elements; 
                                    //needed to select the relevant radix for reordering and to be scattered to reorderedKeys;
    __global uint* gReorderedKeys,    //NUM_TOTAL_ELEMENTS_TO_SCAN elements; 
                                    //The reordered keys are written to this buffer,which we may needed in following
                                    //radix sort passes to sort them according to the next radix interval;
                                    //Furthermore, in physics simulation using a uniform grid as acceleration structure,
                                    //the keys are needed to determine the start and end object within a cell.

    //following value ping pong buffers:                                                                 
    __global uint* gOldIndices,   //NUM_TOTAL_ELEMENTS_TO_SCAN elements; 
                                    //Because there are several radix sort passes needed
                                    //until a high-bit key array is completely sorted, it would be madness to reorder (and hence scatter uncoalesced) 
                                    //EVERY value associated to this key (e.g. position vectors, velocity vectors etc.)
                                    //in every pass; Hence, while radix sorting, we use just the old indices to reorder them,
                                    //so that after completion of all radix sort passes, you just have to reorder the several 
                                    //"big associated value arrays" once. (There is an issue, though, as this "deferred reordering" 
                                    //brings the danger of uncoalesced reads from global memory; But in the context of physics simulation
                                    //input arrays are nearly sorted, as objects do not move more than one voxel at a time step. Hence,
                                    //a relatively great amount of coalescing is still implicitely provided.
                                    //In first radix sort pass, this array is unused and get_global_id(0) is taken instead as initial
                                    //index.                                  
    __global uint* gReorderedOldIndices,//NUM_TOTAL_ELEMENTS_TO_SCAN elements;   


    
    //following the different granularities of radix counter scans to compute the new index of each key/value element;
    __global uint* gLocallyScannedRadixCounters,  //NUM_RADICES_PER_PASS * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX elements; (e.g. 64*64k);
                                    //is logically an array NUM_RADICES_PER_PASS radix counter arrays;
                                    //In phase 1 (tabulation and local scan), the results of local scans of the counters (stride e.g. 128 for fermi)
                                    //  are written to this array;
                                    //In phase 3, it serves as "local" offset for the own radix 
    __global uint* gScannedSumsOfLocalRadixCounts, //is the gSumsOfLocalRadixCountsToBeScanned from phase 2, NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE elements
                                    //In phase 3, it serves as "global" offset for the own radix 
    __global uint* gPartiallyScannedSumsOfGlobalRadixCounts, //NUM_RADICES_PER_PASS elements (e.g. 64); 
                                    //In phase 2, the total sum of each radix counter array will be calculated,
                                    //  then a partial scan (NUM_RADICES_PER_PASS / NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE), e.g. (64/2)=32
                                    //  will be performed by the NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE and uploaded to this array
                                    //In phase 3, this array is completely read to shared memory, and "the rest" of the scan is performed,
                                    //  namely NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE are scanned   
    __global uint* gSumsOfPartialScansOfSumsOfGlobalRadixCounts, //NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE elements used (e.g. 2 for geforce GT 435M);
                                    //Naming of all those buffers for  scans of scans of scans becomes weird at the latest now; 
                                    //This buffer represents the fourth and most coarse layer of "counter" granularity; 
                                    //Each work group of phase 2 sets 
                                    //  gSumsOfPartialScansOfSumsOfGlobalRadixCounts[groupID]= totalSumOfPartialScan;
                                    //In phase 3, this very small array *** is scanned and the final reorderd position is
                                    //calculated from all those offset values;
                                                                  
                                    // *** (worst case: size 32 for GTX280 with its 30 multiprocessors,
                                    //Fermi devices have at most 16 multiprocessors (GTX580), instead 4 to 6 times the 
                                    //execution units count per Multiprocessor)
                                                                
    uint numPass
  )
  {
  
    
    //locally cached keys 'n values because of otherwise uncoalesced global memory access 
    //due to serialization of scatter and radix counter incrementation                                    
    __local uint lKeysToSort[ PADDED_STRIDE( NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE ) ]; //padding due to interleaved reads
    __local uint lOldIndices[ PADDED_STRIDE( NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE ) ]; //padding due to interleaved reads
           
                           
    //Similar usage as in phase 1;
    //Note that in constrast to phase 1, this buffer needs no padding, because there is no scan to be performed :).
    //Is logically an array of NUM_RADICES_PER_PASS _NON_padded radix counter arrays;
    //(has e.g. 64*(128)=8192 elements for fermi. 64*(32)=2048 elements for GT200 );
    //NOTE:  because one work group needs only one "column" in gScannedSumsOfLocalRadixCounts and hence every needed element
    //       must be read uncoalesced, these values won't be explicitely cached to local memory, as this would inevitably mean
    //       the worst case bandwidth wasting 
    //         (e.g. 64 elements * 128 bytes per cache line (fermi) = 8192 bytes for (at most) 64 * 4 bytes per value = 256 bytes needed);
    //       Because of the often mentioned "nearly-sorted" property of the input keys, the radices of adjacent elements are highly
    //       likely to be the same; So there may not every element in the relevant "column" beeing actually needed, saving bandwitdh;
    //       A scenario like this _might_ profit from the 16kB L1 cache of the fermi architecture (in case a warp needs the same element
    //       already grabbed by another warp).
    __local uint lLocallyScannedRadixCounters[ NUM_RADICES_PER_PASS * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ];
    
    //copy of gPartiallyScannedSumsOfGlobalRadixCountsb because of the NUM_KEY_ELEMENTS_PER_RADIX_COUNTER reads
    __local uint lPartiallyScannedSumsOfGlobalRadixCounts[ NUM_RADICES_PER_PASS ];
 
    
    // e.g. 2+0=2 elements for GT435M
    //copy of gSumsOfPartialScansOfSumsOfGlobalRadixCounts, so that it can be scanned;
    __local uint lSumsOfPartialScansOfSumsOfGlobalRadixCounts[ PADDED_STRIDE( NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE ) ];
    
    
    __local uint lDummyScanTotalSum; //unused, but checking for a NULL pointer is too costly; let's pass this to scanExclusive();
    
    
  
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0);
    
    //for fermi default: [0,0,0,0,1,1,1,1,2,2,2,2,...,127,127,127,127]
    //uint localRadixCounterIndexForOwnKeyElement = lwiID / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER;
    
    uint paddedLocalCopyIndex = CONFLICT_FREE_INDEX( lwiID );
    


  //{ setup the local buffers:
    
      //keys: padded copy due to strided local reads because of serialization of radix counter increments
      lKeysToSort[ paddedLocalCopyIndex ] = gKeysToSort[gwiID]; 

      //the values, being actually indices of the position of the elements before the radix sort:
      // padded copy due to strided local reads because of serialization of radix counter increments
      if(numPass == 0)
      {
        //initial pass: value index corresponds to global work item id :)
        lOldIndices[ paddedLocalCopyIndex ] = gwiID;
      }else
      {
        //non-initial pass: old indices have already been subverted by previous scatters; grab those from buffer;
        lOldIndices[ paddedLocalCopyIndex ] = gOldIndices[gwiID];
      }
      
      
/*
      //{ lame not fully parallel radix download for bug hunting :(
        if(lwiID < NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP )
        {
          for(uint currentRadix = 0; currentRadix < NUM_RADICES_PER_PASS; currentRadix++)
          {
             lLocallyScannedRadixCounters[
                  //select "radix row"
                  currentRadix * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  
                  //select element in "radix row"
                  + lwiID                                      
                 ]
                 = 
                 gLocallyScannedRadixCounters[
                  //select "radix row"
                  currentRadix * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX
                  //select "work group radix stripe"
                  + groupID * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP
                  //select element in "work group radix stripe"
                  + lwiID
                 ];  
          }
        }
      //}

*/
      
        

      //for fermi default: 4* [0..128]
      uint localRadixCounterCopyIndex = 
        lwiID % NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ; //TODO restor base2 stuff when stable
        //BASE_2_MODULO( lwiID, NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP );
     
        
      //{ parallel local radix download...
       
        //e.g. 512/128 = 4
        #define NUM_RADICES_TO_DOWLOAD_IN_PARALLEL  \
          ( NUM_WORK_ITEMS_PER_WORK_GROUP__TABULATION_PHASE_REORDER_PHASE \
            / NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP   )
        
        #define NUM_RADIX_DOWLOAD_ITERATIONS \
          ( NUM_RADICES_PER_PASS / NUM_RADICES_TO_DOWLOAD_IN_PARALLEL )
        
        
        //work item id / (size of one local radix counter array)
        //default (fermi): [0..511]/128= [ 128*0,128*1,128*2,128*3 ]
        uint radixOffsetToParticipateInDowload = lwiID / NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP; 
        
        for(uint i= 0; i < NUM_RADIX_DOWLOAD_ITERATIONS; i++)
        {
          //radix in whose copy the current work item will participate = 
          uint radixToCopy = 
            i * NUM_RADICES_TO_DOWLOAD_IN_PARALLEL
            + radixOffsetToParticipateInDowload;
            
          lLocallyScannedRadixCounters[
              //select "radix row"
              radixToCopy * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  
              //select element in "radix row"
              + localRadixCounterCopyIndex                                      
             ]
             = 
             gLocallyScannedRadixCounters[
              //select "radix row"
              radixToCopy * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX
              //select "work group radix stripe"
              + groupID * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP
              //select element in "work group radix stripe"
              + localRadixCounterCopyIndex  
             ];  
        }
      
      //}
      
 
      //-------------------------------------------------
      if(lwiID < NUM_RADICES_PER_PASS )
      {
        lPartiallyScannedSumsOfGlobalRadixCounts[lwiID] = gPartiallyScannedSumsOfGlobalRadixCounts[lwiID];
      }

      //-------------------------------------------------
      if(lwiID < NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE )
      {
        //padded copy due to scanning
        lSumsOfPartialScansOfSumsOfGlobalRadixCounts[ paddedLocalCopyIndex ] = gSumsOfPartialScansOfSumsOfGlobalRadixCounts[lwiID];
      }
      //-------------------------------------------------
      
      barrier(CLK_LOCAL_MEM_FENCE);
            
    //} //end setup of local buffers
   
   
   
    //{ scan the coarsest granularity   
    //default for GT435M 2/2=1; yes ,really, it is not worth such an invocation on such a device, but i wanna stay general
    //and not optimize for a single graphics card ;)
          
    //check if we even have more than one compute units, otherwise the scan would not be called and hence the first element
    //in lSumsOfPartialScansOfSumsOfGlobalRadixCounts wouldn't be zero and corrupt the offsetting; we have to catch this situation.
    #if NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE > 1
    
      //masking of too great lwiIDs' happens within scanExclusive!
      //becaus it uses barrier(), it must be called by every work item, else corruption occurs!
      //never call barrier in a code segmant not executed by ALL work items!!11
      scanExclusive(lSumsOfPartialScansOfSumsOfGlobalRadixCounts, & lDummyScanTotalSum, NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE, lwiID );
      
    #else
      if( lwiID == 0 )
      {
        lSumsOfPartialScansOfSumsOfGlobalRadixCounts[0]=0;
      }  
      barrier(CLK_LOCAL_MEM_FENCE);
    #endif
    //}

       
    //{
      //the first threads do the radix increment and sequential scatter to ensure that a warp doesn't diverge 
      //and that only a few warps stay active
      if(lwiID < NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP)
      {
        uint key;
        uint oldIndex;
        uint radix;
        
        //TODO check if unroll amortizes or not
        //#pragma unroll
        for(uint i=0 ; i< NUM_KEY_ELEMENTS_PER_RADIX_COUNTER; i++ )
        {
          //default (fermi): 4* [0..127] + [0..3]
          uint localElementIndex = 
            lwiID * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER  
            + i;
          uint localpaddedElementIndex = CONFLICT_FREE_INDEX( localElementIndex );
                  
          //strided access requiring padding
          key = lKeysToSort[ localpaddedElementIndex ];
          oldIndex = lOldIndices[ localpaddedElementIndex ]; 
          radix = getRadix( key,numPass);
          
          uint newIndexInSortedArray =
            
            //level 3 offset: "global" offset to previous radices
            //offset by the "scan of the partial scans of the total radix counts":
            lSumsOfPartialScansOfSumsOfGlobalRadixCounts[ 
              //CONFLICT_FREE_INDEX (  radix / NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE ) <<-- horribly wrong! shame on me ;(
              CONFLICT_FREE_INDEX (  radix  / NUM_RADICES_PER_WORK_GROUP__GLOBAL_SCAN_PHASE )
            ] 
            
            //level 2 offset: "local" offset to previous radices
            //offset by the "partial scans of the total radix counts":
            + lPartiallyScannedSumsOfGlobalRadixCounts [ radix ]
            
            //level 1 offset: "global" offset within own radix
            //offset by the "global radix count" for the current radix:
            + gScannedSumsOfLocalRadixCounts[ 
              radix * NUM_WORK_GROUPS__TABULATION_PHASE_REORDER_PHASE 
              + groupID 
             ]
             
            //level 0 offset: "local" offset within own radix
            //offset by the "local radix count" for the current radix and increment the radix counter
            //so that the following key/value pair of the element group sharing the same counter with possibly the same radix
            //is offset one position further than the preceding one:
            + 
            (   
                lLocallyScannedRadixCounters[ 
                  radix * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  
                  //+ localElementIndex   <<-- another time a horrible indexing mistake! shame on me again;(
                  +lwiID // <-- this is the local radix counter index for the current element;
                ]
                ++ //<-- hope there is no driver bug .. post increment on a buffer value .. who knows..
            );
            
            //and, at last, after in total around 800 lines of code and comments, do the SCATTER!!!1
            
            gReorderedKeys[newIndexInSortedArray] = key;
            gReorderedOldIndices[newIndexInSortedArray] = oldIndex;
            
            
            //debug HACK: track buffer overflows:
            //if( newIndexInSortedArray >= get_global_size(0))
            //{
            //  atom_inc(gSumsOfPartialScansOfSumsOfGlobalRadixCounts +   NUM_WORK_GROUPS__GLOBAL_SCAN_PHASE );             
            //}
                  

        }
      }
    //}



    
  }
  //=====================================================================================
  
  
 
  
  
  
  

  
  
