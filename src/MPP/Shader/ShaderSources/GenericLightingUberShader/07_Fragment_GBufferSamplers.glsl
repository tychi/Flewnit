{%comment%}
	GLSL Shader Template: G-Buffer samplers:
	applicable to following stages: fragment {%endcomment%} 
{% if RENDERING_TECHNIQUE_DEFERRED_LIGHTING or SHADING_FEATURE_AMBIENT_OCCLUSION %}
	{% if G_BUFFER_TEXTURE_TYPE_2D 							%}
	#define GBUFFER_SAMPLER 	sampler2D
	#define GBUFFER_ISAMPLER 	isampler2D
	#define GBUFFER_USAMPLER 	usampler2D
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelFetch(textureName, ivec2(gl_FragCoord.xy))
	{%endif%}
	{% if G_BUFFER_TEXTURE_TYPE_2D_RECT					%}
	#define GBUFFER_SAMPLER 	sampler2DRect
	#define GBUFFER_ISAMPLER 	isampler2DRect
	#define GBUFFER_USAMPLER 	usampler2DRect
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelFetch(textureName, ivec2(gl_FragCoord.xy))
	{%endif%}
	{% if G_BUFFER_TEXTURE_TYPE_2D_MULTISAMPLE	%}
	#define GBUFFER_SAMPLER 	sampler2DMS
	#define GBUFFER_ISAMPLER 	isampler2DMS
	#define GBUFFER_USAMPLER 	usampler2DMS
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelfetch(textureName, ivec2(gl_FragCoord.xy), sampleNum)
	{%endif%}
		uniform GBUFFER_SAMPLER 	positionInWorldCoordsTexture;
	{% if RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}
		uniform GBUFFER_SAMPLER 	normalInWorldCoordsTexture;
		uniform GBUFFER_SAMPLER 	colorTexture; //code shininess in alpha channel
		{%comment%}	no texcoord and tangent stuff, because simple decal texturing, cube and/or normal mapping will be applied directly 
								(the overhead with materialIndices write/read, texcoord write/read and several textures to be dependently read 
								(or globbed into an array) seems not worth the benefit of saving more overdraw, especially if a z-pass prepends 
								the G-Buffer- fill pass. (i may be wrong... ;() 																																		{%endcomment%}
		{% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION %}
	    	uniform GBUFFER_ISAMPLER	genericIndicesTexture;
		{% endif %}
	{% endif %}
{% endif %}
