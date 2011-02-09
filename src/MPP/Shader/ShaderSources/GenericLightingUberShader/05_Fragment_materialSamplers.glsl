{%comment%}
	GLSL Shader Template: material samplers:
	applicable to following stages: fragment {%endcomment%} 
{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
	//following textures helping to determine the "unlit" fragment color to be lit or to be written to the Color buffer of the G-Buffer
	{% if SHADING_FEATURE_DECAL_TEXTURING 	%}uniform sampler2D decalTexture;		{%endif%}
	{% if SHADING_FEATURE_DETAIL_TEXTURING 	%}uniform sampler2D detailTexture;	{%endif%}
	{% if SHADING_FEATURE_NORMAL_MAPPING 		%}uniform sampler2D normalMap;			{%endif%}
	{% if SHADING_FEATURE_CUBE_MAPPING	 		%}uniform sampler2D cubeMap;				{%endif%}
{% endif %}
