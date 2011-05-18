  
  {% extends "particleSimulationTemplate.cl" %}
  
  {% block documentHeader %}
    /**
      updateForce_integrate_calcZIndex.cl

    */ 
  {% endblock documentHeader %}
  
  
  {% block functionDefinitions %} 
    
    float getPressure( float density, __constant ObjectGenericFeatures* cObjectGenericFeatures, uint objectID)
    {
      return cObjectGenericFeatures[ objectID ].gasConstant * (density - cObjectGenericFeatures[ objectID ].restDensity);
    }
    
    #define GET_PRESSURE( density, cSimParams, objectID )  \
     ( cObjectGenericFeatures[ objectID ].gasConstant * (density - cObjectGenericFeatures[ objectID ].restDensity) )
     
     
   float4 staticGeometryCollisionAcceleration(
      __constant SimulationParameters* cSimParams, 
      float4 particlePosition, 
      float4 particleVelocity,
      float particleMass)
   {        
     float4 collisionForce = (float4)(0.0f,0.0f,0.0f,0.0f);
         
     //i just don't know how to reduce the boilerplate factor here without wasting memory and control flow;
     //it's anyway just a hack until i have static triangle collision meshes;


    float particleRadius =  1.0f;


     if( (particlePosition.x - particleRadius ) < cSimParams->simulationDomainBorders.minPos.x){
          collisionForce.x += 
            ( cSimParams->simulationDomainBorders.minPos.x - (particlePosition.x - particleRadius ) ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.x;
      }
      


      if( (particlePosition.x + particleRadius ) > cSimParams->simulationDomainBorders.maxPos.x){
          collisionForce.x += 
            ( cSimParams->simulationDomainBorders.maxPos.x - (particlePosition.x + particleRadius )  ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.x;
      }

      if( (particlePosition.y - particleRadius ) < cSimParams->simulationDomainBorders.minPos.y){
          collisionForce.y += 
           ( cSimParams->simulationDomainBorders.minPos.y -  (particlePosition.y - particleRadius ) ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.y;
      }
      if(  (particlePosition.y + particleRadius ) > cSimParams->simulationDomainBorders.maxPos.y){
          collisionForce.y += 
             ( cSimParams->simulationDomainBorders.maxPos.y - (particlePosition.y + particleRadius )  ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.y;
      }

      if(  (particlePosition.z - particleRadius ) < cSimParams->simulationDomainBorders.minPos.z){
          collisionForce.z += 
            ( cSimParams->simulationDomainBorders.minPos.z - (particlePosition.z - particleRadius ) ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.z;
      }
      if( (particlePosition.z + particleRadius ) > cSimParams->simulationDomainBorders.maxPos.z){
          collisionForce.z += 
            ( cSimParams->simulationDomainBorders.maxPos.z - (particlePosition.z + particleRadius ) ) 
            * cSimParams->penaltyForceSpringConstant      
            -   //MINUS, we wanna add a damping force in the opposite direction of the current vel
            cSimParams->penaltyForceDamperConstant * particleVelocity.z;
      }





      return collisionForce / particleMass ; //TODO native..
      
      /* some code contataining the general, vectorized formula needed for generic primitives
        float distance = cSimParams->simulationDomainBorders.min.x - particlePosition.x;
       //negative x
       if(distance < 0.0f)
       {
         float4 normal = (float4)(1.0f,0.0f,0.0f,0.0f);
          collisionForceDensity += 
             (
               ( cSimParams->penaltyForceSpringConstant * distance )
               //MINUS, we wanna add a damping force in the opposite direction of the current vel;
               //this is wrong on page 33 of thomas steil's DA: we shall
               //not pruduce force vectors pointing INTO the collision primitive
               -
               ( cSimParams->penaltyForceDamperConstant * dot(ownPredictedVelCurrent, normal ) )
             ) 
             * normal ;
        }
      */
   }
    
  {% endblock functionDefinitions %} 
  
  
  

  {% block kernelName %}
      kernel_updateForce_integrate_calcZIndex
  {% endblock kernelName %}




  {% block particleAttribsMalloc %} 
  
    {{ block.super }}
                  
    float ownDensity;
    float ownPressure;
    float4 ownPredictedVelCurrent;
    
    //physical quantities resulting from sph "force" computation;
    //physical units: [ (kg m/(s^2)) / V]= force/volume;
    //to get the acceleration from this value: 
    //    forceDensity = m*a/V 
    //--> a = forceDensity * V/m
    //--> a = forceDensity / (m/V)
    //--> a = forceDensity / density   
    float4 ownPressureForceDensityNew; 
    float4 ownViscosityForceDensityNew;
 
    {% if not useCacheUsingOpenCLImplementation %}    
      __local float lCurrentNeighbourDensities[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ];
      __local float4 lCurrentNeighbourPredictedVelsCurrent[ NUM_MAX_ELEMENTS_PER_SIMULATION_WORK_GROUP  ];
    {% endif %}    
        
  {% endblock particleAttribsMalloc %}



  {% block particleAttribsInit %} 
  
    {{ block.super }}
          
    ownDensity = gDensitiesOld[ ownGlobalAttributeIndex ];
    ownPressure = GET_PRESSURE( ownDensity, cObjectGenericFeatures, ownParticleObjectID );
    
    ownPredictedVelCurrent = gPredictedVelocitiesCurrent[ ownGlobalAttributeIndex ];
   
    //accumulator variable; init to zero
    ownPressureForceDensityNew= (float4)(0.0f,0.0f,0.0f,0.0f);
    ownViscosityForceDensityNew= (float4)(0.0f,0.0f,0.0f,0.0f);
    
  {% endblock particleAttribsInit %}




  {% block kernelDependentNeighbourParticleAttribsDownload %} 
    
    {{ block.super }}
     
    lCurrentNeighbourDensities[ lwiID ] = gDensitiesOld[ neighbourParticleStartIndex + lwiID ]; 
    lCurrentNeighbourPredictedVelsCurrent[ lwiID ] =  gPredictedVelocitiesCurrent[ neighbourParticleStartIndex + lwiID ]; 

                      
  {% endblock kernelDependentNeighbourParticleAttribsDownload %} 


  {% block performSPHCalculations %}
  //----------------------------------------------------------------------------------------------------
                  // 
                  //  Pressure force
                  //
                                           //MINUS!!     
                  //ownPressureForceDensityNew.xyz -=
                  ownPressureForceDensityNew.xyz +=
                      (
                        //mass 
                        cObjectGenericFeatures [ GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID  ].massPerParticle
                        //mean of both pressures for symmetry reasons:
                        * 0.5f  
                        * (
                            ownPressure + 
                            GET_PRESSURE( 
                              GET_CURRENT_NEIGHBOUR_PARTICLE_DENSITY,
                              cObjectGenericFeatures, 
                               GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID
                            ) 
                         )
                        / GET_CURRENT_NEIGHBOUR_PARTICLE_DENSITY //TODO native_divide( ) or * native_recip() or so ;)
                      )
                      //kernel
                      * gradSpiky( ownPosition - GET_CURRENT_NEIGHBOUR_POS, cSimParams ).xyz
                      ;
                      

                  // 
                  //  Viscosity force 
                  //


                  ownViscosityForceDensityNew.xyz += 
                      //mass 
                      cObjectGenericFeatures [ GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID ].massPerParticle
                      //relative velocity of currently treated neighbour particle
                      * ( GET_CURRENT_NEIGHBOUR_PARTICLE_PREDICTED_VEL_CURRENT.xyz - ownPredictedVelCurrent.xyz )                  
                      / GET_CURRENT_NEIGHBOUR_PARTICLE_DENSITY //TODO native_divide( ) or * native_recip() or so ;)
                      //kernel
                      * laplacianViscosity( ownPosition - GET_CURRENT_NEIGHBOUR_POS, cSimParams ) 
                      ;    

                 
                      
                  // 
                  //   TODO maybe later Surface tension force
                  //

  //----------------------------------------------------------------------------------------------------
  {% endblock performSPHCalculations %}


    
  {% block processSPHResults %}
  
      //don't forget to scale the viscosity term by the own viscosity value!
      ownViscosityForceDensityNew.xyz *= cObjectGenericFeatures[ ownParticleObjectID ].viscosity;

      /*collision test with static geometry and reaction*/
      float4 staticGeomCollisionAcceleration = 
        staticGeometryCollisionAcceleration(
          cSimParams, 
          ownPosition, 
          ownPredictedVelCurrent,
          cObjectGenericFeatures[ ownParticleObjectID ].massPerParticle );


      float accellRange = 0.8f;
      float4 ownAccelerationNew =  
        clamp
	(
          //force/Volume/(mass/Volume) yields acceleration
          ( (ownPressureForceDensityNew + ownViscosityForceDensityNew ) / ownDensity ) //TODO native_divide or precompute inverse;
          //add gravity term, is already an acceleration
          + cSimParams->gravityAcceleration
	  //+ staticGeomCollisionAcceleration

	  ,
	  (float4)(-accellRange,-accellRange,-accellRange,0.0f),
	  (float4)(accellRange,accellRange,accellRange,0.0f)
        )
        + staticGeomCollisionAcceleration
        ;
        
      

      //calculate and add user force
      //TODO unroll when stable and interface is less subject to change;
      for(uint controlPointRunner =0; controlPointRunner < cSimParams->numUserForceControlPoints; controlPointRunner++ )
      {
        //vector from force origin to particle;
        float4 distvec = ownPosition - cUserForceControlPoints[ controlPointRunner  ].forceOriginWorldPos; 
        float squaredLenDistVec =  SQUARED_LENGTH( distvec ); 
        float inverseLenDistVec = 1.0f / sqrt( squaredLenDistVec ); //TODO float inverseLenDistVec = native_rsqrt(squaredLenDistVec); 
        
        if( 
          //is particle near enough to receive the user force?
          ( squaredLenDistVec <= cUserForceControlPoints[ controlPointRunner ].squaredInfluenceRadius )
          //mask nullvector
          && (inverseLenDistVec > EPSILON)
         )
         {
          ownAccelerationNew.xyz += 
              distvec.xyz
              * 
              ( inverseLenDistVec //normalize
                //scale by user force intensity
                * cUserForceControlPoints[ controlPointRunner ].intensity
                //force shall increase quadratically with decreasing distance within [0..intensity] 
                //(1 - (length(distVec)/ influenceRadius)^2 ), rewritten to be faster ;(
                * ( 1.0f - ( squaredLenDistVec *  cUserForceControlPoints[ controlPointRunner ].inverseSquaredInfluenceRadius ) )
              );
         }
      }

      
  {% endblock processSPHResults %}
  
  
  {% block integrate %}
    {% comment %}
      calculate new positions and velocites from new accelerations and old poistions and velocities;
    {% endcomment %}  
    
    /*
      Velocity Verlet Integration : see "Game Physics" by  David H. Eberly, page 485
      
      We are at time t_i in this pass;
      
      What we have read form the previous pass: 
        ° r_i           : ownPosition
        ° v_i-1_corr    : ownCorrectedVelOld
        ° v_i_pred      : ownPredictedVelCurrent
        ° a_i-1         : ownAccelerationOld
       
       What we have computed yet in this pass:
        ° a_i           : ownAccelerationNew        ; a_i = accel( t_i, r_i, v_i_pred )
       
       What we will compute in thie integrations step:
        ° v_i_corr      : ownCorrectedVelNew        : v_i_corr = v_i-1_corr + timestep/2 * (a_i-1 + a_i)
        ° r_i+1         : ownPositionNew            ; r_i+1 = r_i + v_i_corr * timestep + (timestep^2)/2 * a_i
        ° v_i+1_pred    : ownPredictedVelFuture     ; v_i+1_pred = ( r_i+1 - r_i ) / timestep;
        
    */
    
    //declare not until here in hope of register re-usage by compiler to take register pressure from the work group 
    //to increase occupancy; This way, the scheduler can toggle between more warps to hide memorya latency;
    float4 ownCorrectedVelOld = gCorrectedVelocitiesOld[ ownGlobalAttributeIndex ];
    float4 ownAccelerationOld = gAccelerationsOld[ ownGlobalAttributeIndex ];  

    //--------------------------------------------------------------------------------------------- 
    if(any(isnan(ownAccelerationNew)) || any(isinf(ownAccelerationNew)) ) {ownAccelerationNew =  (float4)(0.0f,0.0f,0.0f,0.0f); }
   
    float4 ownCorrectedVelNew = 
      ownCorrectedVelOld 
      + 0.5f * cSimParams->timestep * ( ownAccelerationOld + ownAccelerationNew );

    
    if(any(isnan(ownCorrectedVelNew)) || any(isinf(ownCorrectedVelNew)) ) {ownCorrectedVelNew =  (float4)(0.0f,0.0f,0.0f,0.0f); }


    
    float4 ownPositionNew =  
      ownPosition  
      + cSimParams->timestep * ownCorrectedVelNew
      + ( 0.5f * cSimParams->squaredTimestep ) * ownAccelerationNew;

    //debug
    //if(any(isnan(ownPositionNew)) || any(isinf(ownPositionNew))) {ownPositionNew =  (float4)(1.0f,40.0f,1.0f,0.0f); }
      
    float4 ownPredictedVelFuture = ( ownPositionNew - ownPosition ) * cSimParams->inverseTimestep;

    //debug
    if( any(isnan(ownPredictedVelFuture)) || any(isinf(ownPredictedVelFuture)) ) {ownPredictedVelFuture =  (float4)(0.0f,0.0f,0.0f,0.0f); }
    //---------------------------------------------------------------------------------------------
      
  {% endblock integrate %}


  {% block calcZIndex %}  
    //TODO check if an "if(BELONGS_TO_FLUID(particleObjectID))" performs better or worse;
    uint zIndexNew = getZIndexf(ownPositionNew, cSimParams, cGridPosToZIndexLookupTable);
  {% endblock calcZIndex %}
     
     
     
  {% block uploadUpdatedParticleAttribs %}
    {% comment %}
      pattern:  g<attribute name plural>New[ lwiID ] = own<attribute name singular>;
    {% endcomment %}
    
      gPositionsNew[ ownGlobalAttributeIndex ] = ownPositionNew;
      gZindicesNew[ ownGlobalAttributeIndex ] = zIndexNew;
      gCorrectedVelocitiesNew[ ownGlobalAttributeIndex ] = ownCorrectedVelNew;
      gPredictedVelocitiesFuture[ ownGlobalAttributeIndex ] = ownPredictedVelFuture;
      gAccelerationsNew[ ownGlobalAttributeIndex ] = ownAccelerationNew;

  {% endblock uploadUpdatedParticleAttribs %}
  
  
