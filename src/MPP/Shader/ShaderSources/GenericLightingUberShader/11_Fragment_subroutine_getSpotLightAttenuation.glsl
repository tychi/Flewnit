{%comment%}
  GLSL Shader Template: spot light attenuation calculations:
  applicable to following stages: fragment                      {%endcomment%} 

{% if LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS %}
    
float getSpotLightAttenuation(    
  vec3  lightSourceDirection,   
  float lightSourceSpotExponent, 
  float lightSourceInnerSpotCutOff_Radians, 
  float lightSourceOuterSpotCutOff_Radians_Radians, 
  vec3  lightToFragWN
)
{   
  float spotLightConeAttenuation = 1.0;
  float cosLightDir_lightToFragW = dot( lightSourceDirection , lightToFragWN );
  float angleLightDir_lightToFragW_Radians = acos(cosLightDir_lightToFragW); 

  //is fragment within light cone?
  if (angleLightDir_lightToFragW_Radians <= lightSourceOuterSpotCutOff_Radians_Radians)
  {
   spotLightConeAttenuation = pow(cosLightDir_lightToFragW, lightSourceSpotExponent );

   //Does the fragment lie within the border region of the cone?
   //Then smooth out the lights' contribution by linear interpolating between spotLightAmbientFactor and 1, 
   //and multiplicate this to spotLightConeAttenuation
   if (angleLightDir_lightToFragW_Radians > lightSourceInnerSpotCutOff_Radians)
   {
    float fractionWithinBorder =  
     (angleLightDir_lightToFragW_Radians - lightSourceInnerSpotCutOff_Radians) 
     / 
     (lightSourceOuterSpotCutOff_Radians_Radians - lightSourceInnerSpotCutOff_Radians);

    spotLightConeAttenuation *= mix(1.0, spotLightAmbientFactor, fractionWithinBorder);
   }
  }
  else
  {
   spotLightConeAttenuation = spotLightAmbientFactor;
  }

  return spotLightConeAttenuation;
}
{% endif %}

