{%comment%}
	GLSL Shader Template: shadow map samplers:
	applicable to following stages: fragment
{%endcomment%} 
{% ifequal shaderFeatures.renderingTechnique.key RENDERING_TECHNIQUE_DEFAULT_LIGHTING %}
	//the shadow map, its sampler type depends on the number and type of shadow casting lightsources
	{% if lightSourcesShadowFeatureOneSpotLight 	%}uniform sampler2DShadow 			shadowMap;	{%endif%}
	{% if lightSourcesShadowFeatureOnePointLight 	%}uniform samplerCubeShadow 		shadowMap;	{%endif%}
	{% if lightSourcesShadowFeatureAllSpotLights	%}uniform sampler2DArrayShadow 	shadowMap;	{%endif%}
{% endif %}
