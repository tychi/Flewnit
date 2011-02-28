{%comment%}
  GLSL Shader Template: spot light attenuation calculations:
  applicable to following stages: fragment                      {%endcomment%} 

{% if LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS %}
    
float getSpotLightAttenuation(    
  vec3  lightSourceDirection,   
  float lightSourceSpotExponent, 
  float lightSourceInnerSpotCutOff_Radians, 
  float lightSourceOuterSpotCutOff_Radians, 
  vec3  lightToFragWN
)
{   
  float spotLightConeAttenuation = 1.0;
  float cosLightDir_lightToFrag = dot( lightSourceDirection , lightToFragWN );
  float angleLightDir_lightToFrag_Radians = acos(cosLightDir_lightToFrag); 

  //is fragment within light cone?
  if (angleLightDir_lightToFrag_Radians <= lightSourceOuterSpotCutOff_Radians)
  {
   //spotLightConeAttenuation = pow(cosLightDir_lightToFrag, lightSourceSpotExponent );
   spotLightConeAttenuation = clamp( pow(cosLightDir_lightToFrag, lightSourceSpotExponent ), spotLightAmbientFactor , 1.0 );

   //Does the fragment lie within the border region of the cone?
   //Then smooth out the lights' contribution by linear interpolating between spotLightAmbientFactor and 1, 
   //and multiplicate this to spotLightConeAttenuation
   if (angleLightDir_lightToFrag_Radians > lightSourceInnerSpotCutOff_Radians)
   {
    float fractionWithinBorder =  
     (angleLightDir_lightToFrag_Radians - lightSourceInnerSpotCutOff_Radians) 
     / 
     (lightSourceOuterSpotCutOff_Radians - lightSourceInnerSpotCutOff_Radians);

    //spotLightConeAttenuation *= mix(1.0, spotLightAmbientFactor, fractionWithinBorder);
    spotLightConeAttenuation = mix(spotLightConeAttenuation, spotLightAmbientFactor, fractionWithinBorder);
   }
  }
  else
  {
    spotLightConeAttenuation = spotLightAmbientFactor;
    //spotLightConeAttenuation = spotLightAmbientFactor*  pow(cos(lightSourceOuterSpotCutOff_Radians), lightSourceSpotExponent );
  }

  return spotLightConeAttenuation;
}
{% endif %}

