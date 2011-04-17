/*

  physicsDataStructures.cl

  
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


  
#endif //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  
  
  

  typedef struct 
  {
    float4 minPos;
    float4 maxPos;
  } AABB;
  
  
  typedef struct 
  {
    float massPerParticle;
    float restDensity; //is always the density for rigid bodies, 
                       //is rest density for pressure compuation to numerically stablelize the pressure gradient compuation;
    float gasConstant; //for pressure computation, (physically plausible only for fluids, 
                       //but used to calculate pressure for ANY particle during simulation; this "dirty" way, one can adjust the repulsion force
                       //for rigid bodies seperately);
    float viscosity;   //only relevant for fluids
    
  } ObjectGenericFeatures;
  
  
  typedef struct
  {
  
    float4 forceOriginWorldPos; //where is  the control point?
    
    //how big shall be the region where the force has influence? 
    //(we don't wanna attract objects over an infinite distance 
    //float influenceRadius;  <-- actually not even needed
    float squaredInfluenceRadius;
    float inverseSquaredInfluenceRadius;
     
    float intensity; //positive: push away; negative: pull towards origin;
     
     
    uint targetObjectID; //object which is influence by the force; a certain rigid body or a certain fluid object; 
     

     //structure implicitely aligned to 2*(4*4)=32 bytes;
     
  } UserForceControlPoint;
  
  
  
   typedef struct
  {
    //for a simulation where we need to exact control over a rigid body (e.g. when we want to paddle in the water to move a canoe),
    //trying this via applying forces to certain points/regions might become quite messy;
    //therefor,the host know a target position and orientation th rigid body shall converge to;
    //The host takes the angle between the current direction and the target direction, multiplicates it with a time dependent constant,
    //and and rotates the current direction and current up vector around the cross product of the current and target correction by the time dependent 
    //angle portion;
    //same procuedure is done with the rotated dir and up for the up-vector;
    //the final resulting "interpolated target dir and up" are put intor a rotation matrix; target position is also 
    //interpolated and put into the matrix;
    //accumulating this matrix in the cl kernel with the "physics dependent on the fly calculated"matrix yields the final transformations matrix,
    //with which are all relatvie RB-particles are transformed.
    float16 correctiveTransformationMatrix; 
    
    //direction, direction and upVector are read back by the host to construct a transformation matrix
    //to update both the scene graph transform ogf the rigid body object and to provide a transform for the glsl vertex shader;
    float4 centreOfMassPosition;
    
    float4 direction; //rotated around angular velocity by  length(angularVelocity)*timeStep radians;
                      //transformationMatrix= constructTransfPormationMatrix(
                      //  centreOfMassPosNew, normalize(angularVelocity), length(angularVelocity)*timeStep );
                      //directionNew,  = transformationMatrix * directionOld;
                      //n.b.:(applxy same transform on all old particle relative values);

    float4 upVector;  //influenced by angular velocity;
                      //caclulation is the same as for direction;
    
    float4 linearVelocity;
    float4 angularVelocity;
    
    
    float massPerParticle;    //will also be in gObjectParticleMasses
    float densityPerParticle; //precomputed by host; doesn't depend on particle count; 
                              //some kind of massPerParticle*(numParticleizationVoxels/rigidBodyAABBVolume)
                              //           = massPerParticle/(rigidBodyAABBVolume / numParticleizationVoxels)
                              //           = massPerParticle/particleizationVoxelVolume
                              //           = massPerParticle*inverseParticleizationVoxelVolume
                              
    uint numContainingParticles;
    float inverseNumContainingParticles;
    
    //1/(sum( length(centreOfMassRelParticlePos)^2 )); Needed for angular vel. approximation; precomputed by host
    float inverseTotalSquaredDistancesFromCenterOfMass; 

    //uint ID; unneeded as implcitely encoded in position in gRigidBodies; 
             //start index in gRigidBodyIndices = ID*;
             //gRigidBodyIndices holds for all particles belonging to a rigid body (and NOT the fluid)
             //a list of indices where a particle is in the buffers for physical attributes;
             //due to movement an z-Index curve, the particles belonging to a distinct rigid body
             //are not guaranteed to be adjacent in the physical attribute buffer;
             //gRigidBodyIndices provides tracking functionality to directly acces any particle needed for rigid body 
             //computations;
             
    //TODO adjust padding to align to 128 bytes 
        
  } RigidBody;
  
  
  
 
  /**
    SimulationParameters structure; Passed via a buffer to __constant memory at every physics kernel invocation;
    This way, every crucial information is available at maximum speed without blowing up the kernel's parameter lists;
  */
  typedef struct  
  {
    AABB simulationDomainBorders; //as long i have no triangle-collision-mesh functionality, i have to keep the particles
                                  //in a lame box ;(

    float4 gravityAcceleration; //gravity is an acceleration, yields different forces depending on mass, hence must be passed as gravity;
    

    //{ uniform grid params
    //calced by app to save offset calculations: worldPositionCenter - (( (float)(numCellsPerDimension) )*0.5f) * uniGridCellSizes
    float4 uniGridWorldPosLowerCorner;
    float4 uniGridCellSizes;
    float4 inverseUniGridCellSizes;
    //}
    
    uint numUserForceControlPoints;
    

    float penaltyForceSpringConstant;
    float penaltyForceDamperConstant;    
    
    
    float timestep;
    float squaredTimestep;
    float inverseTimestep; //needed for velocity prediction;
                           //as this structure is in constant memory, read speed is as fast as register read speed
                           //and hence precomputation is a valid optimization here to replace a costly division by a cheaper multiplication;
                          
    
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
    
    
    //float padForce0; //stuff to enforce 16-byte alignment of following vector types independent from compiler features



    
    //{ 
    //  SPH definitions; refer to "Particle-Based Fluid Simulation for Interactive Applications" by Matthias Mueller et. al.
    //  for further info;
    
    //must be <= uniGridCellSize; 
    float SPHsupportRadius;
    float SPHsupportRadiusSquared; //as this structure is in constant memory, read speed is as fast as register read speed
                                   //and hence precomputation is a valid optimazation here to save one multiplication;
    
    
    //constant terms of used SPH kernels, precomputed by app in order to save calculations in a VERY performance critical code section
    //( 315.0f / ( 64.0f * PI * pown(SPH_SUPPORT_RADIUS, 9) ) )
    float poly6KernelConstantTerm;
    //( 45.0f / ( PI * pown(SPHsupportRadius,6) ) ) <-- 3 * spiky constant term; non-negative because it is the derivative 
    //"in the direction towards the center", hence coming from positive infinity going to origin, the gradient is positive
    //as the values become greater towards the origin;
    float gradientSpikyKernelConstantTerm;
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) <-- that this is the same value as gradientSpikyKernelConstantTerm is
    //a "coincidence"; the true laplacian of the viscosity kernel would be 
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) * (h-r  - ((h^4)/(r^3)) ) instead of
    //( 45.0f / ( PI * pown(SPH_SUPPORT_RADIUS,6) ) ) * (h-r)                   denoted in the paper; a reason wasn't given;
    //I can only speculate that this is a reasonable simplification to trade speed for accuracy;
    float laplacianViscosityConstantTerm;

    //} end SPH definitions

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
   //#define GET_RIGID_BODY_ID ( particleObjectInfo ) ( ( GET_OBJECT_ID(particleObjectInfo) ) - RIGID_BODY_OFFSET )
   #define GET_RIGID_BODY_ID ( particleObjectID ) ( ( particleObjectID ) - RIGID_BODY_OFFSET )
                         

  //(1<<11)=2048; reason: this is the maximum number of elements we can scan in parallel within a single work group
  //without any "tricks"; we need the total sums of positions and velocities
  #define NUM_MAX_PARTICLES_PER_RIGID_BODY (1<<11)
  //default: 1: if we wanna simulate different fluids types (i.e. fluid particles can have  different features),
  //we might increase this offset, but for now, let's not think about TOO much special cases/extension possibilites...
  #define RIGID_BODY_OFFSET ( 1 )
  //#define IS_RIGID_BODY_PARTICLE ( particleObjectInfo ) ( ( GET_OBJECT_ID(particleObjectInfo) ) >= RIGID_BODY_OFFSET )                       
  #define IS_RIGID_BODY_PARTICLE ( particleObjectID ) ( ( particleObjectID ) >= RIGID_BODY_OFFSET )                       
  #define IS_FLUID_PARTICLE ( particleObjectID ) ( ( particleObjectID ) < RIGID_BODY_OFFSET )
     
  #define BELONGS_TO_FLUID      ( particleObjectID ) ( ( particleObjectID ) <  RIGID_BODY_OFFSET )
  #define BELONGS_TO_RIGID_BODY ( particleObjectID ) ( ( particleObjectID ) >= RIGID_BODY_OFFSET )      
 
 //}  end particle object info
 //------------------------------------------------------------------------------------------------------                      
   
  

  
  
  
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
