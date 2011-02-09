{%comment%}
	GLSL Shader Template: material samplers:
	applicable to following stages: fragment
{%endcomment%} 

what is shaderFeatures?: {{ shaderFeatures.renderingTechnique.3.key }}

renderingTechniqueDefaultLighting: {{renderingTechniqueDefaultLighting}}

Oliver is value {{ myObj.Oliver }}.

{% if shaderFeatures.renderingTechnique  %}
YEAH
	//following textures helping to determine the "unlit" fragment color to be lit or to be written to the Color buffer of the G-Buffer
	{% if shadingFeatureDecalTexturing 	%}uniform sampler2D decalTexture;	{%endif%}
	{% if shadingFeatureNormalMapping 	%}uniform sampler2D normalMap;		{%endif%}
	{% if shadingFeatureCubeMapping	 		%}uniform sampler2D cubeMap;			{%endif%}
{% endif %}

