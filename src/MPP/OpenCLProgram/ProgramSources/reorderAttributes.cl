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
    
    __global float4* gVelocitiesOld,
    __global float4* gVelocitiesReordered,
    
    __global ParticleRigidBodyInfo* gParticleRigidBodyInfosOld,
    __global ParticleRigidBodyInfo* gParticleRigidBodyInfosReordered,
    
    __global uint* gRigidBodyParticleIndexTable //numRigidBodies * NUM_MAX_PARTICLES_PER_RIGID_BODY elements
  )
  {
    uint gwiID = get_global_id(0);
    uint oldIndex = gReorderedOldIndices[ get_global_id(0) ];
    
    gPositionsReordered[ gwiID ] =  gPositionsOld[ oldIndex  ];
    gVelocitiesReordered[ gwiID ] =  gVelocitiesOld[ oldIndex  ];
    
    ParticleRigidBodyInfo prbi =  gParticleRigidBodyInfosOld[ oldIndex  ];
    if( IS_RIGID_BODY_PARTICLE(prbi) )
    { 
      //update the index of this rigid body particle in the "rigid bodies' particle index tracking list"
      gRigidBodyParticleIndexTable[ prbi.rigidBodyID * NUM_MAX_PARTICLES_PER_RIGID_BODY + prbi.particleID ] = gwiID;
    }
    gParticleRigidBodyInfosReordered[ gwiID ] = prbi;
    
    //maybe to include here: TODO rigid body meta data calculation (index backtracking for rigid body structures etc..)
  
  }
  //=====================================================================================
