//  material samplers:

//applicable to following stages: fragment

//{%materialSamplers}
//{

#if 	(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFAULT_LIGHTING) \
     ||	(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)

	//following textures helping to determine the "unlit" fragment color to be lit or to be written to the Color buffer of the G-Buffer
	#if (SHADING_FEATURE & SHADING_FEATURE_DECAL_TEXTURING)
	uniform sampler2D decalTexture;
	#endif


	#if (SHADING_FEATURE & SHADING_FEATURE_NORMAL_MAPPING)
	uniform sampler2D normalMap;
	#endif

	#if (SHADING_FEATURE & SHADING_FEATURE_CUBE_MAPPING)
	uniform samplerCube cubeMap;
	#endif

#endif //end "unlit" stuff

//}
