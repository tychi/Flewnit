{%comment%}
	GLSL Shader Template: G-Buffer samplers:
	applicable to following stages: fragment
{%endcomment%} 


{% ifequal shaderFeatures.renderingTechnique RENDERING_TECHNIQUE_DEFERRED_LIGHTING %}
	//the shadow map, its sampler type depends on the number and type of shadow casting lightsources
	{% if lightSourcesShadowFeatureOneSpotLight 	%}uniform sampler2DShadow 			shadowMap;	{%endif%}
	{% if lightSourcesShadowFeatureOnePointLight 	%}uniform samplerCubeShadow 		shadowMap;	{%endif%}
	{% if lightSourcesShadowFeatureAllSpotLights	%}uniform sampler2DArrayShadow 	shadowMap;	{%endif%}
{% endif %}


	#if (SHADING_FEATURE & SHADING_FEATURE_AMBIENT_OCCLUSION) \
	  ||(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)

	#if (GBUFFER_TYPE ==	TEXTURE_TYPE_2D)
	#define GBUFFER_SAMPLER 	sampler2D
	#define GBUFFER_ISAMPLER 	isampler2D
	#define GBUFFER_USAMPLER 	usampler2D
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelFetch(textureName, ivec2(gl_FragCoord.xy))
	#endif

	#if (GBUFFER_TYPE ==	TEXTURE_TYPE_2D_RECT)
	#define GBUFFER_SAMPLER 	sampler2DRect
	#define GBUFFER_ISAMPLER 	isampler2DRect
	#define GBUFFER_USAMPLER 	usampler2DRect
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelFetch(textureName, ivec2(gl_FragCoord.xy))
	#endif

	#if (GBUFFER_TYPE ==	TEXTURE_TYPE_2D_MULTISAMPLE)
	#define GBUFFER_SAMPLER 	sampler2DMS
	#define GBUFFER_ISAMPLER 	isampler2DMS
	#define GBUFFER_USAMPLER 	usampler2DMS
	#define GBUFFER_ACQUIRE(textureName, sampleNum) texelfetch(textureName, ivec2(gl_FragCoord.xy), sampleNum)
	#endif

	#endif


	#if (SHADING_FEATURE & SHADING_FEATURE_AMBIENT_OCCLUSION) \
	  ||(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		uniform GBUFFER_SAMPLER 	positionInWorldCoordsTexture;
	#endif

	//no texcoord stuff, because simple decal texturing or cubemapping will be applied directly (the overhead with materialIndices 
	//write/read, texcoord write/read and several textures to be dependently read (or globbed into an array) seems not worth the benefit of saving more overdraw
	//(i may be wrong... ;()
 

	#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		uniform GBUFFER_SAMPLER 	normalInWorldCoordsTexture;
		//no tangent stuff needed for the same reasons as for the texture coords
		//no texcoords needed, as "final" color already in colorTexture
		uniform GBUFFER_SAMPLER 	colorTexture; //code shininess in alpha channel
	#	ifdef GBUFFER_INDEX_RENDERING
	    	uniform GBUFFER_ISAMPLER	genericIndicesTexture;
	#	endif
	#endif

