//GLSL Shader Template: shadow attenuation calculations:

//applicable to following stages: fragment

//{%subroutine_getShadowAttenuation}
//{
	float getShadowAttenuation(int lightSourceIndex, vec3 modelWorldPos)
	{
	#if 	(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_NONE) || (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_NONE)
		return 1.0;
	#	endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	#if 	(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_DEFAULT)

		//-----------------------------------------------------------------------------------------------

		//spotlightcalc:
		#if	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT) \
		  ||	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS)

			#if(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT)
				if(lightSourceIndex > 0 )
				{
					return 1.0;
				}			

				vec3 shadowCoord =positionInWorldCoords * worldToShadowMapMatrix;
				//divide by homogene coord:
				shadowCoord /= shadowCoord.w;
			#endif

			#if(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS)
				vec4 shadowCoord =positionInWorldCoords * worldToShadowMapMatrices[lightSourceIndex];
				//divide by homogene coord:
				shadowCoord /= shadowCoord.w;
				//rearrange for array shadowmap lookup:		
				shadowCoord = 	vec4(	shadowCoord.x,shadowCoord.y, 
							//hope that I understood the spec correctly, that there is no [0..1] scaling like in texture2D:
							//max 0,min d −1, floorlayer0.5
							(float)(lightSourceIndex),
							shadowCoord.z 	) ;
			#endif
	

			//we don't want a squared shadow-throwing lightsource impression, but a circled one:	
			if( length(vec2(0.5 + shadowCoord.x, 0.5  shadowCoord.y)) < 0.5 )
			{
				return  clamp(texture( shadowMap, shadowCoord ), minimalshadowAttenuation , 1.0);	
			}
			else {return minimalshadowAttenuation;}
		#endif //spotlightcalc

		//-----------------------------------------------------------------------------------------------

		//pointLightCalc
		//assert also that the LIGHTING is only single pointlight, otherwise the shading logic would be TO unrealistic
		//(a technical reason for this restriction is also, that a lookup in GPU global memory due to the uniform buffer lookup shall be omitted due to 	
		//performance reasons)
		#if  (LIGHT_SOURCES_SHADOW_FEATURE   == LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT) \
		  && (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
		
			if(lightSourceIndex > 0 )
			{
				return 1.0;
			}			

			vec3 lightToModelW = modelWorldPos - lightSource.position;
			float cubeShadowMapDepthValue = texture(lightToModelW, shadowMap);
			if(length(lightToModelW) < cubeShadowMapDepthValue)
				{return 1.0;}else{return minimalshadowAttenuation;}
		#endif //pointLightCalc

	#	endif //(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_DEFAULT)

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	#	if (SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_PCFSS)
			not implemented :P
	#	endif
	}
//}
