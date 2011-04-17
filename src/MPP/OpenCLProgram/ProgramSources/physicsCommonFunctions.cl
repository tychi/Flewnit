  
#ifndef FLEWNIT_CL_PROGRAMS_PHYSICS_COMMON_FUNCTIONS_GUARD
#define FLEWNIT_CL_PROGRAMS_PHYSICS_COMMON_FUNCTIONS_GUARD

  {% include "common.cl" %}
  {% include "physicsDataStructures.cl" %}
  

  uint getZIndex(
    float4 position,
    __constant SimulationParameters* cSimParams,
    __constant uint* cGridPosToZIndexLookupTable )
  {
    //calculate floating point cell index, convert to integral type and round to nearest zero
    int4 cell3Dindex = convert_int4_rtz( 
      (position - cSimParams->uniGridWorldPosLowerCorner ) *  cSimParams->inverseUniGridCellSizes    
    );
    
    //repeat infinitely in order to make simulation domain infinite
                    //%= NUM_UNIGRID_CELLS_PER_DIMENSION ;
    cell3Dindex.x = BASE_2_MODULO( cell3Dindex.x, NUM_UNIGRID_CELLS_PER_DIMENSION ); 
    cell3Dindex.y = BASE_2_MODULO( cell3Dindex.y, NUM_UNIGRID_CELLS_PER_DIMENSION ); 
    cell3Dindex.z = BASE_2_MODULO( cell3Dindex.z, NUM_UNIGRID_CELLS_PER_DIMENSION ); 
    
    return (
      gridPosToZIndexLookupTable[ 0* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.x ]
      |
      gridPosToZIndexLookupTable[ 1* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.y ]
      |
      gridPosToZIndexLookupTable[ 2* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.z ]
    );
  }
  
  //-------------------------------------------------------------------------
  
  float poly6(float4 distanceVector, __constant SimulationParameters* cSimParams)
  {
    float sqaredDistCenterToKernelBorder = cSimParams->SPHsupportRadiusSquared - SQUARED_LENGTH(distanceVector);
    
    //a negative value would mean that the distanceVector is longer than the support radius and hence the particles having this
    //distance don't interact with each other
    if(sqaredDistCenterToKernelBorder >= 0.0f )
    {
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
  
  
  float laplacianViscosity(float4 distanceVector, __constant SimulationParameters* cSimParams))
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
