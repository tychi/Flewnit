//GLSL Shader Template: normal mapping calculations, normalization, G-Buffer-retrieval:

//applicable to following stages: fragment

//{%subroutine_getNormal}
//{
	vec3 getNormal()
	{
	#if (SHADING_FEATURE & SHADING_FEATURE_NORMAL_MAPPING) \
	    && \
	    (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		vec3 nonPerturbedNormal = texture(normalMap,inFTexCoords.xy).xyz;

		vec3 normalWN = normalize(inFNormal.xyz);
		vec3 tangentWN = normalize(inFTangent.xyz);
		vec3 binormalWN = cross(normalWN,tangentWN.xyz);
		mat3 TBNMatrix(tangentWN.xyz,binormalWN.xyz,normalWN.xyz);

		return TBNMatrix * nonPerturbedNormal;
	#else
		#if(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		return GBUFFER_ACQUIRE(normalTexture, sampleIndex).xyz;
		#else	
		return normalize(inFNormal.xyz);
		#endif
	#endif
	}
//}
