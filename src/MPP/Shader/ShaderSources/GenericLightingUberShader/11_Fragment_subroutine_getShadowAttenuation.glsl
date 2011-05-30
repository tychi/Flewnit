{%comment%}
  GLSL Shader Template: shadow attenuation calculations:
  applicable to following stages: fragment       
  
  notes:  no check for compatibility to lighting features here to save complexitiy;
          exception: point light, becaus point light shadows with spot light lighting would be too stupid         {%endcomment%} 

{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}

float getShadowAttenuation(float shadowMapLayer, vec3 fragPos)
{
  {% if SHADOW_TECHNIQUE_NONE or LIGHT_SOURCES_SHADOW_FEATURE_NONE  %}
    return 1.0;
  {% endif %}
  
  {%comment%} ~~~~~~~~~~~ default shadowing technique ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {%endcomment%} 
  {% if SHADOW_TECHNIQUE_DEFAULT  %}
  
    if(   (shadowMapLayer < 0.0)                                          //indicator that the light source is no shadow caster
      ||  (shadowMapLayer >= numCurrentlyActiveShadowCasters ) )  //indicator that the light source is no (at least currently active) shadow caster
    { return 1.0; } // layer must be zero, else it is an indicator that the lightsource is not a shadow caster;
  
    {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS  %}
      // ---------- spotlight calculations ----------------------------------------------------------------------
      {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
        {% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER %}   
          vec4 shadowCoord= input.shadowCoord; 
        {% else %}  
          vec4 shadowCoord =  shadowMapLookupMatrix * input.position;
          
          shadowCoord /= shadowCoord.w; //divide by homogene coord:

          //shadowCoord = (shadowCoord + 1.0) *0.5 ;                
        {%endif%}  
      {% endif %}

      {% if LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS %}
        vec4 shadowCoord = shadowMapLookupMatrices[(int)(floor(shadowMapLayer+0.5))] * input.position;
        shadowCoord /= shadowCoord.w; //divide by homogene coord:
        //rearrange shadow coord for array-shadowmap lookup:    
        shadowCoord =   vec4(  shadowCoord.x,shadowCoord.y, 
              //hope that I understood the spec correctly, that there is no [0..1] scaling of the layer like in texture2D:
              //max 0,min d −1, floorlayer0.5
              shadowMapLayer,
              shadowCoord.z   ) ;
      {% endif %}
  
      //we don't want a squared shadow-throwing lightsource impression, but a circled one:  
      float shadowCoordVecDistFromCenter = length(vec2( shadowCoord.x -0.5, shadowCoord.y -0.5));
      if( shadowCoordVecDistFromCenter < 0.5 )
      {
        {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %} {%comment%} distinguish between vec3 and vec4 lookup ;( {%endcomment%}
          return  
          clamp(
            texture( shadowMap, shadowCoord.xyz ) - (4.0 * shadowCoordVecDistFromCenter * shadowCoordVecDistFromCenter  ),
             0.0 , 1.0
             );
            //texture( shadowMap, shadowCoord.xyz ) //clamp(texture( shadowMap, shadowCoord.xyz ), minimalshadowAttenuation , 1.0)
                  
          ;  
        {% else %} 
          return clamp(texture( shadowMap, shadowCoord ),  0.0 , 1.0);   
        {% endif %}       
      }
      else 
      {
        return 0.0;
        //return minimalshadowAttenuation;
      }
    {% endif %}  {%comment%} endif spotlight calculations {%endcomment%}

    
    {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT %}
    //---------- pointlight calculations -----------------------------------------------------------------------
      {% if LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS %}
        //assert that the LIGHTING is also pointlight, otherwise the shading logic would be TO unrealistic
        WHAT WERE YOU THINKING!? ;)  //provoking compiler error on purpose :P                                   
      {% endif %}

      {%if worldSpaceTransform %}
        vec3 lightToModel = fragPos - lightSource.position;    
        float comparisonReferenceValue = 
            (   
              length(lightToModel) 
              * invLightSourceFarClipPlane  //scale to compensate for the depthbuffer [0..1] clamping  
            )
            + shadowMapComparisonOffset;
      {% else %}
        //remember: viewToPointShadowMapMatrix=  invLightSourceFarClipPlane * inversepointLightTranslation * (camView)⁻1
        vec3 lightToModel = viewToPointLightShadowMapMatrix * fragPos;
        float comparisonReferenceValue = length(lightToModel) + shadowMapComparisonOffset;  
      {% endif %}  
      
      return texture(shadowMap, vec4(lightToModel.xyz,comparisonReferenceValue )); 
       
    {% endif %}  {%comment%} endif pointlight calculations {%endcomment%}

  {% endif %}  {%comment%} endif SHADOW_TECHNIQUE_DEFAULT {%endcomment%}

  {%comment%} ~~~~~~~~~~~ PCFSS shadowing technique ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {%endcomment%} 

  {% if SHADOW_TECHNIQUE_PCFSS  %}
      not yet  implemented :P
  {% endif %}  {%comment%} if SHADOW_TECHNIQUE_PCFSS {%endcomment%}
  
}
{% endif %}  {%comment%} endif RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_LIGHTING {%endcomment%}
