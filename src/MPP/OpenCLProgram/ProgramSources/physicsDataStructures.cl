/*
  This source file is both included by the C++ Application and by the OpenCL kernels;
  Hence, some conditional compilation must be done to mask incompatible code segments;
  
  Some poeple may call it a hack, but I'm doing so to ensure synch of data structures by omitting boilerplate duplicate definitions;
  The OpenCL API doesn't provide any type checks, alignment/layout checks etc, so boilerplate code is especially error prone here;
  
  By using this file in both host and device code, even some macros can be used by the app, so that consistency is guaranteed; 

*/

  
#ifndef FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD
#define FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD


#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

  namespace cl {
    
    //provide typename compatibility;
    typedef Flewnit::Vector4D float4;
    typedef unsigned int uint;
  
#else //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

  {% include common.cl %}
  {% include simulationDefinitions.cl %}
  
#endif //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  
  
  

  typedef struct 
  {
    float4 min;
    float4 max;
  } AABB;
 
  /**
    SimulationParameters structure; Passed via a buffer to __constant memory at every physics kernel invocation;
    This way, every crucial information is available at maximum speed without blowing up the kernel's parameter lists;
  */
  typedef struct  
  {
    AABB simulationDomainBorders; //as long i have no triangle-collision-mesh functionality, i have to keep the particles
                                  //in a lame box ;(

    float4 userForcePos;
    float4 userForce;
    float userForceRadius;
    float padForce1,padForce2,padForce3; //stuff to enforce alignment of following vector types independent from compiler features
    
    //{ uniform grid params
    
    //calced by app to save offset calculations: worldPositionCenter - (( (float)(numCellsPerDimension) )*0.5f) * uniGridCellSizes
    float4 uniGridWorldPosLowerCorner;
    float4 uniGridCellSizes;
    float4 inverseUniGridCellSizes;
    
    //}
    
    float timestep;
    
    //float massPerFluidParticle; <-- obsolete, is in __constant float* cObjectMassesPerParticle, index 0
    float inverseRigidBodyParticleizationVoxelVolume;//precomputed by host; needed to get density from mass for rigid boadies;
                              //precondition: The volume represented by a rigid body particle is the same for ALL rigid bodies!
                              //              This is a reaonable demand, because small volumes laed to "oversampling" and overpopulated
                              //              Uniform grid voxels, and large volumes cause the rigid body to be penetrated by fluid
                              //              particles; Unless you wanna simulate a sponge, this behaviour is not desired;
                              //Usage:
                              //rigidBodyDensity=  massPerParticle*(numParticleizationVoxels/rigidBodyAABBVolume)
                              //           = massPerParticle/(rigidBodyAABBVolume / numParticleizationVoxels)
                              //           = massPerParticle/particleizationVoxelVolume
                              //           = massPerParticle*inverseParticleizationVoxelVolume
    float gravityAcceleration;
    
    
    float penaltyForceSpringConstant;
    float penaltyForceDamperConstant;
   
    
    //uint numRigidBodies; //must be power of two
    
  } SimulationParameters;
 
 
  //------------------------------------------------------------------------------------------------------    
  //{ particle object info

  /*
  My first approach was the folloing struct; But then i realized that when binding a buffer containing such a structure
  to OpenGL, maybe strange effects like dependency on endianess may occur; Plus, the fact that both ID values get the same
  bit count doesn't reflect that there are much more particles than "abstract" objects like fluids or rigid bodies;
  Additionally, the internal access to non-4byte-aligned values might also work only via bit shiftings, so that there would be
  no performance gain against working with explicit codings intor bit fields;
  To conclude, the two IDs of the "ParticleObjectInfo" are now encoded into a uint, see below;
  
  typedef struct
  { 
  
    ushort objectID; //belongs to fluid if is < RIGID_BODY_OFFSET, else belongs to rigidbody with ID (objectID - RIGID_BODY_OFFSET);
                     //This rather complicated ID handling comes from the fact that we want omit branches (at least in simulation phases
                     //which don't compute densities) and hence we retrieve the particle masses via array indexing instead of 
                     //branching
    ushort particleID;  //gRigidBodyParticleIndexTable [ rigidBodyID * NUM_MAX_PARTICLES_PER_RIGID_BODY + particleID ]
                        //yields the final index of the particle's attributes in the current physical attribute buffers
                        
  } ParticleObjectInfo;
  */
  
  //20 bits for particle ID; reason; more than one million particles in a real time simulation seems unrealistic at the moment;
  //This way, we can uniquely track _every_ particle, independent of the object id the particle belongs to; This allows e.g.
  //unique shading and other interesting debug stuff ;(
  #define NUM_BITS_FOR_PARTICLE_ID (20)
  #define PARTICLE_ID_BIT_MASK ( (1 << NUM_BITS_FOR_PARTICLE_ID) -1 )
  //rest of the 32 bit uint for the object ID, i.e 12 bits; 
  //#define NUM_BITS_FOR_OBJECT_ID ( (8 * sizeof(uint)) - NUM_BITS_FOR_PARTICLE_ID )
  #define NUM_BITS_FOR_OBJECT_ID ( 32 - NUM_BITS_FOR_PARTICLE_ID )
  #define OBJECT_ID_BIT_MASK ( ~(PARTICLE_ID_BIT_MASK) )
  
  #define GET_OBJECT_ID(particleObjectInfo) ( (particleObjectInfo) >> NUM_BITS_FOR_PARTICLE_ID )
  //mask all bits more significant than NUM_BITS_FOR_PARTICLE_ID
  #define GET_PARTICLE_ID(particleObjectInfo) ( (particleObjectInfo) &  PARTICLE_ID_BIT_MASK )
 
  #define SET_OBJECT_ID(particleObjectInfo, objectID) \
    particleObjectInfo = (objectID << NUM_BITS_FOR_PARTICLE_ID) \
                         + GET_PARTICLE_ID(particleObjectInfo)                         
  #define SET_PARTICLE_ID(particleObjectInfo, particleID) \
    particleObjectInfo = (particleObjectInfo & OBJECT_ID_BIT_MASK )\
                         +particleID
                         
  //convenience macros to handle offset stuff for rigid bodies
  //be sure to use this makro only if IS_RIGID_BODY_PARTICLE() returns true!
   #define GET_RIGID_BODY_ID ( particleObjectInfo ) ( ( GET_OBJECT_ID(particleObjectInfo) ) - RIGID_BODY_OFFSET )
                         

  //(1<<11)=2048; reason: this is the maximum number of elements we can scan in parallel within a single work group
  //without any "tricks"; we need the total sums of positions and velocities
  #define NUM_MAX_PARTICLES_PER_RIGID_BODY (1<<11)
  //default: 1: if we wanna simulate different fluids types (i.e. fluid particles can have  different features),
  //we might increase this offset, but for now, let's not think about TOO much special cases/extension possibilites...
  #define RIGID_BODY_OFFSET ( 1 )
  #define IS_RIGID_BODY_PARTICLE ( particleObjectInfo ) ( ( GET_OBJECT_ID(particleObjectInfo) ) >= RIGID_BODY_OFFSET )                       
 
 //}  end particle object info
 //------------------------------------------------------------------------------------------------------                      
   
  
 typedef struct
  {
    float4 centreOfMassPosition;
    float4 direction; //influenced by angular velocity;
    
    float4 linearvelocity;
    float4 angularVelocity;
    
    uint numContainingParticles;
    
    float massPerParticle;    //will also be in gObjectParticleMasses
    float densityPerParticle; //precomputed by host; doesn't depend on particle count; 
                              //some kind of massPerParticle*(numParticleizationVoxels/rigidBodyAABBVolume)
                              //           = massPerParticle/(rigidBodyAABBVolume / numParticleizationVoxels)
                              //           = massPerParticle/particleizationVoxelVolume
                              //           = massPerParticle*inverseParticleizationVoxelVolume
    
    float inverseTotalSquaredDistancesFromCenterOfMass; //needed for angular vel. approximation; precomputed by host

    //uint ID; unneeded as implcitely encoded in position in gRigidBodies; 
             //start index in gRigidBodyIndices = ID*;
             //gRigidBodyIndices holds for all particles belonging to a rigid body (and NOT the fluid)
             //a list of indices where a particle is in the buffers for physical attributes;
             //due to movement an z-Index curve, the particles belonging to a distinct rigid body
             //are not guaranteed to be adjacent in the physical attribute buffer;
             //gRigidBodyIndices provides tracking functionality to directly acces any particle needed for rigid body 
             //computations;
        
  } RigidBody;
  
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
#endif //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  
  
  
#endif //FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD
