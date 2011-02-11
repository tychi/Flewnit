//GLSL Shader Template: shadow attenuation calculations:

//applicable to following stages: fragment

//{%subroutine_getShadowAttenuation}
//{
	float getShadowAttenuation(float shadowMapLayer, vec3 modelWorldPos)
	{
	#if 	(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_NONE) || (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_NONE)
		return 1.0;
	#	endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	#if 	(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_DEFAULT)

		//-----------------------------------------------------------------------------------------------

		//spotlightcalc:
		#if	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT) \
		  ||	(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)

			#if(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
				if(shadowMapLayer > 0.001 )
				{
					return 1.0;
				}			

				vec3 inFShadowCoord =inFPosition * worldToShadowMapMatrix;
				//divide by homogene coord:
				inFShadowCoord /= inFShadowCoord.w;
			#endif

			#if(LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)
				vec4 inFShadowCoord =inFPosition * worldToShadowMapMatrices[(int)(floor(shadowMapLayer+0.5))];
				//divide by homogene coord:
				inFShadowCoord /= inFShadowCoord.w;
				//rearrange for array shadowmap lookup:		
				inFShadowCoord = 	vec4(	inFShadowCoord.x,inFShadowCoord.y, 
							//hope that I understood the spec correctly, that there is no [0..1] scaling like in texture2D:
							//max 0,min d −1, floorlayer0.5
							shadowMapLayer,
							inFShadowCoord.z 	) ;
			#endif
	

			//we don't want a squared shadow-throwing lightsource impression, but a circled one:	
			if( length(vec2(0.5 + inFShadowCoord.x, 0.5  inFShadowCoord.y)) < 0.5 )
			{
				return  clamp(texture( shadowMap, inFShadowCoord ), minimalshadowAttenuation , 1.0);	
			}
			else {return minimalshadowAttenuation;}
		#endif //spotlightcalc

		//-----------------------------------------------------------------------------------------------

		//pointLightCalc
		//assert also that the LIGHTING is only single pointlight, otherwise the shading logic would be TO unrealistic
		//(a technical reason for this restriction is also, that a lookup in GPU global memory due to the uniform buffer lookup shall be omitted due to 	
		//performance reasons)
		#if  (LIGHT_SOURCES_SHADOW_FEATURE   == LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT) \
		  && (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
		
			if(shadowMapLayer > 0.001 )
			{
				return 1.0;
			}			

			vec3 lightToModelW = modelWorldPos - lightSource.position;		
			float comparisonReferenceValue = 
				( 	length(lightToModelW) 
					//scale to compensate for the depthbuffer [0..1] clamping	
					* inverse_lightSourcesFarClipPlane	 )
				+ shadowMapComparisonOffset;
			return texture(shadowMap, vec4(lightToModelW.xyz,comparisonReferenceValue ));
			
			
			//float cubeShadowMapDepthValue = texture(shadowMap,lightToModelW );
			//if(length(lightToModelW) < cubeShadowMapDepthValue)
			//	{return 1.0;}else{return minimalshadowAttenuation;}
		#endif //pointLightCalc

	#	endif //(SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_DEFAULT)

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	#	if (SHADOW_TECHNIQUE == SHADOW_TECHNIQUE_PCFSS)
			not implemented :P
	#	endif
	}
//}
