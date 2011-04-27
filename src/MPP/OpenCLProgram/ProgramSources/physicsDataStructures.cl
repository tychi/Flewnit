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

  namespace CLshare
  {
    
    //provide typename compatibility;
    typedef Flewnit::Vector4D float4;
    typedef Flewnit::Matrix4x4 float16;
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
    float gasConstant; //temperature dependent value for pressure computation, 
                       //(physically plausible only for fluids, 
                       //but used to calculate pressure for ANY particle during simulation;

    float viscosity;   //only relevant for fluids
    
    //as fluids can have different particle amounts, on-the-fly-computation of the offset gets hard,
    //as it depends on the particle count of the objects with lower ObjectID;
    //so lets store this offset here;
    uint offsetInIndexTableBuffer;
    
    uint padTo2,padTo3,padTo4;
    
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
     
    #ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
      //setter for c++ app ;)
      void setInfluenceRadius(float influenceRadius)
      {
        squaredInfluenceRadius = influenceRadius * influenceRadius;
        inverseSquaredInfluenceRadius = 1.0f/ squaredInfluenceRadius;
      }
    #endif //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
     
  } UserForceControlPoint;
  
  
  /*
    ParticleRigidBody object meta info;
    
    see updateRigidBodies.cl for further information;
  */
  typedef struct
  {    
    //Transformation matrix to update the scene graph global transform of the rigid body object and hence to provide a transform for the glsl vertex shader
    //(which is using the unrotated relative positions as vertex input);
    //kernel access read/write;
    //app access: init: write;
    //            default: read;
    float16 transformationMatrix;
    //kernel access read/write;
    //app access: init and default: write;
    //            if unsued: init to identity;
    float16 correctiveTransformationMatrix;
    

    //float4 centreOfMassPosition;
    
    float4 linearVelocity;
    float4 angularVelocity;


    float4 friction; //unused yet; maybe useful for collision with static geometry;
    
    
    float massPerParticle;    //will also be in gObjectParticleMasses
    float densityPerParticle; //precomputed by host; doesn't depend on particle count; 
                              //some kind of massPerParticle*(numParticleizationVoxels/rigidBodyAABBVolume)
                              //           = massPerParticle/(rigidBodyAABBVolume / numParticleizationVoxels)
                              //           = massPerParticle/particleizationVoxelVolume
                              //           = massPerParticle*inverseParticleizationVoxelVolume
                             
    //because updateRigidBody.cl performs parallel scans and hence each work item treats two particles,
    //the number of paticles MUST be even!! the side effects of one "forgotten" or even worse - missread
    //particles could blow up the wohl simulation; the control flow to catch this in a kernel is way too costly
    //(at least three more if()'s), so ensure that the particleization handles this, e.g. by ignoring the last particle
    //if its index is even... this could result in a slightly "odd" (what a  word wit ;( ) simulation behaviour,
    //but won't blow up!
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
        
  } ParticleRigidBody;
  
  
  
 
  /**
    SimulationParameters structure; Passed via a buffer to __constant memory at every physics kernel invocation;
    This way, every crucial information is available at maximum speed without blowing up the kernel's parameter lists;
  */
  typedef struct  
  {
    //{ uniform grid params
    //calced by app to save offset calculations: worldPositionCenter - (( (float)(numCellsPerDimension) )*0.5f) * uniGridCellSizes
    float4 uniGridWorldPosLowerCorner;
    float4 uniGridCellSizes;
    float4 inverseUniGridCellSizes;
    //}


    AABB simulationDomainBorders; //as long i have no triangle-collision-mesh functionality, i have to keep the particles
                                  //in a lame box ;(

    float4 gravityAcceleration; //gravity is an acceleration, yields different forces depending on mass, hence must be passed as gravity;
    

    
    uint numUserForceControlPoints;
    
    float penaltyForceSpringConstant;
    float penaltyForceDamperConstant;    
    
    
    
    

    
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



    float timestep;
    float squaredTimestep;
    float inverseTimestep; //needed for velocity prediction;
                           //as this structure is in constant memory, read speed is as fast as register read speed
                           //and hence precomputation is a valid optimization here to replace a costly division by a cheaper multiplication;
   
    
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
  } //end namespace CLshare 
#endif //FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
  
  
  
#endif //FLEWNIT_CL_PROGRAMS_PHYSICS_DATA_STRUCTURES_GUARD
