
{% block documentHeader %}
  /**
    SPH calculations template with control flow code wich is the same for each computation type;
    To be derived e.g. by 
      - updateDensity.cl
      - _initial_updateForce_Integrate.cl (for verlocity verlet, we need two force values for one integration step,
                                           so the very first force value must be computed in an initial pass); This file is in turn used derive
        - updateForce_Integrate_calcZIndex.cl

  */ 
{% endblock documentHeader %}



{% block inclusions %}
  {% include "physicsCommonFunctions.cl" %}
{% endblock inclusions %}


  {% block functionDefinitions %} 

  {% endblock functionDefinitions %} 
  
  
  
  {% if  useCacheUsingOpenCLImplementation %}
      #define GET_CURRENT_NEIGHBOUR_POS (   gPositionsOld[ neighbourParticleStartIndex + interactingLocalIndex ] )
      #define GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID ( GET_OBJECT_ID( gParticleObjectInfos[ neighbourParticleStartIndex + interactingLocalIndex ] ) )
      #define GET_CURRENT_NEIGHBOUR_PARTICLE_PREDICTED_VEL_CURRENT ( gPredictedVelocitiesCurrent[ neighbourParticleStartIndex + interactingLocalIndex ] )
      #define GET_CURRENT_NEIGHBOUR_PARTICLE_DENSITY ( gDensitiesOld[ neighbourParticleStartIndex + interactingLocalIndex ] )
  {% else %}
    #define GET_CURRENT_NEIGHBOUR_POS (  lCurrentNeighbourPositions[ interactingLocalIndex ] )
    #define GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID ( lCurrentNeighbourParticleObjectIDs[ interactingLocalIndex ] )
    #define GET_CURRENT_NEIGHBOUR_PARTICLE_PREDICTED_VEL_CURRENT ( lCurrentNeighbourPredictedVelsCurrent[ interactingLocalIndex ] )
    #define GET_CURRENT_NEIGHBOUR_PARTICLE_DENSITY ( lCurrentNeighbourDensities[ interactingLocalIndex ] )
  {% endif %}
  
  
  
  __kernel 
  void 
{% block kernelName %}

{% endblock kernelName %}
  (
  
{% block constantBufferKernelArgs %} {%comment%}/*usually the same for all physics kernels*/{%endcomment%}
      
      __constant SimulationParameters* cSimParams,
      __constant uint* cGridPosToZIndexLookupTable, //lookup table to save some costly bit operations for z-Index calculation
      //RIGID_BODY_OFFSET + numRigidBodies elements; contains (amongs other properties) the masses of the fluid particles, 
      //then the masses of the respective rigid bodies; this way, we can grab the masses without any branching :).      
      //__constant float* cObjectMassesPerParticle, 
      __constant ObjectGenericFeatures* cObjectGenericFeatures,
      __constant UserForceControlPoint* cUserForceControlPoints, //cSimParams->numUserForceControlPoints elements
      
{% endblock constantBufferKernelArgs %}
    
    
{% block uniformGridBufferKernelArgs %}{%comment%}/*usually the same for all physics kernels*/{%endcomment%}
    
      //currentSimulationWorkGroupCount (valid) elements each, i.e. as many elements
      //as there were split and compacted uniform grid cells;                                        
      __global uint* gSimWorkGroups_ParticleStartIndex,
      __global uint* gSimWorkGroups_NumParticles,      
      //NUM_TOTAL_GRID_CELLS elements each; uncompressed structure needed to lookup neighbour particle indices via z-index; 
      //Plus, we need ALL particles for interaction, not only those of a sim work group;    
      __global uint* gUniGridCells_ParticleStartIndex, 
      __global uint* gUniGridCells_NumParticles,
      
{% endblock uniformGridBufferKernelArgs %}
    
    
{% block particleAttributesBufferKernelArgs %}
    
      //better a parameter list with in some kernels unused params than eternal confusion..
      
      //don't toggle this buffer after the kernel invocation
      __global uint* gParticleObjectInfos,

      //write-only buffer; IMPORTANT: bind the activ ping pong ccomponent to this arg, although it is a write arg;
      //don't toggle this buffer after the kernel invocation
      __global uint* gZindicesNew,
      
      //toggle all following particle attribute buffers after kernel invocation:
      __global float4* gPositionsOld,
      __global float4* gPositionsNew,
      
      
      //let's make the argument list ready for the risky optimization of "deferred densitiy usage" to save the density kernel invocation,
      //memory transfer and control flow overhead;
      //TODO maybe store  representing volume instead of densitiy here to compatc the SPH-term mass/density (mass(mass/volume)) to volume;
      //this would have the drawback of one more division for pressure computation and one more multiplication for acceleration computation
      //and one more division per particle densityx computation;
      //the net win, assumindg that viscosity is computed, would be that viscosity computation would have one less division per SPH computation;
      //for pressure compuatation, pros and cons cancel each other nearly out (cons slightly prevail)
      //--> check rather later, don't obfuscate the application logic too much for now 
      __global float* gDensitiesOld, //read
      __global float* gDensitiesNew, //maybe write when doing "deferred density" optimization; else unused;
      
      __global float4* gCorrectedVelocitiesOld,  //corrected velocity values from the last step read for integration of both
                                                 //the position and the new corrected velocity;
      __global float4* gCorrectedVelocitiesNew,  //velocity values corrected after force calculation with predicted velocities 
                                                 //(correctedVelocityNew = correctedVelocityOld 
                                                 //                        + halfTimeStep * (accelerationOld + accelerationNewWithPredictedVelocity)) 
                                                 //to be written in integration kernel
      
      __global float4* gPredictedVelocitiesCurrent, //velocity values (predicted in the last step) read for viscosity force computation in current step;
      __global float4* gPredictedVelocitiesFuture,  //velocity values predicted after position integration ((posNew-posOld)/timeStep) 
                                                    //to be written in integration kernel for viscosity force computation in next step;
                                                    //The Velocity Verlet integration scheme is implicit for velocity values,
                                                    //hence the strange "double representation" of velocities
      
      //we store accelerations instead of forces, force densities or whatever; this is because of the velocity verlet integration,
      //we have to access each computed force value twice; so save the fetch and division by of the old density value to yield the accerleation,
      //we store the acceleration directly ;).
      __global float4* gAccelerationsOld,
      __global float4* gAccelerationsNew

      
{% endblock particleAttributesBufferKernelArgs %}
    
    
{% block triangleBufferKernelArgs %}
    
      {% if useStaticTriangleCollisionMesh %}
        //__global uint* gUniGridCells_TriangleStartIndex, 
        //__global uint* gUniGridCells_NumTriangles,
        //__global Triangle* gTriangles //struct not defined yet; also have no clue yet how to represent 
                                        //triangle best and do calculations most efficent
      {% endif %}
      
{% endblock triangleBufferKernelArgs %}
  )
  {
    //DEBUG
    //uint debugVariable=0;
    
    //DEBUG
     //__local float4 lCurrentPositions[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ];
     //__local float4 lMeanPosition;
    
  
    {% block getCL_IDs %}
     uint lwiID = get_local_id(0); // short for "local work item ID"
     uint gwiID = get_global_id(0); // short for "global work item ID"
     uint groupID =  get_group_id(0);
    {% endblock getCL_IDs %}
     
    
    uint numParticlesInOwnGroup =  gSimWorkGroups_NumParticles[ groupID ];
    uint ownGlobalAttributeIndex = gSimWorkGroups_ParticleStartIndex[ groupID ] + lwiID;


   
    

     
    //{ alloc local mem for neighbours, grab needed attributes for own particles, store them to private mem 
    
      float4 ownPosition;
      uint ownParticleObjectID;

      {% block particleAttribsMalloc %} 
        {% comment %} pattern:
          for each needed attribute: 
            <attribute type> own<attribute name singular>;
            optional: __local <attribute type>  lCurrentNeighbour<attribute name plural>[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ]; 
        {% endcomment %}
      
      
        {% if not useCacheUsingOpenCLImplementation %}     
          __local float4 lCurrentNeighbourPositions[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ];
          //note: here are really __object__ IDs (not the compound named "object info", consisting ob object ID and particle index within object)
          //stored, i.e. no particle index info is in it;
          __local uint lCurrentNeighbourParticleObjectIDs[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ];
        {% endif %}
        
            
      {% endblock particleAttribsMalloc %}
    
    //}
    
    
      
      
    if(lwiID < numParticlesInOwnGroup )
    {
      {% block particleAttribsInit %} 
     
        ownPosition = gPositionsOld[ ownGlobalAttributeIndex ];
        ownParticleObjectID = GET_OBJECT_ID( gParticleObjectInfos[ ownGlobalAttributeIndex ] );
         
      {% endblock particleAttribsInit %}
    
        
    } //end if(lwiID < numParticlesInOwnGroup )
    //note: no barrier() necessary here;
    
    
    
    //note: I don't know if it is possible to efficiently compute the 3D-neighbour from a z-index;
    //      This is why I compute a 3D grid position from the own particles 3D position, and compute from this
    //      3D neighbour grid position the neighbour's z-Index; TODO research a more direct/ more efficient way;     
    int4 ownGridPos = getGridPos(
        //calc grid pos from first particle in group; this way, even wthe work items with ID >=numParticlesInOwnGroup
        //have a valid state; thisi is important because the for()-lopp should not diverge
        gPositionsOld[ ownGlobalAttributeIndex -lwiID ], 
        cSimParams
    );

     
     

    
    //iterate over all 3^3=27 neigbour voxels, including the own one:

    //#pragma unroll
    for(int z=-1;z<=1;z++)
    {
      //#pragma unroll
      for(int y=-1;y<=1;y++)
      {
        //#pragma unroll   
       for(int x=-1;x<=1;x++)
        {            
        
        
          //get the "modulo" z-index, i.e border cells will interact with the border cells on the opposite side;
          //we accept this performance penalty, as we get en unlimited simulation domain this way, though performance
          //drastically decreases when particle are not all in "one rest group" of the uniform grid, because then,
          //particles land in the same "buckets" which aren't spacially adjacent, hence such calculations are in vain;
          uint neighbourZIndex = getZIndexi( 
            //grid pos of neighbour grid cell:
            ownGridPos + (int4)(x,y,z,0), 
            cSimParams, 
            cGridPosToZIndexLookupTable 
          );
          

          uint numNeighbourParticlesToInteractWith = gUniGridCells_NumParticles[ neighbourZIndex ];


          //TODO uncomment this when stable;
          //if(numNeighbourParticlesToInteractWith == 0) { continue; }
          //----------------------------------------------------------------------------------------------------
          
          uint neighbourParticleStartIndex = gUniGridCells_ParticleStartIndex[ neighbourZIndex ];


{% if not useCacheUsingOpenCLImplementation %}     
                
          uint numNeighbourSimWorkGroups = GET_NUM_SIM_WORK_GROUPS_OF_CELL( numNeighbourParticlesToInteractWith ); 
            
          //sequential work on neighbours simulation work groups
          //the compiler bug should not occur here, because again, there is no work item divergence;
          //TODO check if this is true ;(
          for(uint simGroupRunner=0; simGroupRunner < numNeighbourSimWorkGroups; simGroupRunner++ )
          {
            //debugVariable++;   //<-- indicator for number of neighbour simulation groups  
   
            uint numCurrentNeighbourSimGroupParticles = 
              //is not last simulation group in neighbour  voxel?
              ( simGroupRunner < ( numNeighbourSimWorkGroups - 1 ) )
              //for all but the last group, there are 32 elements in each sim work group
              ? NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP
              : numNeighbourParticlesToInteractWith
              ;

              //grab all neighbours in particle stride
              if(lwiID < numCurrentNeighbourSimGroupParticles)
              {
                {% block kernelDependentNeighbourParticleAttribsDownload %} 
                  {% comment %}
                    pattern:  lCurrentNeighbour<attribute name plural>[ lwiID ] = g<attribute name singular>Old[ neighbourParticleStartIndex + lwiID ]; 
                  {% endcomment %}
                    
                  lCurrentNeighbourPositions[ lwiID ] = gPositionsOld[ neighbourParticleStartIndex + lwiID ];
                  lCurrentNeighbourParticleObjectIDs[ lwiID ] = 
                      GET_OBJECT_ID( gParticleObjectInfos[ neighbourParticleStartIndex + lwiID ] );
                      
                {% endblock kernelDependentNeighbourParticleAttribsDownload %} 
   
              } //end if(lwiID < numCurrentNeighbourSimGroupParticles) 

              barrier(CLK_LOCAL_MEM_FENCE); //all freshly downloaded current neighbours shall be available to any work item


              //accum SPH calculations in sequence for each neigbour particle ...
              //the compiler bug should not occur here, because again, there is no work item divergence;
              for(  uint interactingLocalIndex=0; 
                    interactingLocalIndex < numCurrentNeighbourSimGroupParticles;
                    interactingLocalIndex++ )
              {


              
{% else %}  {% comment %} not useCacheUsingOpenCLImplementation {% endcomment %} 

              
              for(  uint interactingLocalIndex=0; 
                    interactingLocalIndex < numNeighbourParticlesToInteractWith;

                    interactingLocalIndex++ )
              {               
              
{% endif %}  {% comment %} not useCacheUsingOpenCLImplementation {% endcomment %} 

        
                                    
                //for each particle in own simulation group in parallel do...
                if(lwiID < numParticlesInOwnGroup)
                {  
                  //debugVariable++;   //<-- indicator for number of interacted-with neighbour particles       

         
                  {% block performSPHCalculations %}
                  //----------------------------------------------------------------------------------------------------
                    {% comment %}
                      the core of the physics simulation: accumulate all relevant values (density, pressure force, viscosity force etc ...)
                    {% endcomment %}
                    
                  //----------------------------------------------------------------------------------------------------
                  {% endblock performSPHCalculations %}

                } //end if(lwiID < numParticlesInOwnGroup)  
                
{% if not useCacheUsingOpenCLImplementation %}         
          
              }//end accum SPH calculations 
            
            
            
              neighbourParticleStartIndex += NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP;
              numNeighbourParticlesToInteractWith -= NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP;
            
            
              //synch, because not every local particle may have finished its loop over the currently stored neighbours 
              //before new neighbour particles are downloaded to local mem;
              barrier(CLK_LOCAL_MEM_FENCE); 
               
          
            }// end sequential work on one neighbour's simulation work groups
            
            
{% else %}  {% comment %} not useCacheUsingOpenCLImplementation {% endcomment %}             
     

     
            }// end sequential work on one neighbour cell's particles
            
            
{% endif %}  {% comment %} not useCacheUsingOpenCLImplementation {% endcomment %}         
          
          
        }  //end for x
      }  //end for y
    } //end for z

    
    //---------------------------------------------
      
    if(lwiID < numParticlesInOwnGroup )
    {
     
      {% block processSPHResults %}
        {% comment %}
          evalute SPH results: add force densities together compute collision forces with static geometry, trnasform to acceleration, 
          add gravity...
        {% endcomment %}      
      {% endblock processSPHResults %}


      //integrat only for valid particles, rest MUST stay at zero to not corrupt scans in rigid body calculations
      if(! IS_INVALID_PARTICLE(ownParticleObjectID) )
      {
          
        {% block integrate %}
          {% comment %}
            calculate new positions and velocites from new accelerations and old poistions and velocities;
          {% endcomment %}          
        {% endblock integrate %}

        {% block calcZIndex %}
                  
        {% endblock calcZIndex %}
            

        {% block uploadUpdatedParticleAttribs %}
          {% comment %}
            pattern:  g<attribute name plural>New[ lwiID ] = own<attribute name singular>;
          {% endcomment %} 
        {% endblock uploadUpdatedParticleAttribs %}

      } // end if(! IS_INVALID_PARTICLE() )
      
    } //end if(lwiID < numParticlesInOwnGroup )
    
    
  } //end kernel function
  
