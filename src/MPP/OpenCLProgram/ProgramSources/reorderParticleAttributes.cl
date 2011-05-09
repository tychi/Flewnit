  //=====================================================================================
  /*
    Kernel to reorder the actual physical attributes on the basis of the gReorderedValueIndices buffer written in the last pass
    of the kernel_radixSort_reorder_Phase;
    TODO optimize: merge the latter and this kernel; 
                   Pros: one less kernel invokation and gReorderedValueIndices write;
                   Cons: modularization broken due to mix up of generic sorting and physics simulation specific code                  
  */
  
  {% include "physicsDataStructures.cl" %}
  
  __kernel 
  void kernel_reorderParticleAttributes(
  
    __constant ObjectGenericFeatures* cObjectGenericFeatures,
     
    //lookup of old index whose element is to be copied to the current index (global work item ID) 
    __global uint* gReorderedOldIndices,

    //tracker to follow where the particles belonging to a meta object (Fluid, Rigid body) are;
    //used as OpenGL index buffer for fluids, OpenGL index buffer for rigid body degug draw and for
    //acquisition of the relevant particles for the ridgid-body meta info calculation 
    //(centre of mass, linear and angular velocity, transform)
    __global uint* gParticleIndexTable, 
    
    //needed to identify for a particle to which meta object is belongs (fluid, rigid body or invalid )
    __global uint* gParticleObjectInfosOld,
    __global uint* gParticleObjectInfosReordered,

    __global float4* gPositionsOld,
    __global float4* gPositionsReordered,
    
    //if doing "deferred density" optimization to save one kernel invocation, 
    //we have in return to pass the last frame's densities to the next frame,
    //hence we also have to reorder them ;(
    __global float* gDensitiesOld, 
    __global float* gDensitiesReordered,
    
    __global float4* gCorrectedVelocitiesOld,
    __global float4* gCorrectedVelocitiesReordered, 
    
    __global float4* gPredictedVelocitiesOld, 
    __global float4* gPredictedVelocitiesReordered, 
    
    __global float4* gAccelerationsOld,
    __global float4* gAccelerationsReordered

  )
  {
    uint gwiID = get_global_id(0);
    uint oldIndex = gReorderedOldIndices[ get_global_id(0) ];
    uint particleObjectInfo =  gParticleObjectInfosOld[ oldIndex ];
    
    //update the index of this particle in the "object particle index tracking list"
    //note: even the invalid particles have their own index table; branching for those few particles doesn't make sense;
    //plus, with tracking the invalids; we can put them into the simulaton domain where we want...
    //Can't help neutrinos coming into my mind... ;)
   
    gParticleIndexTable[ 
        cObjectGenericFeatures[ GET_OBJECT_ID( particleObjectInfo ) ].offsetInIndexTableBuffer
        + GET_PARTICLE_ID( particleObjectInfo ) 
      ] = gwiID;
    

    gParticleObjectInfosReordered[ gwiID ] = particleObjectInfo;

/*
    //debug
    float4 posOld = gPositionsOld[ oldIndex ];
    if(any(isnan(posOld))) {posOld =  (float4)(1.0f,30.0f,1.0f,0.0f); }
    gPositionsReordered[ gwiID ] =posOld;

    float4 corrVelOld = gCorrectedVelocitiesOld[ oldIndex ];
    if(any(isnan(corrVelOld))) {corrVelOld =  (float4)(0.0f,0.0f,0.0f,0.0f); }
    gCorrectedVelocitiesReordered[ gwiID ] =corrVelOld;

    float4 predVelOld = gPredictedVelocitiesOld[ oldIndex ];
    if(any(isnan(predVelOld))) {predVelOld =  (float4)(0.0f,0.0f,0.0f,0.0f); }
    gPredictedVelocitiesReordered[ gwiID ] =predVelOld;

    float4 accOld = gAccelerationsOld[ oldIndex ];
    if(any(isnan(accOld))) {accOld =  (float4)(0.0f,0.0f,0.0f,0.0f); }
    gAccelerationsReordered[ gwiID ] =accOld;
*/


    gPositionsReordered[ gwiID ] =  gPositionsOld[ oldIndex ];
    
    gCorrectedVelocitiesReordered[ gwiID ] =  gCorrectedVelocitiesOld[ oldIndex ];
    
    //those two copies hurt:
    //the following buffers are needed due to the usage of the velocity verlet integration;
		//TODO as this considerably increases bandwidth (also of all things in scattered reordering),
		//we maybe should consider a different integration scheme where no force and only one velocity
		//is sufficient;
    gPredictedVelocitiesReordered[ gwiID ] =  gPredictedVelocitiesOld[ oldIndex ];
    gAccelerationsReordered[ gwiID ] =  gAccelerationsOld[ oldIndex ];

    
    
    
  }
  //=====================================================================================
  
  
