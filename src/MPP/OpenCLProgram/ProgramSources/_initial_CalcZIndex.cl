/**

  Z-index calculation program for inititial computation

  During simulation, the Z-index is calculated within the integration kernel in order to save one kernal invocation

*/


{% include physicsCommonFunctions.cl %} 

__kernel void _initial_CalcZIndex(
  __constant uint* cGridPosToZIndexLookupTable, //lookup table to save some costly bit operations for z-Index calculation
  __constant SimulationParameters* cSimParams,
  __global float4* gPositions,
  __global uint* gZIndices,     

)
{
  gZIndices[get_global_id(0)] = getZIndex(gPositions[get_global_id(0)], cSimParams, cGridPosToZIndexLookupTable);
}
