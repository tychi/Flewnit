{%comment%}
  GLSL Shader Template: Fragment outputs
  applicable to following stages: fragment     {%endcomment%} 

{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING %}
		out vec4 outFFinalLuminance;
{% endif %}
	
{% if RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
		out vec4 outFGBufferPosition; //TODO NOT write position out this wasting way: try writing gl_FragDepth or single floating point texture instead when this default way is stable works
		out vec4 outFGBufferNormal;   //TODO NOT write normal   out this wasting way: try writing to two-channel normalized 8bit signed int texture instead when this default way is stable works out
		out vec4 outFGBufferColor; 	
{% endif %}


{%comment%} ################################# following shadow/pos/depth outputs ##############################################################{%endcomment%}
{% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION or RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION  %}
  {% if RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION %}
    out vec4 outFPosition;
{% endif %}
  
  {%comment%} for default spotlight shadowmap generation, there is no fragment shader necessary at all, hence no output variable {%endcomment%}
  {%comment%} for pointlight shadowmap or generic depth image generation, the output is written directly to gl_FragDepth,  hence no output variable {%endcomment%}
{% endif %}

{%comment%} ################################# following index            output ##############################################################{%endcomment%}
{% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION  %}
			out ivec4 outFGBufferGenericIndices; //usage examples : x: gl_PrimitiveID; y: z index of voxel z: material id
{% endif %}
