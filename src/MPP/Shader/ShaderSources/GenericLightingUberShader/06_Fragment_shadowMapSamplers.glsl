{%comment%}
	GLSL Shader Template: shadow map samplers:
	applicable to following stages: fragment		{%endcomment%} 
{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING and not LIGHT_SOURCES_SHADOW_FEATURE_NONE %}
	//the shadow map, its sampler type depends on the number and type of shadow casting lightsources
	{% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT 	%}uniform sampler2DShadow 			shadowMap;	{%endif%}
	{% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT %}uniform samplerCubeShadow 		shadowMap;	{%endif%}
	{% if LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS	%}uniform sampler2DArrayShadow 	shadowMap;	{%endif%}
{% endif %}
