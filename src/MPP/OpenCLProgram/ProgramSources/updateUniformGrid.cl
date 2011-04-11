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
    

    __global uint* gUniGridCells_ParticleStartIndex, //NUM_TOTAL_GRID_CELLS elements
    
    //__global int* gUniGridCells_NumParticles //NUM_TOTAL_GRID_CELLS elements
                                               //is initialized to zero for every entry, so that unwritten values are valid for
                                               //stream compaction scan in following kernel  
   __global uint* gUniGridCells_ParticleEndIndexPlus1 //NUM_TOTAL_GRID_CELLS elements
                                               //is actually the gUniGridCells_NumParticles buffer, but for performance reasons
                                               //(don't wanna make thousands of global atom_adds and atom_subs),
                                               //it will hold the particle end index+1 after the kernel has completed;
                                               //the transformation to the number of residing particles 
                                               //(numParticles = (endIndexPlus1 > 0) ? (endIndexPlus1 - startIndex) :0) will be performed during 
                                               //tabulation for the stream compaction
                                               
  )
  {
    //note that we only buffer the z-indices at local memory, because we read them twice;
    //In the worst, uncached, unoptimized case, not doing so would mean to double global memory bandwidth;
    //On fermi devices, the 16kB L1 cache and the fact that the two reads happen im close temporal succession 
    //might raise the possibility that performanc might be even better without the local buffering, because this way,
    //there are fewer special cases and hence control flow divergences, and there is no synchronization needed;
    //TODO check this out ;(
    __local uint lSortedZIndices[NUM_MAX_WORK_ITEMS_PER_WORK_GROUP];
    
    
    uint lwiID = get_local_id(0); // short for "local work item ID"
    uint gwiID = get_global_id(0); // short for "global work item ID"
    uint groupID =  get_group_id(0);
    
    //define some neighbouthood macros to not get confused by the indexing
    #define GLOBAL_LEFT_INDEX  ( get_global_id(0) -1 )
    #define LOCAL_LEFT_INDEX   ( get_local_id(0)  -1 )
    #define GLOBAL_OWN_INDEX   ( get_global_id(0)    )
    #define LOCAL_OWN_INDEX    ( get_local_id(0)     ) 
    //#define GLOBAL_RIGHT_INDEX ( get_global_id(0) +1 )
    //#define LOCAL_RIGHT_INDEX  ( get_local_id(0)  +1 )
    
    lSortedZIndices[ LOCAL_OWN_INDEX ] = gSortedZIndices[ GLOBAL_OWN_INDEX ];
    barrier(CLK_LOCAL_MEM_FENCE);
   
    //{ check the special cases where no neighbours exist at all
    //leftmost global element?
    if(gwiID == 0)
    {
      //index of own particle is definitely the start index of the cell it lies in:
      //obsolete because is zero anyway:
      //gUniGridCells_ParticleStartIndex[ lSortedZIndices[ 0 ] ]= 0; 
      return;     
    }
    //rightmost global element?
    if(gwiID == (get_global_size() -1) )
    {
      //(index of own particle +1) is definitely the (end index +1) of the cell it lies in:
      gUniGridCells_ParticleEndIndexPlus1[ lSortedZIndices[ LOCAL_OWN_INDEX ] ] = get_global_size();
      return;      
    }
    //} end check the special cases where no neighbours exist at all  
     
    //left and right neighbours do exist in the following code, otherwise, the particle would have been masked by the above code
    
    //grab left index from local memory unless it is the leftmost element in work group, then grab from global memory
    uint leftZIndex = ( (lwiID > 0) ?  (lSortedZIndices[ LOCAL_LEFT_INDEX ]) : (gSortedZIndices[ GLOBAL_LEFT_INDEX ]) );
    uint ownZIndex = lSortedZIndices[ LOCAL_OWN_INDEX ] ;

      
    //is left index different?
    if(ownZIndex != leftZIndex )
    {
      //left index is different, i.e. here starts a new index; 
      //set the relevant start index of own grid cell :
      gUniGridCells_ParticleStartIndex[ ownZIndex ]= GLOBAL_OWN_INDEX ;
      //set the relevant (end index+1) of left neighbour grid cell :
      gUniGridCells_ParticleEndIndexPlus1[ leftZIndex ]= GLOBAL_OWN_INDEX; 
    }  
    //we dont have to check inequalitity of the own z-index to its right neigbour, because when the global border cases are caught,
    //there are as many middle-right differences as there are middle-left differences ;). Both start and end of the relevant grid cells
    //are marked.
    
    /*
      Bullsh** from Goswami paper? Seems quite unnecessary to me, and this approach wasting operations and bandwidth and
      enforces many serializations due to the atomics:
      "Whereas the first particle in a block can
       be determined using the atomicMin operation in CUDA, the
       number of particles is found by incrementing a particle count
       with atomicInc. Thus each particle updates both the starting
       index and particle count of its block in the list B [...]"
       
     My approach should be much faster, though in the subsequent "cell split and compatction" kernel,
     we have some overhead to compensate for this optimization 
    */
 
  }
  //=====================================================================================
