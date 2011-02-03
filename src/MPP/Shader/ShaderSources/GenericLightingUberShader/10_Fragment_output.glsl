//Fragment outputs

//applicable to following stages: fragment

//{%output}
//{
	#if (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)
		out vec4 finalLuminance;
	//bullshit, for depth/position-only rendering, we use a separat lean shader!
	//#	if (SHADING_FEATURE & SHADING_FEATURE_AMBIENT_OCCLUSION)
	//		out vec4 gBufferPosition;
	//#	endif
	#else
		out vec4 gBufferPosition;
		out vec4 gBufferNormal;
		out vec4 gBufferColor; 
	#	ifdef GBUFFER_INDEX_RENDERING
			out ivec4 gBufferGenericIndices;
	#	endif
	#endif
//}
