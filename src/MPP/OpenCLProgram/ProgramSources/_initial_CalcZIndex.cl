/**

  Z-index calculation program for inititial computation

  During simulation, the Z-index is calculated within the integration kernel in order to save one kernal invocation

*/


{% include physicsCommonFunctions.cl %} 

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
