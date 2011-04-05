#pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable


/*

  (Hopefully) highly optimized radix sort:
  Basic algorithm ideas are taken from http://http.developer.nvidia.com/GPUGems3/gpugems3_ch32.html
  and http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html;
  
  Though, there seem to be some mistakes an unnecessary bottle necks (doumentation follows);
  Hence, I had to adopt some elements of the algorithms;

  I don't try to enforce template usage at the moment; It rather confuses than helps me with structuring at the moment; 

  The goal of this thesis is to simulate 256k particles as efficently as possible;
  Due to severe time pressure, the early implementation might contain some "hardcodes" to achieve this
  goal asap; But I try to stay as generic as possible...
*/

 
  //target in this thesis: 2^18 = 256 k, both for numParticles and for numUniGridCells
  #define NUM_TOTAL_ELEMENTS ( {{ numTotalElements }} )
  
  //work items for scan, i.e. phase 2! Does NOT apply to tabulate phase one!
  //default: 1024; reason: maximum work group size on fermi architectures   
  #define NUM_SCAN_WORK_ITEMS_PER_WORK_GROUP ({{numScanWorkItemsPerWorkGroup}})
  
  //--------------------------------------------------------------------------------------
    //default: 3 * log2(uniGridNumCellsPerDimension) = 18
    #define NUM_BITS_PER_KEY_TO_SORT ( {{numBitsPerKey}} )
    //goal: sort 6 bits per pass, i.e. we need 2^6=64 radices at once per pass so that we need only three passes for 18 bits
    #define NUM_RADICES_PER_PASS ( {{numRadicesPerPass}} )
    //default: log2(64)=6
    #define LOG2_NUM_RADICES_PER_PASS ( {{log2NumRadicesPerPass}} )
    #define NUM_RADIX_SORT_PASSES ( NUM_BITS_PER_KEY_TO_SORT / LOG2_NUM_RADICES_PER_PASS )
  //-------------------------------------------------------------------------------------

  //-------------------------------------------------------------------------------------
    //"serialization amount" in order to
    //	- reduce memory needs and/or
    //  - reduce the length of the radix counter arrays to be scanned
    //	- do more work per item for small kernels to compensate kernel invocation/management overhead (doesn't apply to radix sort)
    // default: 4; this way, we need...
    #define NUM_ELEMENTS_PER_RADIX_COUNTER ( {{numElementsPerRadixCounter }} )
    //... "only" 256k / 4 = 64k radix counters per radix ...
    #define NUM_TOTAL_RADIX_COUNTERS_PER_RADIX ( NUM_TOTAL_ELEMENTS / NUM_ELEMENTS_PER_RADIX_COUNTER )
    //...for which we need "only" 64 scan work groups @ 1024 items/WG...
    #define NUM_SCAN_WORK_GROUPS  ( NUM_TOTAL_RADIX_COUNTERS_PER_RADIX / NUM_SCAN_WORK_ITEMS_PER_WORK_GROUP )
    //...which can be performed by a single warp, so that in phase 3, for the "global scan", all but the first 32 items can be skipped
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
    #define NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP ( LOCAL_MEM_FOR_RADIX_COUNTERS / ( 4 * NUM_RADICES_PER_PASS ) )
    //number of elements in local radix counter array (needed for initial clearing to zero)
    //default : 128*64 = 8192
    #define NUM_LOCAL_RADIX_COUNTER_ELEMENTS (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_RADICES_PER_PASS)
  //-------------------------------------------------------------------------------------
  
  //-------------------------------------------------------------------------------------
    //one work item per key element, to that each work item can copy a key to __local memory
    //default for fermi devices: 128 radix counters per radix * 4 elements per radix counter = 512
    #define NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_ELEMENTS_PER_RADIX_COUNTER)
    //define explicitely the numbers of elements per tabulation work group: default: 512
    #define NUM_TABULATE_ELEMENTS_PER_WORK_GROUP (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_ELEMENTS_PER_RADIX_COUNTER)
    //default : 2^18/( 2^7 * 2^2) = 2^9 = 512
    #define NUM_TABULATE_WORK_GROUPS ( NUM_TOTAL_ELEMENTS  / (NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_ELEMENTS_PER_RADIX_COUNTER) )
  //-------------------------------------------------------------------------------------
  


 


  uint getRadix(uint key, uint numPass)
  {
    //shift relevant radix to LSB, mask higher bits
    return ( key >> (numPass * LOG2_NUM_RADICES_PER_PASS ) ) & (NUM_RADICES_PER_PASS -1) ;
  }

  //phase one out of three in a radix sort pass
  __kernel __attribute__((reqd_work_group_size(NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void tabulateForRadixSort(
    __global uint* gKeysToSort,     //NUM_TOTAL_ELEMENTS elements
    __global uint* gRadixCounters,  //NUM_TOTAL_RADIX_COUNTERS_PER_RADIX * NUM_RADICES_PER_PASS elements; (e.g. 64k*64);
                                    //is actually an array NUM_RADICES_PER_PASS radix counter arrays;
    __local uint* lRadixCounters,   //NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP * NUM_RADICES_PER_PASS elements; (e.g. 128*64);
                                    //is actually an array NUM_RADICES_PER_PASS radix counter arrays;
    __local uint* lKeysToTabulate,  //NUM_TABULATE_ELEMENTS_PER_WORK_GROUP + CONFLICT_FREE_OFFSET(NUM_TABULATE_ELEMENTS_PER_WORK_GROUP) 
                                    //elements, e.g. 512 + 512/32 = 512 +16 = 528;
                                    //offset is necessary here so that after copying all elements to local mem by 
                                    //NUM_TABULATE_ELEMENTS_PER_WORK_GROUP (e.g.512) work items,
                                    //the first NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP (e.g.128) work items can read
                                    //the values from local memory without bank conflicts;
                                    //Without padding, the NUM_ELEMENTS_PER_RADIX_COUNTER (e.g. 4) stride would cause
                                    //a NUM_ELEMENTS_PER_RADIX_COUNTER-way bank conflict.
    uint numPass                    //determines the bit range relevant for radix counter tabulation
  )
  {
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint localKeyIndex = CONFLICT_FREE_INDEX(lwiID);
    lKeysToTabulate[localKeyIndex] = gKeysToSort[get_global_id(0)];
    
    //clear to local radix counters to zero
    //will be unrolled to be executed e.g.  8192/512=16 times without bank conflicts
    #pragma unroll
    for (uint offset =0; offset < NUM_LOCAL_RADIX_COUNTER_ELEMENTS; offset+= NUM_TABULATE_WORK_ITEMS_PER_WORK_GROUP )
    {
      lRadixCounters[offset + lwiID] = 0;
    }
    
    //synchronize, because the tabulation of an element is not necessaryly performed by the same work item having done the copy
    barrier(CLK_LOCAL_MEM_FENCE);
    
    
    
    /*
      //only one element can increment one counter of radix counter set, otherwise race conditions can arise;
      if(lwiID < NUM_RADIX_COUNTERS_PER_RADIX_AND_WORK_GROUP )
      #pragma unroll
      for(int currentElementIndex = 0; currentElementIndex < NUM_ELEMENTS_PER_RADIX_COUNTER; currentElementIndex++ )
      {
      
      }
    }
    */
  
  }
  
  
  
