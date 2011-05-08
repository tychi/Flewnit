  
#ifndef FLEWNIT_CL_PROGRAMS_PHYSICS_COMMON_FUNCTIONS_GUARD
#define FLEWNIT_CL_PROGRAMS_PHYSICS_COMMON_FUNCTIONS_GUARD


  {% include "particleSimulationCommon.cl" %}
  
  
  
  //{ following two functions are inspired by the Nvidia oclParticles demo
  int4 getGridPos(float4 p,  __constant SimulationParameters* cSimParams){
      int4 gridPos;
      gridPos.x = (int) floor( ( p.x - cSimParams->uniGridWorldPosLowerCorner.x ) / cSimParams->uniGridCellSizes.x );
      gridPos.y = (int) floor( ( p.y - cSimParams->uniGridWorldPosLowerCorner.y ) / cSimParams->uniGridCellSizes.y );
      gridPos.z = (int) floor( ( p.z - cSimParams->uniGridWorldPosLowerCorner.z ) / cSimParams->uniGridCellSizes.z );
      gridPos.w = 0;
      return gridPos;
  }


   uint getZIndexi(
    int4 gridPos,
    __constant SimulationParameters* cSimParams,
    __constant uint* cGridPosToZIndexLookupTable 
   )
   {
    //Wrap addressing, assume power-of-two grid dimensions
    gridPos.x = gridPos.x & ( NUM_UNIGRID_CELLS_PER_DIMENSION - 1);
    gridPos.y = gridPos.y & ( NUM_UNIGRID_CELLS_PER_DIMENSION - 1);
    gridPos.z = gridPos.z & ( NUM_UNIGRID_CELLS_PER_DIMENSION - 1);

     return (
      cGridPosToZIndexLookupTable[ 0* NUM_UNIGRID_CELLS_PER_DIMENSION + gridPos.x ]
      |
      cGridPosToZIndexLookupTable[ 1* NUM_UNIGRID_CELLS_PER_DIMENSION + gridPos.y ]
      |
      cGridPosToZIndexLookupTable[ 2* NUM_UNIGRID_CELLS_PER_DIMENSION + gridPos.z ]
    );
    
  }
  
//} 
  
  
  
  uint getZIndexf(
    float4 position,
    __constant SimulationParameters* cSimParams,
    __constant uint* cGridPosToZIndexLookupTable 
    )
  {
    return  
      getZIndexi(
        getGridPos( position, cSimParams),
        cSimParams,
        cGridPosToZIndexLookupTable 
      );   
  } 
  
  
  

/*
  uint getZIndex(
    float4 position,
    __constant SimulationParameters* cSimParams,
    __constant uint* cGridPosToZIndexLookupTable 
    )
  {
  
    // this horrible peace of shit function!!11 wasted hours to find this bug!
    //position = remainder(
    //  position - cSimParams->uniGridWorldPosLowerCorner,
    //  NUM_UNIGRID_CELLS_PER_DIMENSION * cSimParams->uniGridCellSizes
    //);
    //
     
    float4 uniGridFloatIndex =  
      fmod( 
        ( position - cSimParams->uniGridWorldPosLowerCorner), NUM_UNIGRID_CELLS_PER_DIMENSION * cSimParams->uniGridCellSizes
      )
      *cSimParams->inverseUniGridCellSizes
    ;    
    
    //the mod for negative vals doesnt work like on the CPU :(
    //int4 uniGridIntIndex = convert_int4( uniGridFloatIndex ) % NUM_UNIGRID_CELLS_PER_DIMENSION  ;
    
    
    
    //shift to positive domain if negative:
    //the screwed spce defines to return PLUS 1 for negative scalar floats, 
    //but to return MINUS 1 in each intn component for negative components of vector floats; wtf!
    uniGridFloatIndex -= convert_float4(signbit(uniGridFloatIndex)) * ( (float)(NUM_UNIGRID_CELLS_PER_DIMENSION) );
    
    uint4 uniGridIntIndex = convert_uint4_rtz( uniGridFloatIndex );
 
     return (
      cGridPosToZIndexLookupTable[ 0* NUM_UNIGRID_CELLS_PER_DIMENSION + uniGridIntIndex.x ]
      |
      cGridPosToZIndexLookupTable[ 1* NUM_UNIGRID_CELLS_PER_DIMENSION + uniGridIntIndex.y ]
      |
      cGridPosToZIndexLookupTable[ 2* NUM_UNIGRID_CELLS_PER_DIMENSION + uniGridIntIndex.z ]
    );
    
   
  }
*/
  
  //-------------------------------------------------------------------------
  
  float poly6(float4 distanceVector, __constant SimulationParameters* cSimParams)
  {
    float sqaredDistCenterToKernelBorder = cSimParams->SPHsupportRadiusSquared - SQUARED_LENGTH(distanceVector);
    
    //a negative value would mean that the distanceVector is longer than the support radius and hence the particles having this
    //distance don't interact with each other
    if(sqaredDistCenterToKernelBorder >= 0.0f )
    {
        //return 1.0f;
    
        return 
          // 315 / (64 pi * h^9) * (h^2-r^2)^3
          cSimParams->poly6KernelConstantTerm
          * sqaredDistCenterToKernelBorder * sqaredDistCenterToKernelBorder * sqaredDistCenterToKernelBorder;
        //* pown( sqaredDistCenterToKernelBorder, 3    );
        // powr( sqaredDistCenterToKernelBorder, 3.0f );
        //TODO use native functions when stable
        //native_powr(sqaredDistCenterToKernelBorder, 3.0f );
    }
    else
    {
      return 0.0f;
    }
  }
  
  
  float4 gradSpiky(float4 distanceVector, __constant SimulationParameters* cSimParams)
  {
    float lenDistVec = length(distanceVector);
    //TODO use native functions when stable
    //float lenDistVec = sqrt( SQUARED_LENGTH(distanceVector) ) ;
    //float lenDistVec = native_sqrt( SQUARED_LENGTH(distanceVector) ) ;
    //float lenDistVec = fast_length( distanceVector )  ;
    
    float distCenterToKernelBorder = cSimParams->SPHsupportRadius - lenDistVec;

    //a negative distCenterToKernelBorder value would mean that the distanceVector is longer than the support radius and hence the particles having this
    //distance don't interact with each other;
    //also catch length 0 disc vector to catch div by zero; anyway; a particle shall not push away itself ;)
    if((distCenterToKernelBorder >= 0.0f ) &&  (lenDistVec > EPSILON) )
    {
        // 45 / (pi * h^6) * (h-|r_vec|)^2 * r_vec / |r_vec|
        return 
          (
            ( cSimParams->gradientSpikyKernelConstantTerm *  distCenterToKernelBorder * distCenterToKernelBorder) 
              / lenDistVec 
          ) 
          * distanceVector; 
          
        //TODO use native functions when stable
        //return 
        //  native_divide(
        //    ( cSimParams->gradientSpikyKernelConstantTerm *  distCenterToKernelBorder * distCenterToKernelBorder ),
        //    lenDistVec
        //  ) 
        //  * distanceVector;
    }
    else
    {
      return (float4) (0.0f,0.0f,0.0f,0.0f);
    }
  }
  
  
  float laplacianViscosity(float4 distanceVector, __constant SimulationParameters* cSimParams)
  {
    float lenDistVec = length(distanceVector);
    //TODO use native functions when stable
    //float lenDistVec = sqrt( SQUARED_LENGTH(distanceVector) ) ;
    //float lenDistVec = native_sqrt( SQUARED_LENGTH(distanceVector) ) ;
    //float lenDistVec = fast_length( distanceVector )  ;
    
    float distCenterToKernelBorder = cSimParams->SPHsupportRadius - lenDistVec;

    //a negative distCenterToKernelBorder value would mean that the distanceVector is longer than the support radius and hence the particles having this
    //distance don't interact with each other;
    //don't catch length 0 disc vector; Although a particle shall not glue  itself, 
    //this case is autimatically catches as the relative velocity to itself is also zero ;) ;)
    if((distCenterToKernelBorder >= 0.0f ) )
    {
        // 45 / (pi * h^6) * (h-r)
        return cSimParams->laplacianViscosityConstantTerm * distCenterToKernelBorder;
    }
    else
    {
      return 0.0f;
    }
  }
  
  
  
#endif //FLEWNIT_CL_PROGRAMS_PHYSICS_COMMON_FUNCTIONS_GUARD
