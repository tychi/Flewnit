  
#ifndef FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD
#define FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD


#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  
  
  namespace cl {
    
    typedef Flewnit::Vector4D float4;
  
  
#else
  {% include common.cl %}
  {% include simulationDefinitions.cl %}
#endif
  
  
  

  typedef struct 
  {
    float4 min;
    float4 max;
  } AABB;
 
  

  struct SimulationParameters
  {
    AABB simulationDomainBorders; //as long i have no triangle-collision-mesh functionality, i have to keep the particles
                                  //in a lame box ;(

    float4 userForcePos;
    float4 userForce;
    float userForceRadius;
    float padForce1,padForce2,padForce33; //stuff to enforce alignemt independent from compiler features
    
    //{ uniform grid params
    
    float4 uniGridWorldPosLowerCorner;
    float uniGridCellSize;
    float inverseUniGridCellSize;
    
    //}
    
    float timestep;
    
    float massPerFluidParticle;
    float gravityAcceleration;
    
    
    float penaltyForceSpringConstant;
    float penaltyForceDamperConstant;
    
    //uint numRigidBodies; //must be power of two
    
  };
  
  typedef struct
  { 
  
    ushort rigidBodyID; //INVALID_RIGID_BODY_ID if particle is standard fluid particle, i.e. does not belong to a rigid body
    ushort particleID;  //gRigidBodyParticleIndexTable [ rigidBodyID * NUM_MAX_PARTICLES_PER_RIGID_BODY + particleID ]
                        //yields the final index of the particle's attributes in the current physical attribute buffers
                        
  } ParticleRigidBodyInfo; 
  
 typedef struct
  {
    float4 centreOfMassPosition;
    float4 direction; //influenced by angular velocity;
    
    float4 linearvelocity;
    float4 angularVelocity;
    
    uint numContainingParticles;
    
    float massPerParticle; //
    float densityPerParticle; //precomputed by host doesn't depend on particle count; 
                              //some kind of numVoxels*massPerParticle/rigidBodyVolume;
    
    float inverseTotalSquaredDistancesFromCenterOfMass; //needed for angular vel. approximation; precomputed by host

    //uint ID; unneeded as implcitely encoded in position in gRigidBodies; 
             //start index in gRigidBodyIndices = ID*;
             //gRigidBodyIndices holds for all particles belonging to a rigid body (and NOT the fluid)
             //a list of indices where a particle is in the buffers for physical attributes;
             //due to movement an z-Index curve, the particles belonging to a distinct rigid body
             //are not guaranteed to be adjacent in the physical attribute buffer;
             //gRigidBodyIndices provides tracking functionality to directly acces any particle needed for rigid body 
             //computations;
        
  }RigidBody;
  
  /**
  For time pressure reasons, i have to skip consideration about triangle data structures and related computations;
  It's enough to strongly believe that a belated integration of static triangle collision meshes will be possible without
  TOO much code change...
  typedef struct
  {
    uint indexVert1,indexVert2,indexVert3;
  } Triangle;
  
  //__global float4* triangleVertices; //z-curve-sorted 3d-positions, like particle positions;
  //__global Triangle* triangles;      
  //__global uint* gUniGridCells_TriangleStartIndex
  */
  
#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  } //end namespace cl 
#endif
  
  
  
#endif //FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD
