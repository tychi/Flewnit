  //-------------------------------------------------------------------------------------
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
    {% endifequal nvidiaComputeCapabilityMajor 1 %}
    #define CONFLICT_FREE_OFFSET(n)  ( (n) >> (LOG2_NUMBANKS) )   
    #define CONFLICT_FREE_INDEX(n)  ( (n) + ( (n) >> (LOG2_NUMBANKS) ) )    
  //-------------------------------------------------------------------------------------
