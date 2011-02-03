// uniforms and uniform buffers:
//{%uniforms}
//{
	uniform int numCurrentlyActiveLightSources = 1;

	#if (SHADING_FEATURE != SHADING_FEATURE_NONE)
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
			LightSource lightSources[NUM_MAX_LIGHT_SOURCES];
		};
	#	endif
	#endif



	#if (LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)

	//any hint we need more than one shadowmap? (assert also that multiple-lightsource-lighting is enabled)
	#	if    (LIGHT_SOURCES_SHADOW_FEATURE   == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS) \
		   && \
		   (   (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
	 	    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
	 	    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS ) \
		   )

		layout(shared) uniform worldToShadowMapMatrixBuffer
		{
			mat4 worldToShadowMapMatrices[NUM_MAX_LIGHT_SOURCES]; //bias*perspLight*viewLight
		};
	#	else
			//assert that lighting is enabled
	#		if  (LIGHT_SOURCES_LIGHTING_FEATURE != LIGHT_SOURCES_LIGHTING_FEATURE_NONE ) 
	
	#			if 	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT) 
					//only one spot light source for shadowing
					uniform mat4 worldToShadowMapMatrix; //bias*perspLight*viewLight
	#			endif //one spotlight
	
	#			if 	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT) 
	
					//in case of one point light shadow mapping, the lookup is in "non-1/z" camera space, 
					//i.e. needs no additional transformation matrix;
					//but because the depth buffer is always clamped to [0..1], the depth value
					//must be scaled by 1/farclipPlane of lightSource
					uniform float inverse_lightSourcesFarClipPlane;
					//bias for lookup, in case the glPolygonOffset doesn't apply when writing the gl_FragDepth manually;
					uniform float shadowMapComparisonOffset= -0.01;
										
	#			endif //one pointlight shadowmap
	
	#		endif //the lighting-enable-guard
	
	#	endif //the all-spotlights shadowmap-stuff
	
	//with no shadow mapping, we don't need a shadowmap transformation, of course :P
	#endif //(LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)
	//-----end lightsource/shadowmap interface

//}
