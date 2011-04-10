  //=====================================================================================
  /*
    Kernel to reorder the actual physical attributes on the basis of the gReorderedValueIndices buffer written in the last pass
    of the kernel_radixSort_reorder_Phase;
    TODO optimize: merge the latter and this kernel; 
                   Pros: one less kernel invokation and gReorderedValueIndices write;
                   Cons: modularization broken due to mix up of generic sorting and physics simulation specific code                  
  */
  __kernel void kernel_reorderPhysicalAttributes(
    __global uint* gReorderedOldIndices,

    __global float4* gPositionsOld,
    __global float4* gPositionsReordered,
    
    __global float4* gVelocitiesOld,
    __global float4* gVelocitiesReordered,
    
    __global uint* gParticleIDsOld,
    __global uint* gParticleIDsReordered 
  )
  {
    uint gwiID = get_global_id(0);
    uint oldIndex = gReorderedOldIndices[ get_global_id(0) ];
    
    gPositionsReordered[ gwiID ] =  gPositionsOld[ oldIndex  ];
    gVelocitiesReordered[ gwiID ] =  gVelocitiesOld[ oldIndex  ];
    gParticleIDsReordered[ gwiID ] =  gParticleIDsOld[ oldIndex  ];
    
    //maybe to include here: TODO rigid body meta data calculation (index backtracking for rigid body structures etc..)
  
  }
  //=====================================================================================
