{%comment%}
GLSL Shader Template: subroutine for distance attenuation calculations:
applicable to following stages: fragment                                {%endcomment%} 

{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING or RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}
float 	getDistanceAttenuation(vec3 lightToFragW) 			
{
	/*	
	distanceAttenuation formula hopefully more performant than the following:
	distanceAttenuation = 1.0 / (length(fragToLight)* length(fragToLight));
	(first take a sqrt, than take the reciprocal of its sqare... this is too much overhead :P)	
	*/
	return  100.0 / sqrt( (lightToFragW.x * lightToFragW.x) + (lightToFragW.y * lightToFragW.y) + (lightToFragW.z * lightToFragW.z) );
}
{% endif %}  {%comment%} endif RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_LIGHTING {%endcomment%}
