//Fragment inputs from Vertex/Geom/Tesseleation evaluation shader

//applicable to following stages: fragment

//{%input}
//{
	//a number of inputs from the vertex shader; come from textures or are obsolete when doing deferred lighting on a G-buffer, hence the #define
	#if (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
	in vec4 positionInWorldCoords;
	in vec4 normalInWorldCoords;
	in vec4 texCoords;
	#if (SHADING_FEATURE & SHADING_FEATURE_NORMAL_MAPPING)
	//create TBN-matrix in fragment shader due to the several lightsources;
	//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
	in vec4 tangentInWorldCoords;
	#endif

	#ifdef GBUFFER_INDEX_RENDERING
	in ivec4 genericIndices;
	#endif

	#endif //not deferred ligthing

	//later TODO test this optimaziation for one shadowmap..
	//#if (defined(SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER) \
	//  && (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT  ) )
	//in vec4 shadowCoord;
	//#endif

//}
