


{%block globalDefinitions %}
  //target in this thesis: 2^18 = 256 k, both for numParticles and for numUniGridCells
  #define NUM_TOTAL_ELEMENTS ( {{ numTotalElements }} )

  //mainly dependent from shared memory consumption of a single work item (calculated by by host);
  //trade off between reduction of work group size in order to manage more work groups simultaneous
  //is subject to experimentation for non-memory bound kernels; For memory bound kernels, not more than oe work group
  //at a time fits into the local memory.
  //Minimum (in order to hide memory latency): 256; Target: multiple of 256  
  #define NUM_WORK_ITEMS_PER_WORK_GROUP ( {{ numWorkItemsPerWorkGroup }} )

  //"serialization amount" in order to
  //	- reduce memory needs or
  //	- do more work per item for very small kernels
  //	  to compensate kernel invocation/management overhead
  #define NUM_ELEMENTS_PER_WORK_ITEM ( { {numElementsPerWorkItem }} )

  #define NUM_TOTAL_WORK_ITEMS ( NUM_TOTAL_ELEMENTS  / NUM_ELEMENTS_PER_WORK_ITEM )


  #define NUM_WORK_GROUPS ( NUM_TOTAL_WORK_ITEMS / NUM_WORK_ITEMS_PER_WORK_GROUP )
  
  
  //bank conflict avoidance; see http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html to get the idea,
  //though the formula seems wrong despite the correct explanation
  {% ifequal nvidiaComputeCapabilityMajor 2 %}
    #define NUM_BANKS 32
    #define LOG2_NUMBANKS 5
  {% endifequal nvidiaComputeCapabilityMajor 2 %}
  {% ifequal nvidiaComputeCapabilityMajor 1 %}
    //bank conflict avoidance
    #define NUM_BANKS 16
    #define LOG2_NUMBANKS 4
  {% endifequal nvidiaComputeCapabilityMajor 2 %}
  #define CONFLICT_FREE_OFFSET(n)  ( (n) >> (LOG2_NUMBANKS) )    

{%endblock globalDefinitions %}


{% block specificDefinitions %}

  //{ to be outsourced to derived radixSort.cl, but for the overview at the moment...
    //default: 3 * log2(uniGridNumCellsPerDimension) = 18
    #define NUM_BITS_PER_KEY ( {{numBitsPerKey}} )
    //goal: sort 6 radices at once so that we need only three passes for 18 bits
    #define NUM_RADICES_PER_PASS ( {{numRadicesPerPass}} )
    #define NUM_RADIX_SORT_PASSES ( NUM_BITS_PER_KEY / NUM_RADICES_PER_PASS )
  //}
  
  //{ to be outsourced to derived streamCompaction.cl, but for the overview at the moment...
  
    //generic identifier; name was chosen in order to not pollute the generic algorithms with concrete;
    //simulation semantics; 
    //But Hint: In the uniform grid-"particles per cell"-context, this means "max particles per launch group"
    //this shall be 32; less would let many warp trheads idle, more would mean a significant increase in comput cost,
    //because due to the uniform grid, the complexity is O(n*m), where m is the average number of particles in a grid cell;
    //it shall be tried by SPH kernel and simulation params (particle radius, paricle mass, gravity...) that
    //there are seldom more than 32 particles in a cell and that there aren't siginficantly less than 32 particles
    //in an "inner" cell, i.e. no splash/surface cell;
    #define NUM_MAX_ELEMENTS_PER_ELEMENT_GROUP ( {{ numMaxElementsPerLaunchGroup}} )
  //}
  
  
{% endblock specificDefinitions %}



  //template for
  //  - first phase (out of three) in a radix sort pass
  //  - first phase (out of two: localScan, globalScan_Combine) in a stream compaction pass
  __kernel void {{tabulate_localScan_KernelFunctionName}}(
    {% block tabulationArgs %}
      
    {% endblock tabulationArgs %}
    ,
    {% block localScanArgs %}
    
    {% endblock localScanArgs %}
  )
  {
    {% block tabulate %}
    
    {% endblock tabulate %}
    //----------------------------------------------------
    {% block localScan %}
    
    {% endblock localScan %}
    //----------------------------------------------------
    {% block writeBackResults %}
    
    {% endblock writeBackResults %}
  }
  
    //template for
  //  - second phase (out of three) in a radix sort pass
  //  - second phase (out of two: localScan, globalScan_Combine) in a stream compaction pass
  __kernel void {{tabulate_localScan_KernelFunctionName}}(
    {% block tabulationArgs %}
      
    {% endblock tabulationArgs %}
    ,
    {% block localScanArgs %}
    
    {% endblock localScanArgs %}
  )
  {
    {% block tabulate %}
    
    {% endblock tabulate %}
    //----------------------------------------------------
    {% block localScan %}
    
    {% endblock localScan %}
    //----------------------------------------------------
    {% block writeBackResults %}
    
    {% endblock writeBackResults %}
  }


