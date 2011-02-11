{%comment%}
  GLSL Shader Template: shadow attenuation calculations:
  applicable to following stages: fragment       
  
  notes:  no check for compatibility to lighting features here to save complexitiy;
          exception: point light, becaus point light shadows with spot light lighting would be too stupid         {%endcomment%} 

{% if SHADING_FEATURE_AMBIENT_OCCLUSION %}
float getAOAttenuation()
{
  STILL TO IMPLEMENT IN FOR FUTURE;
  return 1.0;
}
{% endif %}  {%comment%} endif SHADING_FEATURE_AMBIENT_OCCLUSION {%endcomment%}
