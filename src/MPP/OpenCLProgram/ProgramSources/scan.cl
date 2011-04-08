/*
  Work efficient parallel prefix sum (scan);
  Algoritm completely taken from http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html
  
  There is an issue with the padding macro for bank conflict avoidance; See bankConflictsAvoidance.cl for further info

*/

{% include bankConflictsAvoidance.cl %}



   //-------------------------------------------------------------------------------------
   //returns the total sum
   uint scanExclusive(
      __local uint* arrayToScan, //correctly padded element input/output array
      uint numElements, 
      uint workItemOffsetID, //usually just get_local_id(0), but when scanning several small arrays within the same work group,
                            //the work item id may be smaller than workItemOffsetID in order to fit the index calculations
     
   )
   {
      //no read in, everything is at the correct place in local memory
      
      scanExclusive_upSweep(arrayToScan, numElements, workItemOffsetID);
      
      uint totalSum = arrayToScan[ CONFLICT_FREE_INDEX( numElements - 1 ) ];
      if (workItemOffsetID == 0) 
      { 
        arrayToScan[ CONFLICT_FREE_INDEX(numElements - 1) ] = 0;// clear the last element
      } 
      
      scanExclusive_downSweep(arrayToScan, numElements, workItemOffsetID);
      
      //ensure that the caller can read any value without hazard;
      barrier(CLK_LOCAL_MEM_FENCE);  
   
      return totalSum;
   }
  
  
  //-------------------------------------------------------------------------------------
  void scanExclusive_upSweep(__local uint* arrayToScan, uint numElements, uint workItemOffsetID)
  {
      int indexOffset = 1; 
      for (
        int d = (numElements >> 1); //numElements/2
        d > 0;  //TODO try optimization: d >1; this way, the last upsweep iteration is omitted; this one is totally obsolete,
                //because the last element is set to zero before the downsweep anyway
        d >>= 1  // d/=2;
      )    
      {   
        barrier(CLK_LOCAL_MEM_FENCE);  
        if (workItemOffsetID < d)  
        {  
          uint lowerIndex = indexOffset * ( 2 * workItemOffsetID + 1) -1;  
          uint higherIndex = indexOffset * ( 2 * workItemOffsetID + 2) -1;  
          lowerIndex += CONFLICT_FREE_OFFSET(lowerIndex);  
          higherIndex += CONFLICT_FREE_OFFSET(higherIndex);  
          arrayToScan[higherIndex] += arrayToScan[lowerIndex];  
        }
        indexOffset <<= 1; //indexOffset *= 2; 
      }  
  }
  //-------------------------------------------------------------------------------------
  
  //-------------------------------------------------------------------------------------
  void scanExclusive_downSweep(__local uint* arrayToScan, uint numElements, uint workItemOffsetID)
  {
      int indexOffset = numElements; 
      for (
        int d = 1;
        d < numElements;
        d <<= 1  // d *= 2;
      )    
      {   
        indexOffset >>= 1; //indexOffset /= 2; 
        barrier(CLK_LOCAL_MEM_FENCE);  
        if (workItemOffsetID < d)  
        {  
          uint lowerIndex = indexOffset * ( 2 * workItemOffsetID + 1) -1;  
          uint higherIndex = indexOffset * ( 2 * workItemOffsetID + 2) -1;  
          lowerIndex += CONFLICT_FREE_OFFSET(lowerIndex);  
          higherIndex += CONFLICT_FREE_OFFSET(higherIndex); 
           
          uint tempVal = arrayToScan[lowerIndex];
          arrayToScan[lowerIndex] = arrayToScan[higherIndex];  
          arrayToScan[higherIndex] += tempVal;
        }
      }  
  }
  //-------------------------------------------------------------------------------------
