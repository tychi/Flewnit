  
  
  {% block documentHeader %}
    /**
      updateDensity.cl;

    */ 
  {% endblock documentHeader %}

  {% block kernelName %}
      kernel_updateDensity
  {% endblock kernelName %}


  {% block particleAttributesBufferKernelArgs %}

    __global float4* gPositionsOld,
    __global float* gDensitiesNew,
    __global uint* gParticleObjectInfos
        
  {% endblock particleAttributesBufferKernelArgs %}


  {% block kernelDependentParticleAttribsMalloc %} 
    {% comment %} pattern:
      for each needed attribute: 
        <attribute type> own<attribute name singular>;
        optional: __local <attribute type>  lCurrentNeighbour<attribute name plural>[ NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP  ]; 
    {% endcomment %}
                  
          float ownDensity;
        
  {% endblock kernelDependentParticleAttribsMalloc %}


  {% block kernelDependentOwnParticleAttribsInit %} 
          
            //init to zero because the SPH calculations accumulate stuff in this variable;
            ownDensity = 0.0f;
       
  {% endblock kernelDependentOwnParticleAttribsInit %}




  {% block kernelDependentNeighbourParticleAttribsDownload %} 
    {% comment %}
      pattern:  lCurrentNeighbour<attribute name plural>[ lwiID ] = g<attribute name singular>Old[ neighbourParticleStartIndex + lwiID ]; 
    {% endcomment %}
                  
    /*empty for density calculations*/
                      
  {% endblock kernelDependentNeighbourParticleAttribsDownload %} 


{% block performSPHCalculations %}
//----------------------------------------------------------------------------------------------------
                      /* 
                        Compute only SPH-density for fluid particles; Imagine a object consisting of Blumb within a Hydrogen cloud:
                        The density of the Hydrogen gas is NOT increased by the proximity of plumb!
                        (This means on the other side, that SPH-computed density DEcreases proprotionally with the surrounding
                        percentage of rigid body volume; Considered on an infinitesimal scale, this doesn't seem physically plausible
                        to me, too; But on the other hand, this "implausible low density" at fluid particles near rigid bodies
                        yields pressure gradients pointing towards the rigid body for fluid particles slightly further away
                        from the rigid body,  )
                      */
                      if( ! IS_RIGID_BODY_PARTICLE( lCurrentNeighbourParticleObjectIDs[ interactingLocalIndex ]  ) )
                      {
                        ownDensity +=
                          //mass 
                          cObjectMassesPerParticle [ lCurrentNeighbourParticleObjectIDs[ interactingLocalIndex ]  ]
                          //* kernel
                          * poly6( ownPosition - lCurrentNeighbourPositions[ interactingLocalIndex ], cSimParams )
                          ;
                      }
                    }
//----------------------------------------------------------------------------------------------------
{% endblock performSPHCalculations %}



    
    
    
    {% block processSPHResults %}
      
      /*empty for density caculations*/
      
    {% endblock processSPHResults %}
     
     
     
    {% block uploadUpdatedParticleAttribs %}
      {% comment %}
        pattern:  g<attribute name plural>New[ lwiID ] = own<attribute name singular>;
      {% endcomment %}
      
      if( ! IS_RIGID_BODY_PARTICLE( ownParticleObjectID  ) )
      {
        gDensitiesNew[ ownGlobalAttributeIndex ] = ownDensity;      
      }
      else
      {
        gDensitiesNew[ ownGlobalAttributeIndex ] =
          cObjectMassesPerParticle[ ownParticleObjectID ]
          //see member definition of the SimulationParameters structure for further info on this value 
          * cSimParams->inverseRigidBodyParticleizationVoxelVolume ;
      }

    {% endblock uploadUpdatedParticleAttribs %}
