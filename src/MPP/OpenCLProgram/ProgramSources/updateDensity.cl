  
  {% extends "particleSimulationTemplate.cl" %}
  
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


  {% block particleAttribsMalloc %} 
    
    {{ block.super }}
                  
    float ownDensity;
        
  {% endblock particleAttribsMalloc %}


  {% block particleAttribsInit %} 
  
    {{ block.super }}
          
    //init to zero because the SPH calculations accumulate stuff in this variable;
    //ownDensity = 1.0f;
    //ownDensity = 0.003f;
    ownDensity = 0.0f;
    
  {% endblock particleAttribsInit %}




  {% block performSPHCalculations %}
  //----------------------------------------------------------------------------------------------------
		      {% if  useCacheUsingOpenCLImplementation %}
 			float4 currentNeighbourPosition = gPositionsOld[ neighbourParticleStartIndex + interactingLocalIndex ];
		        uint currentNeighbourParticleObjectID = GET_OBJECT_ID( gParticleObjectInfos[ neighbourParticleStartIndex + interactingLocalIndex ] ) ;          

		      {% endif %}


                      // 
                      //  Compute only SPH-density for fluid particles; Imagine a object consisting of Blumb within a Hydrogen cloud:
                      //  The density of the Hydrogen gas is NOT increased by the proximity of plumb!
                      //  (This means on the other side, that SPH-computed density DEcreases proprotionally with the surrounding
                      //  percentage of rigid body volume; Considered on an infinitesimal scale, this doesn't seem physically plausible
                      //  to me, too; But on the other hand, this "implausible low density" at fluid particles near rigid bodies
                      //  yields pressure gradients pointing towards the rigid body for fluid particles slightly further away
                      //  from the rigid body,  )
                      //
                      if( BELONGS_TO_FLUID( GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID  ) )
                      {

                        ownDensity +=
                          //mass 
                          cObjectGenericFeatures [ GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID  ].massPerParticle
                          //* kernel
                          * poly6( ownPosition -  GET_CURRENT_NEIGHBOUR_POS , cSimParams )
                          ;

                          
                      //DEBUG
//if( (isnan(ownDensity)) ||  (isinf(ownDensity)) ) {ownDensity = cObjectGenericFeatures[ ownParticleObjectID ].restDensity  ; }
                      // wtf, machine frezes and program crashes when uncommenting this... VERY strange; 
                      //   EDIT: not, after all... don't know what fixed this, too^^; but i have a suggestion: 
                      //   there was a false loop condition corrupting mem acceses; because of debug stuff, 
                      //   the inner loop didnt contribute to global memory written values and hence was 
                      //   optimized out by the compiler, this way not revealing the programmingf error;
                      //   when incrementing the debugg variable, the loop body wasn't optimized out and so the error was generated;
                      //
                       //debugVariable++;   //<-- indicator for number of interacted-with neighbour particles   

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
      
    if( IS_FLUID_PARTICLE( ownParticleObjectID  ) )
    {
      gDensitiesNew[ ownGlobalAttributeIndex ] = ownDensity;//max(0.5f, ownDensity);   
      
      //gDensitiesNew[ ownGlobalAttributeIndex ] = 
        //cSimParams->SPHsupportRadiusSquared;
        //cSimParams->poly6KernelConstantTerm;
       // (float) ( debugVariable );
      
      //int4 signbits = signbit((float4)(1.0f,-1.0f,-1.0f,1.0f));
      
     //gDensitiesNew[ ownGlobalAttributeIndex ] =
       //getZIndex(  ownPosition - cSimParams->uniGridCellSizes , cSimParams, cGridPosToZIndexLookupTable );
       //getZIndex( (float4)(-1.0f,-1.0f,-1.0f,1.0f) , cSimParams, cGridPosToZIndexLookupTable );
       //getZIndex( (float4)(1.0f,1.0f,1.0f,1.0f) , cSimParams, cGridPosToZIndexLookupTable );
       //fmod(-1.0f,64.0f);
       //(float)( (int)(-1) % (int)(NUM_UNIGRID_CELLS_PER_DIMENSION ) ); //<-- wtf -1! should be NUM_UNIGRID_CELLS_PER_DIMENSION-1 !! fffuu
       //(float)(signbit(1.0f)); //<-- returns int 0
       //(float)(signbit(-1.0f)); //<-- returns int 1
       //signbits.x; //<-- returns int  0 if signbits = signbit((float4)( 1.0f,1.0f,1.0f,1.0f));
       //signbits.x; //<-- returns int -1 if signbits = signbit((float4)(-1.0f,-1.0f,-1.0f,1.0f)); wtf, another spec for vecs than for scalars fffuu
      
    }
    else
    {
      //must be rigid body  or invalid particle;
      gDensitiesNew[ ownGlobalAttributeIndex ] =  cObjectGenericFeatures[ ownParticleObjectID ].restDensity;
      //see member definition of the SimulationParameters structure for further info on this value 
      //* cSimParams->inverseRigidBodyParticleizationVoxelVolume ;
      
      //gDensitiesNew[ ownGlobalAttributeIndex ] = 666.0f;
    }
    

  {% endblock uploadUpdatedParticleAttribs %}
  
  
