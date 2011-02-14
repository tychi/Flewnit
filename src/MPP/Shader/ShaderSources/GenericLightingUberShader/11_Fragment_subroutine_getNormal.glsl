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
      return normalize(interfaceData.normal.xyz);
    {% else %}
      vec3 nonPerturbedNormal = texture(normalMap,interfaceData.texCoords.xy).xyz;

      vec3 normalN = normalize(interfaceData.normal.xyz);
      vec3 tangentN = normalize(interfaceData.tangent.xyz);
      vec3 binormalN = cross(normalN,tangentN.xyz);
		  
      mat3 TBNMatrix(tangentN.xyz,binormalN.xyz,normalN.xyz);
      return TBNMatrix * nonPerturbedNormal;   
    {% endif %}	
  {% endif %}	
}
{% endif %}  {%comment%} endif RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL or RENDERING_TECHNIQUE_DEFERRED_LIGHTING {%endcomment%}
