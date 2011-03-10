{%comment%}
  GLSL Shader Template: subroutine for normal mapping calculations, normalization, possibly G-Buffer normal-retrieval:
  applicable to following stages: fragment     {%endcomment%} 

{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL or RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}
vec3 getNormal(int sampleIndex)
{
  {% if RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}
    return GBUFFER_ACQUIRE(normalTexture, sampleIndex).xyz;
  {% else %}
    {% if not SHADING_FEATURE_NORMAL_MAPPING %}
      return normalize(input.normal.xyz);
    {% else %}
      vec3 nonPerturbedNormal = (texture(normalMap,input.texCoords.xy).xyz - vec3(0.5)) ;//* 2.0;
      //vec3 nonPerturbedNormal = (texture(normalMap,input.texCoords.xy).xyz );
      nonPerturbedNormal = normalize(nonPerturbedNormal);

      vec3 normalN = normalize(input.normal.xyz);
      vec3 tangentN = normalize(input.tangent.xyz);
      vec3 binormalN = cross(normalN,tangentN.xyz);
      //binormalN = normalize(binormalN);
		  
      mat3 TBNMatrix= mat3(tangentN.xyz,binormalN.xyz,normalN.xyz);
      
      //vec3 debugNormal = TBNMatrix * nonPerturbedNormal;   
      //if(debugNormal.z < 0.0) return vec3(100.0,0.0,0.0); 
      
      return TBNMatrix * nonPerturbedNormal;   
    {% endif %}	
  {% endif %}	
}
{% endif %}  {%comment%} endif RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL or RENDERING_TECHNIQUE_DEFERRED_LIGHTING {%endcomment%}
