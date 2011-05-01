//pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
  
  
#ifndef FLEWNIT_CL_PROGRAMS_COMMON_GUARD
#define FLEWNIT_CL_PROGRAMS_COMMON_GUARD
  
  
  
  //--------------------- numbers ------------------
  #define EPSILON (0.00001f)
  
  
  #ifndef PI 
    #define PI (3.141592653589793115998f)
  #endif
  //--------------------------------------------------------
  
  
  //--------arithmetic expression macros-----------------------------------------
  #define SQUARED_LENGTH( vec ) ( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z )
  
  //will execute faster than de "generic" modulo operator
  #define BASE_2_MODULO(val,base2Val) ( ( val ) & ( (base2Val) -1 ) )
  //--------------------------------------------------------
  
  
  //---------hardware related macros ----------------------------------------------
   #define WARP_SIZE 32
   #define HALF_WARP_SIZE 16
  
  //pow2(ceil(log2(CL_DEVICE_MAX_COMPUTE_UNITS))) work groups (value calced by app);
  //we need this value to determine the number of work groups for kernels, which can be theoretically performced by a single work group
  //without considerable memory or performance bottle necks; To use all cumpute units of a system anyway, the number of work groups 
  //for those algorithms will be set to this value;
  #define NUM_COMPUTE_UNITS_BASE2_CEILED ( {{ numComputeUnits_Base2Ceiled }} )
  
 //default: 1024; reason: maximum base 2 - work item count per work group; We wanna stick to base 2 as it fits best the hardware, many algorithms
 //and allows some optimized arithmetics (e.g. modulo, integer division) and as it keeps application logic simple
  #define NUM_MAX_WORK_ITEMS_PER_WORK_GROUP ( {{ numMaxWorkItems_Base2Floored }} )
  //--------------------------------------------------------
  
 
 

  
  
  
  
  //------------bank conflict omittance using vec3's; currently unued-------------------------------------------
  /*
  
  //{ see doc/Algorithm/combining global coalescing and alignment with local omission of bank conflicts for vector types.txt
  //  for further information on the following macros   
  //bank conflict free write from registers or global memory to local "implicit float3" buffer
  #define WRITE_TO_LOCAL_FLOAT3_BUFFER(localFloatBuffer, float4Value) \
    localFloatBuffer[3 * get_local_id(0) + 0] = float4Value.x;  \
    localFloatBuffer[3 * get_local_id(0) + 1] = float4Value.y;  \
    localFloatBuffer[3 * get_local_id(0) + 2] = float4Value.z     
  //bank conflict free load from local "implicit float3" buffer into a register float4:
  #define READ_VECTOR_FROM_LOCAL_FLOAT3_BUFFER(localFloatBuffer, localIndex) \
    ( (float4) \
      ( localFloatBuffer[3 * localIndex + 0 ], \
        localFloatBuffer[3 * localIndex + 1 ], \
        localFloatBuffer[3 * localIndex + 2 ], \
        0.0 )                                  )
  #define READ_POSITION_FROM_LOCAL_FLOAT3_BUFFER(localFloatBuffer, localIndex) \
    ( (float4) \
      ( localFloatBuffer[3 * localIndex + 0 ], \
        localFloatBuffer[3 * localIndex + 1 ], \
        localFloatBuffer[3 * localIndex + 2 ], \
        1.0 )                                  )
  //}
  
  */
  //--------------------------------------------------------
 
  
#endif //FLEWNIT_CL_PROGRAMS_COMMON_GUARD
