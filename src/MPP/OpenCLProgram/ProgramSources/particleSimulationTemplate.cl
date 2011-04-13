
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


{% block extensionDirectives %}
  #pragma OPENCL EXTENSION cl_nv_pragma_unroll : enable
  #pragma  OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
  #pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
  //pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
{% endblock extensionDirectives %}


{% block inclusions %}
  {% include physicsDataStructures.cl %}
  {% include physicsCommonFunctions.cl %}
{% endblock inclusions %}


  {% block functionDefinitions %} 

  {% endblock functionDefinitions %} 
  
  
  __kernel 
  void 
  {% block kernelName %}
    kernel_updateDensity
  {% endblock kernelName %}
  (
  
    {% block constantBufferKernelArgs %} {%comment%}/*usually the same for all physics kernels*/{%endcomment%}
      
      __constant SimulationParameters* cSimParams,
      __constant uint* gridPosToZIndexLookupTable, //lookup table to save some costly bit operations for z-Index calculation
      //RIGID_BODY_OFFSET + numRigidBodies elements; contains masses of the fluid particles, then the masses of the respective
      //rigid bodies; this way, we can grab the masses without any branching :).      
      __constant float* cObjectMassesPerParticle, 
      
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
      
      __global float4* gPositionsOld,
      __global float4* gPositionsNew,
      
      //let's make the argument list ready for the risky optimization of "deferred densitiy usage" to save the density kernel invocation,
      //memory transfer and control flow overhead;
      __global float* gDensitiesOld,
      __global float* gDensitiesNew,
      
      __global float4* gVelocitiesOld,
      __global float4* gVelocitiesNew,    
      
      //we store accelerations instead of forces, force densities or whatever; this is because of the velocity verlet integration,
      //we have to access each computed force value twice; so save the fetch and division by of the old density value to yield the accerleation,
      //we store the acceleration directly ;).
      __global float4* gAccelerationsOld,
      __global float4* gAccelerationsNew,

      __global uint* gParticleObjectInfos
      
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
  
  
    {% block getCL_IDs %}
     uint lwiID = get_local_id(0); // short for "local work item ID"
     uint gwiID = get_global_id(0); // short for "global work item ID"
     uint groupID =  get_group_id(0);
    {% endblock getCL_IDs %}
  
  
    uint numParticlesInOwnGroup = gUniGridCells_NumParticles[ groupID ];
    uint ownGlobalAttributeIndex = gUniGridCells_ParticleStartIndex[ groupID ] + lwiID;

    {% block initOwnParticleAttribs %} 
      /*alloc local mem for neighbrours, grab needed attributes for own particles, store them to private mem*/
      
      __local float4 lCurrentNeighbourPositions[ NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP  ];
      __local uint lCurrentNeighbourParticleObjectInfos[ NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP  ];
      
      float4 ownPosition;
      float ownDensity;
      
      if(lwiID < numParticlesInOwnGroup )
      {
        ownPosition = gPositionsOld[ ownGlobalAttributeIndex ];
        ownDensity = 0.0f;
      } 
      
    {% endblock initOwnParticleAttribs %}
     
     
      float4 posIneighbour = ownPosition - cSimParams->uniGridCellSizes;
      #pragma unroll
      for(int x=-1;x<=1;x++)
      {
        neigbourPointer.x += cSimParams->uniGridCellSizes.x;
        #pragma unroll
        for(int y=-1;y<=1;y++)
        {
          neigbourPointer.y += cSimParams->uniGridCellSizes.y;
          #pragma unroll
          for(int z=-1;z<=1;z++)
          {
            neigbourPointer.z += cSimParams->uniGridCellSizes.z;
            
            //get the "modulo" z-index, i.e border cells will interact with the border cells on the opposite side;
            //we accept this performance penalty, as we get en unlimited simulation domain this way, though performance
            //drastically decreases when particle are not all in "one rest group" of the uniform grid, because then,
            //particles land in the same "buckets" which aren't spacially adjacent, hence such calculations are in vain;
            uint neighbourZIndex = getZIndex( posIneighbour, cSimParams, cGridPosToZIndexLookupTable );
            uint numRemainingNeighbourParticlesToInteract = gUniGridCells_NumParticles[ neighbourZIndex ];
            
            if(numRemainingNeighbourParticlesToInteract > 0)
            {
              uint neighbourParticleStartIndex = gUniGridCells_ParticleStartIndex[ neighbourZIndex ];
              uint numNeighbourSimWorkGroups = GET_NUM_SIM_WORK_GROUPS_OF_CELL( numRemainingNeighbourParticlesToInteract ); 
              for(uint simGroupRunner=0; simGroupRunner < numNeighbourSimWorkGroups; simGroupRunner++ )
              {       
     
     
                {% block performSPHCalculations %}
                
                  //grab all neighbours in particle stride
                  if(lwiID < numRemainingNeighbourParticlesToInteract)
                  {
                    lCurrentNeighbourPositions[ lwiID ] = gPositionsOld[ neighbourParticleStartIndex + lwiID ];
                    lCurrentNeighbourParticleObjectInfos[ lwiID ] = gParticleObjectInfos[ neighbourParticleStartIndex + lwiID ];
                  }//end if(lwiID < numRemainingNeighbourParticlesToInteract)  
                  barrier(CLK_LOCAL_MEM_FENCE);
                  
                  //for each particle in own simulation group in parallel do...
                  if(lwiID < numParticlesInOwnGroup)
                  {    
                    //accum SPH calculations
                    for(uint interactingLocalIndex=0; interactingLocalIndex < numRemainingNeighbourParticlesToInteract; interactingLocalIndex++ )
                    {
                      
                    }
                  }
               
                {% endblock performSPHCalculations %}
     
     
                neighbourParticleStartIndex += NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP;
                numRemainingNeighbourParticlesToInteract -= NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP
              }  //end for simGroupRunner     
            }//endif(numRemainingNeighbourParticlesToInteract >0)
          }  //end for z
        }  //end for y
      } //end for z

     
     {% block processSPHResults %}
      
      TODO CONTINUE
      
     {% endblock processSPHResults %}
     
     
     {% block uploadUpdatedParticleAttribs %}
      
      TODO CONTINUE
      
     {% endblock uploadUpdatedParticleAttribs %}
  
  }
  
