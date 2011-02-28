{%comment%}
	GLSL Shader Template: material samplers:
	applicable to following stages: fragment {%endcomment%} 
{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
	//following textures helping to determine the "unlit" fragment color to be lit or to be written to the Color buffer of the G-Buffer
	{% if SHADING_FEATURE_DECAL_TEXTURING 	%}uniform sampler2D decalTexture;		{%endif%}
	{% if SHADING_FEATURE_DETAIL_TEXTURING 	%}uniform sampler2D detailTexture;	{%endif%}
	{% if SHADING_FEATURE_NORMAL_MAPPING 		%}uniform sampler2D normalMap;			{%endif%}
	{% if SHADING_FEATURE_CUBE_MAPPING	 		%}uniform samplerCube cubeMap;				{%endif%}
	{% if SHADING_FEATURE_AMBIENT_OCCLUSION %}
	  uniform sampler2D depthBufferForAO; //when doing deferred lighting, we can use the positiontexture directly
	  {%comment%} TODO if really doing AO, there is a lot of work to be done;
	              This is just a conceptional stub                                          {%endcomment%} 	
	{%endif%}
{% endif %}
