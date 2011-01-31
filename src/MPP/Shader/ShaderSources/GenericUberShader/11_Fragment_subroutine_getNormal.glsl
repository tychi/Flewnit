//GLSL Shader Template: normal mapping calculations, normalization, G-Buffer-retrieval:

//applicable to following stages: fragment

//{%subroutine_getNormal}
//{
	vec3 getNormal()
	{
	#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_NORMAL_MAPPING) \
	    && \
	    (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		vec3 nonPerturbedNormal = texture(normalMap,texCoords.xy).xyz;

		vec3 normalWN = normalize(normalInWorldCoords.xyz);
		vec3 tangentWN = normalize(tangentInWorldCoords.xyz);
		vec3 binormalWN = cross(normalWN,tangentWN.xyz);
		mat3 TBNMatrix(tangentWN.xyz,binormalWN.xyz,normalWN.xyz);

		return TBNMatrix * nonPerturbedNormal;
	#else
		#if(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		return GBUFFER_ACQUIRE(normalInWorldCoordsTexture, sampleIndex).xyz;
		#else	
		return normalize(normalInWorldCoords.xyz);
		#endif
	#endif
	}
//}
