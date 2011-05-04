
{% block documentHeader %}
  /**
    Generic template for the work efficient an further optimized parallel prefix sum on an array of values generated
    by tabluation of an input array;
    Will be specialized by compactAndSplitUniformGrid.cl to perform a stream compaction on the uniform grid
    to throw out empty cells and split overpopulated cells;
    
    There are many structural and algorithmical parallels to radix Sort.
    Scan is kind of a radix sort with only one "radix" to count, namely the tabulated value.
    
    As "pure scan" is hence less memory bound, there is another distribution of input elements, work items, work groups etc;
    
    I realized that if assuming that performance is best when using as many work groups as there are compute units,
    the "global scan phase (phase 2)" in the current radix sort imlementation could be transformed to serialized
    partial scans within phase 1; There is no guarantee, though, that the assumption is correct for any device and/or driver;
    Furthermore, the "memory-boundness" of the radix sort implementation involves some considerable control flow overhead;
    Hence, I will not try this optimization in the radix sort implementation soon;
    
    
    But as the "simple scan" is much easier to maintain and as hence there are fewer interdependendies between ther several
    optimaziations, I will give the above approach to optimize out the global scan phase at least a try in the "simple scan". 
    
    In the far future, when the overall simulation works, one could try to reduce boiler plate code by merging
    the stream compaction and radix sort kernels to a unified template;
    But at the moment (april 10th, 2011) i rather "rewrite" some code to concentrate on the much easier implementaion of the stream compaction
    (I implemented the kernel after radix sort kernels ).
    EDIT:   
    I more and more realize that the memory-boundness of the radix sort kernels makes easy merging impossible;
    The radix sort stuff is too optimized for itself be be literally pressed into a generic template. Hence, i can concentrate
    on "pure" implementation of the simple scan here.

  */
  
{% endblock documentHeader %}


#pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
//pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
//pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable


  {% include "scan.cl" %}

  //Possible default  values: 
  //  for particles (sorted by radix sort) target in this thesis: 2^18 = 256k;
  //  for uniform grid cells (compacted by stream compaction) target in this thesis: 64^3 = 2^18 = 256k; 
  // -->  both numParticles and numUniGridCells are usually equal to 256k; this is a coincidence and does NOT mean that
  //      one shall try to reuse or merge some kernels just because they work on the same number of elements; This is a special case that will
  //      NOT be abused for any efforts of optimization
  #define NUM_TOTAL_ELEMENTS_TO_SCAN ( {{ numTotalElementsToScan }} )

 
  //default: 1024
  #define NUM_WORK_ITEMS_PER_WORK_GROUP ( NUM_MAX_WORK_ITEMS_PER_WORK_GROUP )
  //default: 2048; one work item can scan up to two elements
  #define NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN ( 2 * NUM_WORK_ITEMS_PER_WORK_GROUP )

  //default 2^18/2^11=2^7 = 128
  #define NUM_ELEMENTS__GLOBAL_SCAN (  NUM_TOTAL_ELEMENTS_TO_SCAN / NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN  )
  
  //Target: keep "scan hierarchies" as flat as possible,
  //so that in the best case, we can safe a "global scan phase", because there are only a few elements left to be scanned
  //globally (optimal case: NUM_BASE2_CEILED_COMPUTE_UNITS elements, i.e 1 to 32 on single GPU, non SLI nvidia devices 
  //(stand: april 2011)).
  //This is why in this implementation, it is tried to to several parallel "local scans" sequentially and accumulate the respective total
  //sums to yield a "partial global scan";
  //This way, we save the "global scan phase" and reduce the kernel invoactions per stream compaction from 3 to 2 :).
  //default values:
  // Geforce GT  435 M: 128/ 2=64
  // Geforce GTX 280  : 128/32= 4
  // Geforce GTX 570  : 128/16= 8 
  #define NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY (  NUM_ELEMENTS__GLOBAL_SCAN / NUM_COMPUTE_UNITS_BASE2_CEILED )
  
  
  {% block specialDefinitions %} 

  {% endblock specialDefinitions %} 
  
  
  //=====================================================================================
  __kernel __attribute__((reqd_work_group_size(NUM_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void kernel_scan_localPar_globalSeq(
    
    {% block tabulationArgs %} 
      __global uint* gValuesToTabulate, //NUM_TOTAL_ELEMENTS_TO_SCAN  elements
    {% endblock tabulationArgs %} 
      
    __global uint* gLocallyScannedTabulatedValues, //NUM_TOTAL_ELEMENTS_TO_SCAN  elements 
    __global uint* gPartiallyGloballyScannedTabulatedValues, //NUM_ELEMENTS__GLOBAL_SCAN elements
    __global uint* gSumsOfPartialGlobalScans  //at least NUM_BASE2_CEILED_COMPUTE_UNITS + 1  elements;
                                               //+1 because the kernel finishing the "total scan" may wanna write out
                                               //the total sum
  )
  {
    __local uint lLocallyScannedTabulatedValues [ PADDED_STRIDE(  NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN ) ];
    //one more item for the total sum; keeps sequential scan on global elements simpler;
    __local uint lPartiallyGloballyScannedTabulatedValues [ NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY + 1 ];
    
    __local uint lTotalSumOfLocalScan;
    
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0);
    
    uint paddedLocalLowerIndex = CONFLICT_FREE_INDEX( get_local_id(0) );
                                                                        //actually: +(NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN/2)
    uint paddedLocalHigherIndex = CONFLICT_FREE_INDEX( get_local_id(0) + NUM_WORK_ITEMS_PER_WORK_GROUP );
    
    if(lwiID == 0)
    {
      //init start to zero (we do EXclusive scans (in contrast to INclusive) throughout the whole simulation)
      lPartiallyGloballyScannedTabulatedValues[0] = 0;
    }
    //no barrier() necessary here, because this values is used not until efter the first exclusive scan,
    //and before this point, there will be issued several barriers() anyway;
    
    uint globalLowerIndex = 
      groupID * NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY * NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN
      + lwiID ;
    uint globalHigherIndex = 
      groupID * NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY * NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN
                //actually: +(NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN/2)
      + lwiID + NUM_WORK_ITEMS_PER_WORK_GROUP ;
      
      
      
    for(int localScanIntervalRunner = 0 ;  localScanIntervalRunner < NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY;  localScanIntervalRunner++)
    {      
    
      {% block tabulation %} 
        //tabulate global values and store them to local mem for scan
        lLocallyScannedTabulatedValues[ paddedLocalLowerIndex  ] = gValuesToTabulate[ globalLowerIndex  ];
        lLocallyScannedTabulatedValues[ paddedLocalHigherIndex ] = gValuesToTabulate[ globalHigherIndex ];
      {% endblock tabulation %}
      
      barrier(CLK_LOCAL_MEM_FENCE); //ensure current to-be-scanned-locally stride is available for every work item;
    

      //do the local parallel scan, grab the respective total sum
      scanExclusive(
        lLocallyScannedTabulatedValues,
        & lTotalSumOfLocalScan,
        NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN,
        lwiID 
      );
      
      //write local scan results to global memory
      gLocallyScannedTabulatedValues[ globalLowerIndex  ] = lLocallyScannedTabulatedValues[ paddedLocalLowerIndex  ];
      gLocallyScannedTabulatedValues[ globalHigherIndex ] = lLocallyScannedTabulatedValues[ paddedLocalHigherIndex ];
          
      if(lwiID == 0)
      {
        //sequential "global scan" on sums of parallel local scans: 
        lPartiallyGloballyScannedTabulatedValues[ localScanIntervalRunner + 1] = 
          lPartiallyGloballyScannedTabulatedValues[ localScanIntervalRunner ] + lTotalSumOfLocalScan;
      }  
      
      //add one  "local stride" size so tha in the next iteration,
      //the next stride is locally scanned (and contributes to the partial global scan ;) )
      globalLowerIndex  +=  NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN;
      globalHigherIndex +=  NUM_ELEMENTS_PER_WORK_GROUP__LOCAL_SCAN;
            
    }//end sequential partial scan of parallel local scans
    
    
    
    if(lwiID < NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY )
    {
      //write the partial global scan results to global memory
      gPartiallyGloballyScannedTabulatedValues[ groupID * NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY + lwiID ] = 
        lPartiallyGloballyScannedTabulatedValues[ lwiID ];
    }
    
    if(lwiID == 0 )
    {
      //write sum of partial global scan results to global memory, so that the following kernel can finish the "total scan"
      gSumsOfPartialGlobalScans[ groupID ] = lPartiallyGloballyScannedTabulatedValues[ NUM_LOCAL_INTERVALS_TO_TREAT_SEQUENTIALLY ];
    }
     
  }
  //=====================================================================================
  
  {% block furtherImplementation %}
  
  {% endblock furtherImplementation %}
