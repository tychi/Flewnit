  //=====================================================================================
  /*
    Kernel to reorder the actual physical attributes on the basis of the gReorderedValueIndices buffer written in the last pass
    of the kernel_radixSort_reorder_Phase;
    TODO optimize: merge the latter and this kernel; 
                   Pros: one less kernel invokation and gReorderedValueIndices write;
                   Cons: modularization broken due to mix up of generic sorting and physics simulation specific code                  
  */
  
  {% include physicsDataStructures.cl %}
  
  __kernel void kernel_reorderPhysicalAttributes(
    __global uint* gReorderedOldIndices,

    __global float4* gPositionsOld,
    __global float4* gPositionsReordered,
    
    //__global float4* gVelocitiesOld,
    //__global float4* gVelocitiesReordered,
    __global float4* gCorrectedVelocitiesOld,
    __global float4* gCorrectedVelocitiesReordered, 
    __global float4* gPredictedVelocitiesOld, 
    __global float4* gPredictedVelocitiesReordered, 
    
    __global uint* gParticleObjectInfosOld,
    __global uint* gParticleObjectInfosReordered,
    
    {% if rigidBodySimulation %}
      __global uint* gRigidBodyParticleIndexTable //numRigidBodies * NUM_MAX_PARTICLES_PER_RIGID_BODY elements
    {% endif %}
  )
  {
    uint gwiID = get_global_id(0);
    uint oldIndex = gReorderedOldIndices[ get_global_id(0) ];
    
    gPositionsReordered[ gwiID ] =  gPositionsOld[ oldIndex ];
    
    gCorrectedVelocitiesReordered[ gwiID ] =  gCorrectedVelocitiesOld[ oldIndex ];
    gPredictedVelocitiesReordered[ gwiID ] =  gPredictedVelocitiesOld[ oldIndex ];
    
    uint particleObjectInfo =  gParticleObjectInfosOld[ oldIndex ];
    gParticleObjectInfosReordered[ gwiID ] = particleObjectInfo;

    {% if rigidBodySimulation %}
      if( IS_RIGID_BODY_PARTICLE(particleObjectInfo) )
      { 
        //update the index of this rigid body particle in the "rigid bodies' particle index tracking list"
        gRigidBodyParticleIndexTable[ 
          GET_RIGID_BODY_ID( particleObjectInfo ) * NUM_MAX_PARTICLES_PER_RIGID_BODY 
          + GET_PARTICLE_ID( particleObjectInfo ) 
        ] = gwiID;
      }
    {% endif %}
    
  
  }
  //=====================================================================================
