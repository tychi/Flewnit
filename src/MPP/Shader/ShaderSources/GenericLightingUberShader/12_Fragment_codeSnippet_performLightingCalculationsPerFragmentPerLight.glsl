 {%comment%}
  GLSL Shader Template: Fragment shader code snippet: do th lighting calculations for one given fragment and one given lightsource
  applicable to following stages: fragment     {%endcomment%} 
  
                              vec3 lightToFragV =   position.xyz - lightSource.position.xyz;
                              vec3 lightToFragVN =  normalize(lightToFragV);

                              float cosFragToLight_Normal = dot( (-1.0) * lightToFragVN , normalVN);



                              //is fragment facing to the light?
                              if(cosFragToLight_Normal > 0.0)
                              {  
                                //{%codeFragment_diffuseLightingCalc}
                                //{    
                                float diffuseFactor  = cosFragToLight_Normal;
                                //} //end codeFragment_diffuseLightingCalc; that was a big one, wasn't it ? :D

                                //{codeFragment_specularLightingCalc}
                                //{
                                vec3 reflectedlightToFragV = reflect ( lightToFragVN , normalVN);
                                float cosFragToCam_reflectedlightToFragV = dot(fragToCamN,reflectedlightToFragV);
                               
                                float specularFactor=0.0;
                                if(cosFragToCam_reflectedlightToFragV > 0.0)
                                {
                                  specularFactor = pow(cosFragToCam_reflectedlightToFragV, shininess);
                                }
                                //float specularFactor = clamp (pow(cosFragToCam_reflectedlightToFragV, shininess), 0.0,1.0 );
                                //} //end codeFragment_specularLightingCalc
                             
                            
                                incidentLight +=
                                  getDistanceAttenuation(lightToFragV) 
                                 
                                 * 
                                  getShadowAttenuation(lightSource.shadowMapLayer, position.xyz) 
                                  * 
                                  {% if LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS %}
                                    {% if LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS %}
                                      //is the lightsource a spotlight?
                                      (
                                        (lightSource.innerSpotCutOff_Radians <= 0.0) //if cutoff <=0.0, it will be interpreted as point light
                                        ? 1.0 
                                        : 
                                    {% endif %}
                                    getSpotLightAttenuation(  lightSource.direction.xyz,      
                                            lightSource.spotExponent, 
                                            lightSource.innerSpotCutOff_Radians, 
                                            lightSource.outerSpotCutOff_Radians, 
                                            lightToFragVN        )
                                       {% if LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS %}     
                                       )
                                       {% endif %} 
                                    *
                                  {% endif %}
                                  {% if SHADING_FEATURE_AMBIENT_OCCLUSION %} getAOAttenuation() * {% endif %}
                                  (
                                    ( vec4(lightSource.diffuseColor.xyz,1.0) *  diffuseFactor ) +
                                    ( vec4(lightSource.specularColor.xyz,1.0) *  specularFactor * reflectivity )
                                  );
        
                              } //endif is fragment facing to the light (cosFragToLight_Normal > 0)  
                        
                      
       
