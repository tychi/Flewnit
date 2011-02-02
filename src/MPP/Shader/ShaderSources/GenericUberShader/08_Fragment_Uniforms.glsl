// uniforms and uniform buffers:
//{%uniforms}
//{
	uniform int numCurrentlyActiveLightSources = 1;

	#if (SHADER_LIGHTING_FEATURE != SHADER_LIGHTING_FEATURE_NONE)
	uniform vec4 eyeVecInWorldCoords;

	//clamp the attenuation due to shadowmapping to [minimalshadowAttenuation, 1.0]
	uniform float minimalshadowAttenuation = 0.2;
	#if (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		uniform float shininess = 2.0;
	#endif
	uniform float spotLightAmbientFactor = 0.1;
	uniform float cubeMapReflectivity = 0.33;
	#endif

	//----- begin lightsource/shadowmap interface ------------------------------------

	#if    (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT ) \
	    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT )
	uniform LightSource lightSource;
	#else
	#	if    (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
	 	   || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
	 	   || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
		layout(shared) uniform lightSourceBuffer
		{
			LightSource lightSources[NUM_LIGHT_SOURCES];
		};
	#	endif
	#endif



	#if (LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)

	//any hint we need more than one shadowmap?
	#	if    (LIGHT_SOURCES_SHADOW_FEATURE   == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS) \
		   && \
		   (   (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
	 	    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
	 	    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS ) \
		   )
		layout(shared) uniform worldToShadowMapMatrixBuffer
		{
			mat4 worldToShadowMapMatrices[NUM_LIGHT_SOURCES]; //bias*perspLight*viewLight
		};
	#	else
	#		if 	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT) \
		  	   &&   (LIGHT_SOURCES_LIGHTING_FEATURE != LIGHT_SOURCES_LIGHTING_FEATURE_NONE ) 
			//only one light source for shadowing
			uniform mat4 worldToShadowMapMatrix; //bias*perspLight*viewLight
			//in case of one point light shadow mapping, the lookup is in world space, i.e. needs no additional transformation :)
			//in no shadow mapping, we don't need a transformation, either;
	#		endif
	#	endif

	#endif //(LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)
	//-----end lightsource/shadowmap interface

//}
