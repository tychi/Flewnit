//  shadow map samplers:

//applicable to following stages: fragment

//{%shadowMapSamplers}
//{

	#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT
	uniform sampler2DShadow shadowMap;
	#else
	#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT
	uniform samplerCubeShadow shadowMap;
	#else
	#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS
	uniform sampler2DArrayShadow shadowMap;
	#endif	
	#endif
	#endif

//}
