//Second concept of a fragment shader template

/*
This "generic uber shader" shall not only serve as example for the template structure,
but also do every rendering in the framework except:
	- pure depth/position image generation (like for cube shadowmaps, which need dpeth values in translated wolrd space, 
		or for ambient occlusion-depth/position rendering-only passes)
	- specialized fluidrendering (might be done anyway later, if the shader structure turns out to be compatible)
Even debug stuff (index visualization etc.) shall be done by this shader
*/


//{%versionTag}

//{%precisionTag}

//{%persistentdefines}

//customizable defines have definetely to be set by the shaders (via string concatenation):
//for examples and explanation refer to file "2_Generic_C_includable_persistentDefines.glsl";
//{%customizabledefines}


//{%typeDefinitions}



//---- interface ---------------------------------------------------------------------------------

//---- application (uniform) input ----

//{%materialSamplers}

//{%shadowMapSamplers}

//{%GBufferSamplers}

//non-sampler uniforms and uniform buffers:
//{%uniforms}


//---- shader input --------------------

//{%input}

//---- shader output -------------------

//{%output}

//------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------
//subroutines (atm not in the GL4-style ;()
//{%subroutines}
//{
	//{%subroutine_getNormal}

	//{%subroutine_getShadowAttenuation}

	//{%subroutine_getDistanceAttenuation}

	//{%subroutine_getSpotLightAttenuation}

//}


void main()
{

#if (RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)

	finalLuminance = vec4(0.0,0.0,0.0,0.0);
	
//{%codeFragment_initSampleValues}
//{
	#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
	float totalValidMultiSamples=0.0;
	for(int sampleIndex = 0; sampleIndex < NUM_MULTISAMPLES ;sampleIndex++);
	{
		//acquire G-buffer values in order to omit multiple reads to same texture and to use the same variable names in the following code;
		vec3 normalWN		=	GBUFFER_ACQUIRE(normalInWorldCoordsTexture, sampleIndex).xyz;
		if(length(normalWN) < 0.1 )
		{
			//normal should have length 1, unless the fragment was never written (or on purpose set to 0, as for skydomes)!
			//tihs means, it is a background sample that MAY NOT BE SHADED; so continue;
			continue;
		}
		vec3 positionInWorldCoords = 	GBUFFER_ACQUIRE(positionInWorldCoordsTexture, sampleIndex).xyz;
		vec4 fragmentColor =		GBUFFER_ACQUIRE(colorTexture, sampleIndex).xyzw;
		float shininess = fragmentColor.w;
		fragmentColor.w = 1.0; //reset alpha to omit some fuckup
		#ifdef GBUFFER_INDEX_RENDERING
	    	ivec4 genericIndices = 		GBUFFER_ACQUIRE(genericIndicesTexture, sampleIndex).xyzw;
		#endif
	#else //endif deferred lighting
		vec3 normalWN = getNormal();
		vec4 fragmentColor =
			#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_DECAL_TEXTURING)
			texture(decalTexture,texCoords.xy);
			#else
			vec4(1.0,1.0,1.0,1.0);
			#endif
	#endif //(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)

		vec3 fragToCamN = normalize(eyeVecInWorldCoords - positionInWorldCoords);

//} //end codeFragment_initSampleValues


//{%codeFragment_initLightValues}
//{
		//variable to accum all light for one sample
		vec4 incidentLight = vec4(0.0,0.0,0.0,0.0);

		#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING)
		
		#if    (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
		 	   || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
		 	   || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )	
		for(int lightIndex = 0; lightIndex < NUM_LIGHTSOURCES ;lightIndex++);
		{
			//lets hope that there will be a component wise copy and NOT some C++-f***up about non-existing operator=() ;)
			LightSource lightSource = lightSources[lightIndex];
		#endif

//} //end codeFragment_initLightValues

//{%codeFragment_diffuseLightingCalc}
//{
			vec3 lightToFragW =   positionInWorldCoords - lightSource.position;
			vec3 lightToFragWN =  normalize(lightToFragW);

			float cosFragToLight_Normal = dot( (-1.0) * lightToFragWN , normalWN);

			//is fragment facing to the light?
			if(cosFragToLight_Normal > 0)
			{			
				float diffuseFactor  = cosFragToLight_Normal;
//} //end codeFragment_diffuseLightingCalc

//{%codeFragment_specularLightingCalc}
//{
				vec3 reflectedlightToFragW = reflect ( lightToFragWN , normalWN);
				float cosFragToCam_reflectedlightToFragW = dot(fragToCamN,reflectedlightToFragW);
				float specularFactor = pow(cosFragToCam_reflectedlightToFragW, shininess);
//} //end codeFragment_specularLightingCalc


//{%codeFragment_accumLightValues}
//{	
				incidentLight + =
					getDistanceAttenuation(lightToFragW) 
					* 
					getShadowAttenuation(lightIndex, positionInWorldCoords) 
					* 
					#if (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT ) \
					  ||  (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	
						#if(LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
						//is the lightsource a spotlight?
						(lightSource.innerSpotCutOff_Radians <= 0.0) ? 1.0 : //if cutoff <=0.0, it will be interpreted as point light
						#endif
						getSpotLightAttenuation(	lightSource.direction,			
										lightSource.spotExponent, 
										lightSource.innerSpotCutOff_Radians, 
										lightSource.outerSpotCutOff_Radians, 
										lightToFragWN				) 
						*
					#endif	
					(
						( vec4(lightSource.diffuseColor.xyz,1.0) *  diffuseFactor ) +
						( vec4(lightSource.specularColor.xyz,1.0) *  specularFactor )
					);

			} //endif is fragment facing to the light (cosFragToLight_Normal > 0)	

		#if  (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
	 	 || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
	 	 || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
		} //end of for lightsources-loop
		#endif
//} //end codeFragment_initLightValues


//{%codeFragment_accumSampleValues}
//{
		#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		totalValidMultiSamples += 1.0;
		#endif

		finalLuminance += incidentLight * fragmentColor;

		#else 	//if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING)
		finalLuminance += fragmentColor; //only accum fragment colors without shading
		#endif 	//if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING)


	#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)	
	} //end of for multisamples-loop
	//divide accumulated luminance by numSamples:
	if(totalValidMultiSamples >0.0) finalLuminance /= totalValidMultiSamples;
	#endif //(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
//} //end codeFragment_accumSampleValues


//{%codeFragment_cubeMapping}
//{
	#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_CUBE_MAPPING)
	//lerp between actual color and cubemap color
	finalLuminance = mix( 	finalLuminance, 
				texture(cubeMap, normalWN),
				cubeMapReflectivity );
	#endif
//} //end codeFragment_cubeMapping
	
#endif//(RENDERING_TECHNIQUE != RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)
	//TODO, too tired now
#endif

} //endMain
