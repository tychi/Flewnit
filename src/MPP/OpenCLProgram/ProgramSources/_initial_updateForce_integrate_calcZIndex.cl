  
  {% extends updateForce_integrate_calcZIndex.cl %}
  
  {% block documentHeader %}
    /**
      _initial_updateForce_integrate_calcZIndex.cl
      
      IMPORTANT:  In order to reuse the template with the "default" integration, the initial velocity values
                  shall be bound to the gPredictedVelocitiesCurrent buffer instead of gCorrectedVelocitiesOld;

    */ 
  {% endblock documentHeader %}
  
  
  
  {% block functionDefinitions %} 
  
    //Kernel used only once for initialization;
    //During simulation, the Z-index is calculated within the SPH simulation kernel in order to save one kernel invocation;
    __kernel void kernel_initial_CalcZIndex(
      __constant uint* cGridPosToZIndexLookupTable, //lookup table to save some costly bit operations for z-Index calculation
      __constant SimulationParameters* cSimParams,
      __global float4* gPositions,
      __global uint* gZIndices,     
    )
    {
      gZIndices[get_global_id(0)] = getZIndex(gPositions[get_global_id(0)], cSimParams, cGridPosToZIndexLookupTable);
    }
    
    
    {{ block.super }}
    
  {% endblock functionDefinitions %} 

  
  
  
  
  
  {% block kernelName %}
      kernel_initial_updateForce_integrate_calcZIndex
  {% endblock kernelName %}


  
  
  {% block integrate %}
    {% comment %}
      calculate new positions and velocites from new accelerations and old poistions and velocities;
    {% endcomment %}  
    
    /*
      Velocity Verlet INITIAL Integration:
      
      Note: This "initalization modification" of velocity verlet is my own try; I haven't done reasearch on the web for this yet;
            This way seems reasonable for me; An euler step for the first velocity integration doesn't see too wrong for me ;(.
      
      We are at time t_0; 
      
      What the following passes expect:
        ° r_i           : ownPosition
        ° v_i-1_corr    : ownCorrectedVelOld
        ° v_i_pred      : ownPredictedVelCurrent
        ° a_i-1         : ownAccelerationOld
        
      What we initially have:
        ° r_i           : ownPosition
        ° v_i_pred      : ownPredictedVelCurrent; we just call these initial velocity values like that, although they are initial
                                                  and henc not predicted values
      What we are hence missing:
        ° v_i-1_corr    : ownCorrectedVelOld
        ° a_i-1         : ownAccelerationOld
       
       What we have computed yet in this pass:
        ° a_i           : ownAccelerationNew        ; a_i = accel( t_i, r_i, v_i_pred )
       
       What we will compute in this integrations step:
        ° v_i_corr      : ownCorrectedVelNew        
                        actual formula:       v_i_corr = v_i-1_corr + timestep/2 * (a_i-1 + a_i)
                        initial formula:      v_i_corr = v_i_pred   + timestep   *  a_i
        ° r_i+1         : ownPositionNew            
                        actual formula:       r_i+1 = r_i + v_i_corr * timestep + (timestep^2)/2 * a_i
                        initial formula:      r_i+1 = r_i + v_i_corr * timestep + (timestep^2)/2 * a_i                        
                        --> can stay the same! :)
        ° v_i+1_pred    : ownPredictedVelFuture     
                        actual formula:       v_i+1_pred = ( r_i+1 - r_i ) / timestep;
                        initial formula:      v_i+1_pred = ( r_i+1 - r_i ) / timestep;
                        --> can stay the same! :)
                        
       Conclusion: Although we are missing two values in this initial pass, we only need to alter one of three formulas;
    */
    
    //declare not until here in hope of register re-usage by compiler to take register pressure from the work group 
    //to increase occupancy; This way, the scheduler can toggle between more warps to hide memorya latency;
    float4 ownCorrectedVelOld = gCorrectedVelocitiesOld[ ownGlobalAttributeIndex ];
    float4 ownAccelerationOld = gAccelerationsOld[ ownGlobalAttributeIndex ];  

    //---------------------------------------------------------------------------------------------    
    float4 ownCorrectedVelNew = 
      //################ single modified executable code in this template file ######
      /*default formula*/      
        //ownCorrectedVelOld 
        //+ 0.5f * cSimParams->timestep * ( ownAccelerationOld + ownAccelerationNew );
      /*initial pass formula*/
      ownPredictedVelCurrent
      + cSimParams->timestep * ownAccelerationNew;
      //#############################################################################
    
    float4 ownPositionNew =  
      ownPosition  
      + cSimParams->timestep * ownCorrectedVelNew
      + ( 0.5f * cSimParams->squaredTimestep ) * ownAccelerationNew;
      
    float4 ownPredictedVelFuture = ( ownPositionNew - ownPosition ) * cSimParams->inverseTimestep;
    //---------------------------------------------------------------------------------------------
      
  {% endblock integrate %}



  
