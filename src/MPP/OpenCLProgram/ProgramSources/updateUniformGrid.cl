#pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
//pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
//pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable


 //=====================================================================================
  /*
      Kernel to determine how many particles are in every uniform grid cell and where the first particle of a cell resides in memory              
  */
  
  //default: 64;
  #define NUM_GRID_CELLS_PER_DIMENSION ( {{ numGridCellsPerDimension }} )
  //default: 64^3 = 2^18 = 256k
  #define NUM_TOTAL_GRID_CELLS ( NUM_GRID_CELLS_PER_DIMENSION * NUM_GRID_CELLS_PER_DIMENSION * NUM_GRID_CELLS_PER_DIMENSION  )

  

  
  
  __kernel __attribute__((reqd_work_group_size(NUM_MAX_WORK_ITEMS_PER_WORK_GROUP,1,1))) 
  void kernel_updateUniformGrid(
    __global uint* gSortedZIndices, //numTotalParticles elements
    
    //Note that in this kernel, these (usually uint-) buffers are bound as SIGNED integer buffer, because
    //depending on the order of the global atomic inc and global atomic sub, the values can become temporaryly negative;
    //I hope that OpenCL and the CLGL interop makes no prolems with this kind of reinterpret_cast ;(
    __global int* gUniGridCells_ParticleStartIndex, //NUM_TOTAL_GRID_CELLS elements
    
    //__global int* gUniGridCells_NumParticles //NUM_TOTAL_GRID_CELLS elements
                                               //is initialized to zero for every entry, so that unwritten values are valid for
                                               //stream compaction scan in following kernel
   
   __global int* gUniGridCells_ParticleEndIndexPlus1 //NUM_TOTAL_GRID_CELLS elements
                                               //is actually the gUniGridCells_NumParticles buffer, but for performance reasons
                                               //(don't wanna make thousands of global atom_adds and atom_subs),
                                               //it will hold the particle end index+1 after the kernel has completed;
                                               //the transformation to the number of residing particles 
                                               //(numParticles = (endIndexPlus1 > 0) ? (endIndexPlus1 - startIndex) :0) will be performed during 
                                               //tabulation for the stream compaction
                                               
  )
  {
     __local uint lSortedZIndices[NUM_MAX_WORK_ITEMS_PER_WORK_GROUP];
    
    
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0);
    
    lSortedZIndices[ get_local_id(0) ] = gSortedZIndices[ get_global_id(0) ];
    barrier(CLK_LOCAL_MEM_FENCE);
    
    
     //TODO implement TOO TIRED NOW: (following code is bullshaat)
    
    
    //leftmost local element? 
    if(lwiID == 0)
    {
      //leftmost global element?
      if(gwiID == 0)
      {
  
        gUniGridCells_ParticleStartIndex[ lSortedZIndices[ lwiID ] ]= gwiID;
        atom_add(gUniGridCells_NumParticles + lSortedZIndices[ lwiID ] , (int)(gwid));      
      }
      else
      {
        //is left index different?
        if(lSortedZIndices[ lwiID ] != lSortedZIndices[ lwiID -1 ])
        {
          //left index is different, i.e. here starts a new index; set the relevant start index:
          uniGridParticleStartIndices[particleZIndex]=gwiID;
        }
      }
    }
    
    
    
    //not the leftmost global element? 
    if(gwiID != 0)
    {
      //not the leftmost local element?
      if(lwiID != 0)
      {
        //is left index different?
        if(lSortedZIndices[ lwiID ] != lSortedZIndices[ lwiID -1 ])
        {
          //left index is different, i.e. here starts a new index; set the relevant start index:
          uniGridParticleStartIndices[particleZIndex]=gwiID;
        }
      }
    }
          //buffers are flushed to zero, so following is obsolete:
          //else{
          //  uniGridParticleStartIndices[particleZIndex]=0;
          //}
          
          if(gwiID != (numParticles -1))
          {
            if(sortedZIndices[gwiID] != sortedZIndices[gwiID +1])
            {
              //right index is different, i.e. here ends the particle list for the voxel containing this particle;
              //set the relevant end index to gwiID +1, so that the particle count can be derived by (startIndex-EndIndex)
              //during voxel split and compression;
              uniGridParticleEndIndices[particleZIndex]=gwiID +1;
            }
          }
          else{
            uniGridParticleEndIndices[particleZIndex]=gwiID +1;
          }
          
          //bullsh** from Goswami paper? Seems quite unnecessary to me and wasting operations and bandwidth:
          "Whereas the first particle in a block can
          be determined using the atomicMin operation in CUDA, the
          number of particles is found by incrementing a particle count
          with atomicInc. Thus each particle updates both the starting
          index and particle count of its block in the list B [...]"
          //atomicMin( uniformGrid[getUniformGridArrayIndex(currentParticle.zIndex)].particleStartIndex, currentParticleArrayIndex);
          //atomicInc( uniformGrid[getUniformGridArrayIndex(currentParticle.zIndex)].numContainingParticles);


  
  }
  //=====================================================================================
