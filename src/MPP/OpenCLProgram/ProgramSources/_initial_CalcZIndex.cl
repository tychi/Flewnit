

//z index calculation program for inititial computation

//during simulation, the z-index is calculated within the integration kernel in order to save one kernal invocation

  
//default: 64
#define UNIGRID_NUM_CELLS_PER_DIMENSION ( {{uniGridNumCellsPerDimension}} )
//default: 6
#define LOG2_UNIGRID_NUM_CELLS_PER_DIMENSION ( {{log2UniGridNumCellsPerDimension}} )

uint getZIndex(float4 position, float4 uniGridWorldPosLowerCorner, float inverseUniGridCellSize,  __constant uint* gridPosToZIndexLookupTable )
{
  //calculate floating point cell index, convert to integral type and round to nearest zero
  int4 cell3Dindex = convert_int4_rtz( 
    (position - uniGridWorldPosLowerCorner ) * inverseUniGridCellSize    
  ) 
  //repeat infinitely in order to make simulation domain infinite
  % NUM_UNIGRID_CELLS_PER_DIMENSION ;
  
  return (
    gridPosToZIndexLookupTable[ 0* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.x ]
    |
    gridPosToZIndexLookupTable[ 1* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.y ]
    |
    gridPosToZIndexLookupTable[ 2* UNIGRID_NUM_CELLS_PER_DIMENSION + cell3Dindex.z ]
  );
}
 

__kernel void _initial_CalcZIndex(
  __global float4* gPositions,  //arg0
  __global uint* gZIndices,     //arg1
  __constant uint* gridPosToZIndexLookupTable, //arg2
  //calced by app to save offset calculations: worldPositionCenter - float(numCellsPerDimension)*0.5f*cellSize
  float4 uniGridWorldPosLowerCorner, //arg3
  float inverseUniGridCellSize //arg4
)
{
  gZIndices[get_global_id(0)] = getZIndex(gPositions[get_global_id(0)],uniGridWorldPosLowerCorner,inverseUniGridCellSize, gridPosToZIndexLookupTable);
}
