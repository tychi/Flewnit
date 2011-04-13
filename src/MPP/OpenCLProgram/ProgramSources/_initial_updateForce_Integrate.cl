

{% extends particleSimulationTemplate.cl %}


  {% block documentHeader %}
    /**
      _initial_updateForce_Integrate.cl

    */ 
  {% endblock documentHeader %}


  {% block functionDefinitions %} 

  {% endblock functionDefinitions %} 


  {% block kernelName %}
    kernel_initial_updateForce_Integrate
  {% endblock kernelName %}
  
  
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
  
  
  
  {% block performSPHCalculations %}
                
  
    //grab all neighbours in particle stride
    if(lwiID < numRemainingNeighbourParticlesToInteract)
    {
      lCurrentNeighbourPositions[ lwiID ] = gPositionsOld[ neighbourParticleStartIndex + lwiID ];
      lCurrentNeighbourParticleObjectInfos[ lwiID ] = gParticleObjectInfos[ neighbourParticleStartIndex + lwiID ];
                    
                    
                    TODO CONTINUE
                  
    }//end if(lwiID < numRemainingNeighbourParticlesToInteract)  
    barrier(CLK_LOCAL_MEM_FENCE);
                  
    //for each particle in own simulation group in parallel do...
    if(lwiID < numParticlesInOwnGroup)
    {    
      //accum SPH calculations
      for(uint interactingLocalIndex=0; interactingLocalIndex < numRemainingNeighbourParticlesToInteract; interactingLocalIndex++ )
      {
                       TODO CONTINUE
      }
    }
              
  {% endblock performSPHCalculations %}
    
  
  
  {% block processSPHResults %}
  /*initial integrate for t=0*/ 
    
    
      TODO CONTINUE
    siehe handschriftliche notizen

  
      
      
  {% endblock processSPHResults %}    
    
    
    
  {% block uploadupdatedParticleAttribs %}
      
      TODO CONTINUE
  
  {% endblock uploadupdatedParticleAttribs %}
    
    
