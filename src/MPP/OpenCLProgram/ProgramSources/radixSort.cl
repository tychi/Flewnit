#pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable


/*

  (Hopefully) highly optimized radix sort:
  Basic algorithm ideas are taken from http://http.developer.nvidia.com/GPUGems3/gpugems3_ch32.html
  and http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html;
  
  Though, there seem to be some mistakes an unnecessary bottle necks (doumentation follows);
  
  Furthermore, the parallel radix sort in the paper relies on the assumption that the radix counter arrays are that small
  that they can be scanned by one work group (i.e. max 2048 elements @ 1024 work items, in for two elements).
  This can - without altering the algorithm - only realized via either short element arrays (32k elements are max in the paper,
  but i need 256k) or via great serialization of key element count to yield short radix counter length 
  (serialization factor of 16 for 32k ky elements @ max. 1024 work items/work group --> for 256k keys, 8*16=128 elements would have 
  to be serialized; This is NOT an option!);
  
  Hence, I had to adopt some elements of the algorithms;
    - use of atomic_inc() instead of explicit serialization
    - local scan of the local radix counters already in the tabulation phase, so that the "global scan" in phase 2
      reduces its element count to NUM_TABULATE_WORK_GROUPS

  
  I don't try to enforce template usage at the moment; It rather confuses than helps me with structuring at the moment; 

  The goal of this thesis is to simulate 256k particles as efficently as possible;
  Due to severe time pressure, the early implementation might contain some "hardcodes" to achieve this
  goal asap; But I try to stay as generic as possible...
  
*/

 
  //target in this thesis: 2^18 = 256 k, both for numParticles and for numUniGridCells
  #define NUM_TOTAL_ELEMENTS ( {{ numTotalElements }} )
  
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
    #define NUM_KEY_ELEMENTS_PER_RADIX_COUNTER ( {{numElementsPerRadixCounter }} )
    //... "only" 256k / 4<16> = 64k<16k> radix counters per radix;
    #define NUM_TOTAL_RADIX_COUNTERS_PER_RADIX ( NUM_TOTAL_ELEMENTS / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER )
  //-------------------------------------------------------------------------------------                                                                                        
   
   
  //-------------------------------------------------------------------------------------          
  {% include bankConflictsAvoidance.cl %}          
  //-------------------------------------------------------------------------------------          
                                                                                                                                                                            
  //-------------------------------------------------------------------------------------
  //hardware dependent memory amounts determine how many radix counters a work group can own
    {% ifequal nvidiaComputeCapabilityMajor 2 %}
      //take 32 kB of 48kB available
      #define LOCAL_MEM_FOR_RADIX_COUNTERS (32768)
    {% endifequal nvidiaComputeCapabilityMajor 2 %}
    {% ifequal nvidiaComputeCapabilityMajor 1 %}
      //take 8kB of 16kB available
      #define LOCAL_MEM_FOR_RADIX_COUNTERS (8192)
    {% endifequal nvidiaComputeCapabilityMajor 1 %}
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
    #define NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER)
    //define explicitely the numbers of elements per tabulation work group: default: same as number of work items per work group
    #define NUM_TABULATE_ELEMENTS_PER_WORK_GROUP (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER)
    //default (fermi) : 2^18/( 2^7 * 2^2) = 2^9 = 512
    //default (GT200) : 2^18/( 2^5 * 2^3) = 2^10 = 1024
    #define NUM_TABULATE_WORK_GROUPS ( NUM_TOTAL_ELEMENTS  / (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_KEY_ELEMENTS_PER_RADIX_COUNTER) )
    
 
    #define NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL ( (2*NUM_TABULATE_ELEMENTS_PER_WORK_GROUP ) / NUM_LOCAL_RADIX_COUNTER_ELEMENTS )
  //-------------------------------------------------------------------------------------




  //####################################################################################
  //-------------------------------------------------------------------------------------
  {% include scan.cl %}
  {% include helpers.cl %}
  //-------------------------------------------------------------------------------------
  //####################################################################################
  

  //-------------------------------------------------------------------------------------
  uint getRadix(uint key, uint numPass)
  {
    //shift relevant radix to LSB, mask higher bits
    return ( key >> (numPass * LOG2_NUM_RADICES_PER_PASS ) ) & (NUM_RADICES_PER_PASS -1) ;
  }
  //-------------------------------------------------------------------------------------




  //=====================================================================================
  //phase one out of three in a radix sort pass: tabulate and local scan
  __kernel __attribute__((reqd_work_group_size(NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void radixSort_tabulate_localScan_Phase(
    __global uint* gKeysToSort,     //NUM_TOTAL_ELEMENTS elements
    __global uint* gRadixCounters,  //NUM_RADICES_PER_PASS * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX elements; (e.g. 64*64k);
                                    //is logically an array NUM_RADICES_PER_PASS radix counter arrays;
                                    //In phase 1 (tabulation and local scan), the results of local scans of the counters 
                                    //  are written to this array;
                                    //In phase 3, it serves as "local" offset for the own radix 
    __global uint* gSumsOfLocalRadixCounts, // NUM_RADICES_PER_PASS * NUM_TABULATE_WORK_GROUPS  elements (e.g. 64 * 512);
                                    //In phase 1, the sums of the local scans of the radix counters will be written to it;
                                    //In phase 2, each of these NUM_RADICES_PER_PASS counter arrays will be scanned
                                    //  to yield the "own radix offsets" to be added to the final reorder-offset
                                    //  SUGGESTION: for read coalescing purposes in phase 3, the scan results of this array 
                                    //              should be written back TRANSPOSED in phase 2 
                                    //              (i.e. logically, it would not be an array of NUM_RADICES_PER_PASS counter arrays 
                                    //              anymore, but an array of NUM_TABULATE_WORK_GROUPS counter set arrays).
                                    //              But as a matrix transpose memory access patterns are only efficient if there
                                    //              are at least as many coloumns as the cache line is big, and as due to big row lenghts
                                    //              there don't fit many columns into the local memory, a transposition seems not a good option;
                                    //              In a far future ;), one could try to use two 2d images intead of a standard buffer for this purpose;
                                    //              But this optimazation is not THAT urgent, because for nearly-sorted key arrays, adjacent elements
                                    //              are likely to have the same radix; hence, the worst case of NUM_RADICES_PER_PASS reads of 
                                    //              128 byte cache lines where we need only NUM_RADICES_PER_PASS*4 byte is not that likely to happen 
                                    //              during simulation
                                    //In phase 3, it serves as "global" offset for the own radix 

    __local uint* lRadixCounters,   //PADDED_STRIDE(NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP) * NUM_RADICES_PER_PASS elements; 
                                    //(e.g. (128+4)*64=8448 elements for fermi. (32+2)*64=2176 elements for GT200 );
                                    //is actually an array of NUM_RADICES_PER_PASS padded radix counter arrays;
    
    uint numPass
  )
  {
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint localPaddedCounterIndex = lwiID / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER;
    localPaddedCounterIndex += CONFLICT_FREE_OFFSET(localPaddedCounterIndex );
   
    //clear to local radix counters to zero
    //will be unrolled to be executed e.g.  8192*(1+1/32)/(512*(1+1/32))=8448/528=16 times without bank conflicts
    #pragma unroll
    for ( uint offset =0; 
          offset < PADDED_STRIDE(NUM_LOCAL_RADIX_COUNTER_ELEMENTS); 
          offset += PADDED_STRIDE(NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP)
    )
    {
      lRadixCounters[offset + localPaddedCounterIndex] = 0;
    }
    //synchronize, because the tabulation of an element is not necessaryly performed by the same work item having done the copy
    //there is no way to circumvent the synch withour causing bank conflicts, i.e. "transposing" the pseudo 2d array
    //increases the danger of bank conflicts
    barrier(CLK_LOCAL_MEM_FENCE);
    
    uint radix = getRadix( gKeysToSort[get_global_id(0)] ,numPass);
    
    atomic_inc( 
      lRadixCounters + 
      //select the counter array for the radix of the current value
      radix * PADDED_STRIDE(NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ) + 
      //select the appropriate counter with padding
      localPaddedCounterIndex 
    );
    
    //------------------------------------------------------------------------------------------
    
    //do the "local" radix scan on every radix, partially in parallel to use all work items:
    #pragma unroll 
    for(uint i= 0; i < ( NUM_RADICES_PER_PASS / NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL ) ; i++)
    {
      //divide work items in several sets with interval [0..half length of one radix counter array], 
      //as each work item can scan two radix counter elements
      //default (fermi): [0..511] % (128/2) --> 8 occurences of interval [0..64]
      //default (GT200): [0..256] % (32/2) --> 16 occurences of interval [0..16] <-- on half warp scans another array than the other.. 
      //                                                                              but the control flow should not diverge worse 
      //                                                                              than when scanning only one array with few elements...                                                      
      uint workItemOffsetID =  BASE_2_MODULO( lwiID,  (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP >> 1) );   
                              //lwiID % ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP / 2 )
      //radix in whose scan the curren work item will participate = 
      //default (fermi): [0..7] *  8 + (2* [0..511])/128 --> [0..7] *  8 + [0 .. 8]
      //default (GT200): [0..4] * 16 + (2* [0..255])/ 32 --> [0..7] *  8 + [0 ..16]
      uint radixToScan = 
        //big loop offset 
        i * NUM_LOCAL_RADIX_COUNTERS_TO_SCAN_IN_PARALLEL 
        //+ work item id / (half size of one radix counter array)
        + ( lwiID << 1 ) / ( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ); 

      
      float totalRadixSum  = 
        scanInclusive(
          //calculate the pointer to the first element in the "array of radix counter arrays"
          lRadixCounters + radixToScan * PADDED_STRIDE( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  ), 
          //as many elements to scan as we have radix counter elements per radix
          NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP, 
          //communicate the stride [0.. half size of one radix counter array] to the scan function so that it can operate
          //indepentently of its actual work item id
          workItemOffsetID
        );
        
              
      //write the total sum of the local counter array to global memory
      if(workItemOffsetID == 0)
      {
        gSumsOfLocalRadixCounts[
          //select the counter array for the recently scanned radix counte array
          radixToScan * NUM_TABULATE_WORK_GROUPS
          //entry in counter array corresponds to work group
          + get_group_id(0)
        ] = totalRadixSum;
      }
      
      //write the scan results to global memory: as one work item scans two counters, it also has to write out two of them
      #pragma unroll
      for(int i=0; i<2; i++)
      {
        gRadixCounters[ 
          //select "radix row" in pseudo 2d array
          radixToScan * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX 
          + get_group_id(0) * NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP
          //write the i'th half of the current array with the  NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP/2 work items
          + i * (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP >> 1) + workItemOffsetID
        ] 
        = lRadixCounters[
          //select "radix row" in pseudo 2d array taking padding into account
          radixToScan * PADDED_STRIDE( NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP  )
          //write the i'th half of the current array with the  NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP/2 work items
          + CONFLICT_FREE_INDEX( i*  (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP >> 1) + workItemOffsetID )
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
   #define NUM_GLOBAL_SCAN_ELEMENTS_TO_SCAN_PER_WORK_GROUP ( NUM_TABULATE_WORK_GROUPS )
  //half as many work items per work group as there are elements to scan
  #define NUM_GLOBAL_SCAN_WORK_ITEMS_PER_WORK_GROUP ( NUM_GLOBAL_SCAN_ELEMENTS_TO_SCAN_PER_WORK_GROUP >> 1 )
  //Because the structure of the algorithm demands that the global scan can be performed by a single work group,
  //theoretically, phase 2 can be completely performed by a single work group; But this approach woudn't scale
  //with the number of multiprocessors in a device; Hence, for perfect load balancing, we take 
  // pow2(ceil(log2(CL_DEVICE_MAX_COMPUTE_UNITS)) work groups calced by app), 
  //i.e. the number of multiprocessors, rounded to the next power of two
  #define NUM_GLOBAL_SCAN_WORK_GROUPS  ( {{ numComputeUnits }} )
  //default value for Geforce GT  435M (fermi) : (64 / pow2(ceil(log2( 2))) = 32
  //default value for Geforce GTX 280  (GT200) : (64 / pow2(ceil(log2(30))) = 2
  //default value for Geforce GTX 570  (fermi) : (64 / pow2(ceil(log2(15))) = 4
  //default value for Geforce GTX 580  (fermi) : (64 / pow2(ceil(log2(16))) = 4
  #define NUM_GLOBAL_SCAN_RADICES_PER_WORK_GROUP (NUM_RADICES_PER_PASS / NUM_GLOBAL_SCAN_WORK_GROUPS )
  
  __kernel __attribute__((reqd_work_group_size(NUM_GLOBAL_SCAN_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void radixSort_globalScan_Phase(
    __global uint* gSumsOfLocalRadixCounts, //NUM_TABULATE_WORK_GROUPS * NUM_RADICES_PER_PASS elements (e.g. 512*64);
                                    //In phase 1, the sums of the local scans of the radix counters will be written to it;
                                    //In phase 2, each of these NUM_RADICES_PER_PASS counter arrays will be scanned
                                    //  to yield the "own radix offsets" to be added to the final reorder-offset
                                    //  SUGGESTION: for read coalescing purposes in phase 3, the scan results of this array 
                                    //              should be written back TRANSPOSED in phase 2 
                                    //              (i.e. logically, it would not be an array of NUM_RADICES_PER_PASS counter arrays 
                                    //              anymore, but an array of NUM_TABULATE_WORK_GROUPS counter set arrays).
                                    //              But as a matrix transpose memory access patterns are only efficient if there
                                    //              are at least as many coloumns as the cache line is big, and as due to big row lenghts
                                    //              there don't fit many columns into the local memory, a transposition seems not a good option;
                                    //              In a far future ;), one could try to use two 2d images intead of a standard buffer for this purpose;
                                    //              But this optimazation is not THAT urgent, because for nearly-sorted key arrays, adjacent elements
                                    //              are likely to have the same radix; hence, the worst case of NUM_RADICES_PER_PASS reads of 
                                    //              128 byte cache lines where we need only NUM_RADICES_PER_PASS*4 byte is not that likely to happen 
                                    //              during simulation
                                    //In phase 3, it serves as "global" offset for the own radix 
    __global uint* gSumsOfGlobalRadixCounts, //NUM_RADICES_PER_PASS elements (e.g. 64); 
                                    //In phase 2, the total sum of each radix counter array will be calculated,
                                    //  then a partial scan (NUM_RADICES_PER_PASS / NUM_GLOBAL_SCAN_WORK_GROUPS), e.g. (64/2)=32
                                    //  will be performed by the NUM_GLOBAL_SCAN_WORK_GROUPS and uploaded to this array
                                    //In phase 3, this array is completely read to shared memory, and "the rest" of the scan is performed,
                                    //  namely NUM_GLOBAL_SCAN_WORK_GROUPS are scanned
    
    __local uint* lSumsOfLocalRadixCounts,   // PADDED_STRIDE( NUM_GLOBAL_SCAN_ELEMENTS_TO_SCAN_PER_WORK_GROUP ) elements,
                                    //so that one padded array to scan fits in;
                                    // example size (fermi) :  512 *(1+1/32) =  528 elements
                                    // example size (fermi) : 1024 *(1+1/16) = 1088 elements
                                    
   __local uint*  lSumsOfGlobalRadixCounts,  //NUM_GLOBAL_SCAN_RADICES_PER_WORK_GROUP elements (e.g. 32 for Geforce GT435M);
                                    //a _sequential_ scan will be performed, as the different radix counter arrays are scanned
                                    //consecutively; one work item has to grab the total sum of each global radix scan,
                                    //add the sums of the previous global radix scans and write the result to the appropriate element
                                    //in the local array; The final result will be a partial scan on the total radix sums on
                                    //which one work group has worked on. (The final scan will be done in phase 3, as global synch is needed)
                                    
    uint numPass
  )
  {
    uint lwiID = get_local_id(0); // short for "local work item ID"
    
    

  }
  //=====================================================================================
  
  
  
  
  //=====================================================================================
  //phase three out of three in a radix sort pass: reorder
  //number of work items corremponds to that of phase one, so this is no mistake:
  __kernel __attribute__((reqd_work_group_size(NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void radixSort_reorder_Phase(
    TODO
    uint numPass
  )
  {
    uint lwiID = get_local_id(0); // short for "local work item ID"
    
    //pseudo code:
    uint radix = getRadix( gKeysToSort[get_global_id(0)] ,numPass)
    uint finalNewPosition = 
      //offset from all previous radices
      lSumsOfAllRadixCounts[radix]    <--still to compute via two different scale partial scans!
      //"global" offset for the own radix 
      +
      gSumsOfLocalRadixCounts[         <-- make local for coalescing? coalscing seems impossible here :@ <-- solution: transpose in phase 2?
                                           assumption: scattered writes do not have the "cache line band width overhead" like scattered reads; I'm missing resources to verify this assumption :(
        //select appropriate "global" radix counter array
        radix * NUM_TABULATE_WORK_GROUPS 
        //select the appropriate counter element within the "global" radix counter array via the own work group ID
        + get_group_id(0)
      ]
      //"local" offset for the own radix within each work group
      + 
      atomic_inc(
          lRadixCounters                                                          <-- make local for coalescing and omittanc of atomic_inc!
          //select appropriate "local" radix counter array
          + radix * NUM_TOTAL_RADIX_COUNTERS_PER_RADIX
           //select the appropriate counter element within the "local" radix counter array
          + lwiID / NUM_KEY_ELEMENTS_PER_RADIX_COUNTER
      )
      ;

  }
  //=====================================================================================
  
  
  
