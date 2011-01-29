//A first concept of a fragment shader template

//section 0: Version Tag
//{%versionTag}
//{
#version 330 core
//}

//section 1: precision:
//{%precisionTag}
//{
precision highp float;
//}

//------------------------------------------------------------------------------------------------
//section 2: defines:
//{%defines}
//{

	//section 2a: persistent defines:
	//{%persistentdefines}
	//{
	
	#define TRUE 1
	#define FALSE 0

	#define	POSITION_SEMANTICS 	0
	#define	NORMAL_SEMANTICS	1
	#define	TANGENT_SEMANTICS	2
	#define	TEXCOORD_SEMANTICS	3
	//for debug draw and water rendering semantics
	#define VELOCITY_SEMANTICS	4
	#define MASS_SEMANTICS		5
	#define DENSITY_SEMANTICS	6
	#define PRESSURE_SEMANTICS	7
	#define FORCE_SEMANTICS		8
	#define Z_INDEX_SEMANTICS	9

	//the texture-only semantics don't play a role here, as certain textures are bound and identified by a name, not via an array

	//#define TEXTURE_ACCESS_DEFAULT 	0
	//#define TEXTURE_ACCESS_RECT	1
	//#define TEXTURE_ACCESS_CUBE	2
	//#define TEXTURE_ACCESS_ARRAY	3

	#define RENDERING_TECHNIQUE_DIRECT			0
	#define	RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL	1
	#define	RENDERING_TECHNIQUE_DEFERRED_LIGHTING		2

	#define LIGHT_SOURCES_SHADOW_FEATURE_NONE		0
	#define LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT	1
	#define LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT	2
	#define LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	3

	#define SHADOW_TECHNIQUE_NONE	0
	#define SHADOW_TECHNIQUE_DEFAULT	1
	#define SHADOW_TECHNIQUE_PCFSS		2

	#define LIGHT_SOURCES_LIGHTING_FEATURE_NONE		0
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT	1
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT	2
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS	3
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS	4
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS	5

	#define SHADER_LIGHTING_FEATURE_NONE			0
	#define SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING		1<<0
	//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented	
	#define SHADER_LIGHTING_FEATURE_GLOBAL_LIGHTING		1<<0
	#define SHADER_LIGHTING_FEATURE_DECAL_TEXTURING		1<<1
	#define SHADER_LIGHTING_FEATURE_NORMAL_MAPPING		1<<2
	#define SHADER_LIGHTING_FEATURE_CUBE_MAPPING		1<<3
	#define SHADER_LIGHTING_FEATURE_AMBIENT_OCCLUSION	1<<4


	//}

	//section 2b: customizable defines have definetely to be set by the shaders (via string concatenation):
	//{%customizabledefines}
	//{
	//these are only examples:
	
	/*
	//the absolute minimal, for initial debugging:
	#define RENDERING_TECHNIQUE 				RENDERING_TECHNIQUE_DIRECT
	#define LIGHT_SOURCES_SHADOW_FEATURE 			LIGHT_SOURCES_SHADOW_FEATURE_NONE
	#define SHADOW_TECHNIQUE 				SHADOW_TECHNIQUE_NONE
	#define LIGHT_SOURCES_LIGHTING_FEATURE			LIGHT_SOURCES_LIGHTING_FEATURE_NONE
	#define SHADER_LIGHTING_FEATURE 			SHADER_LIGHTING_FEATURE_NONE


	//rather minimalistic phong: direct lighting without texturing or shadowing with one point light
	#define RENDERING_TECHNIQUE 				RENDERING_TECHNIQUE_DIRECT
	#define LIGHT_SOURCES_SHADOW_FEATURE 			LIGHT_SOURCES_SHADOW_FEATURE_NONE
	#define SHADOW_TECHNIQUE 				SHADOW_TECHNIQUE_NONE
	#define LIGHT_SOURCES_LIGHTING_FEATURE			LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT
	#define SHADER_LIGHTING_FEATURE 			SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING

	//amend the before with decal textureing
	#define RENDERING_TECHNIQUE 				RENDERING_TECHNIQUE_DIRECT
	#define LIGHT_SOURCES_SHADOW_FEATURE 			LIGHT_SOURCES_SHADOW_FEATURE_NONE
	#define SHADOW_TECHNIQUE 				SHADOW_TECHNIQUE_NONE
	#define LIGHT_SOURCES_LIGHTING_FEATURE			LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT
	#define SHADER_LIGHTING_FEATURE 			SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING | SHADER_LIGHTING_FEATURE_DECAL_TEXTURING

	
	//default classic phong rendering: nondeferred, one shadowmap, decal tex, one lightsource, quite conservative ;)	
	#define RENDERING_TECHNIQUE 				RENDERING_TECHNIQUE_DIRECT
	#define LIGHT_SOURCES_SHADOW_FEATURE 			LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT
	#define SHADOW_TECHNIQUE 				SHADOW_TECHNIQUE_DEFAULT
	#define LIGHT_SOURCES_LIGHTING_FEATURE			LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT
	#define SHADER_LIGHTING_FEATURE 			SHADER_LIGHTING_FEATURE_DIRECT_LIGHTING | SHADER_LIGHTING_FEATURE_DECAL_TEXTURING

	//normal mapping and all the rest to go when those initial stuff runs;
	*/


	//#define SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER

	#define SHADER_FEATURE_INSTANCING
	#define NUM_INSTANCES 4
	#define NUM_LIGHTSOURCES 4
	//}




//}


//------------------------------------------------------------------------------------------------
//section 3: Data type definitions:
//{%typeDefinitions}
//{
struct LightSource
{
	vec3 position;
	vec3 diffuseColor;
	//a dedicated specular color to produce unrealistic but nice effects;	
	vec3 specularColor;

	//----------------------------------------------------------------------
	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
	//to indicate a point light, those valus are all zero
	vec3 direction;
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;
	float outerSpotCutOff_Radians;
	float spotExponent;
	//alignment is everything :P
	//value in [0..1] at which the attenuation factor due to shadowing is clamped
	//if the SHADOW_FEATURE_ALL_SPOTLIGHTS is enabled, a lightsource can have a shadowmap in a Sampler2DArrayShadow;
	//is negative if no shadowmap is associated
	float shadowMapLayer;
	
	//float lightSourceAmbientFactor;

	//aligned to 64 bytes;
};
//}
//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------
//section 4:  uniform samplers:
//{%uniformSamplers}
//{

#if 	(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DIRECT) \
     ||	(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL)

	//following textures helping to determine the "unlit" fragment color to be lit or to be written to the Color buffer of the G-Buffer
	#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_DECAL_TEXTURING)
	uniform sampler2D decalTexture;
	#endif


	#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_NORMAL_MAPPING)
	uniform sampler2D normalMap;
	#endif

	#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_CUBE_MAPPING)
	uniform samplerCube cubeMap;
	#endif

#endif //end "unlit" stuff



#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT
uniform sampler2DShadow shadowMap;
#else
#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT
uniform sampler2DCubeShadow shadowMapCube;
#else
#if LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS
uniform sampler2DArrayShadow shadowMapArray;
#endif
#endif
#endif




#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_AMBIENT_OCCLUSION) \
  ||(RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
	uniform sampler2DRect worldPosTexture;
#endif

#if (RENDERING_TECHNIQUE == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
	uniform sampler2DRect normalTexture;
	//no texcoords needed, as "final" color already in colorTexture
	uniform sampler2DRect colorTexture; //code shininess in alpha channel
#endif
//}
//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------
//redesign:	uniformBuffers standardUniforms get one common section, as readability is better
//		when stuff has semantic locality and not technical;	
//section 5:  uniforms and uniform buffers:
//{%uniforms}
//{

//-----begin lightsource/shadowmap interface

#if    (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT ) \
    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT )
uniform LightSource lightSource;
#else
#if    (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS ) \
    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS ) \
    || (LIGHT_SOURCES_LIGHTING_FEATURE == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
layout(shared) uniform lightSourceBuffer
{
	LightSource lightSources[NUM_LIGHTSOURCES];
};
#endif
#endif



#if (LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)
#if (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS)
layout(shared) uniform worldToShadowMapMatrixBuffer
{
	mat4 worldToShadowMapMatrices[NUM_LIGHTSOURCES]; //bias*perspLight*viewLight
};
#else
#if (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT)
//only one light source for shadowing
uniform mat4 worldToShadowMapMatrix; //bias*perspLight*viewLight
//in case of one point light shadow mapping, the lookup is in world space, i.e. needs no additional transformation :)
//in no shadow mapping, we don't need a transformation, either;
#endif
#endif
#endif //(LIGHT_SOURCES_SHADOW_FEATURE != LIGHT_SOURCES_SHADOW_FEATURE_NONE)

//-----end lightsource/shadowmap interface




#if (SHADER_LIGHTING_FEATURE != SHADER_LIGHTING_FEATURE_NONE)
uniform vec4 eyeVecInWorldCoords;

//clamp the attenuation due to shadowmapping to [minimalshadowAttenuation, 1.0]
uniform float minimalshadowAttenuation = 0.2;
uniform float shininess = 1.0;
uniform float lightSourceAmbientFactor = 0.1;
uniform float cubeMapReflectivity = 0.5;
#endif
//}
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//section 6: Fragment inputs from Vertex/Geom/Tesseleation evaluation shader
//{%shaderSpecificInput}
//{
in vec4 positionInWorldCoords;
in vec4 normalInWorldCoords;
in vec4 texCoords;
#if (SHADER_LIGHTING_FEATURE & SHADER_LIGHTING_FEATURE_NORMAL_MAPPING)
//create TBN-matrix in fragment shader due to the several lightsources;
//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
in vec4 tangentInWorldCoords;
#endif

#if (defined(SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER) \
  && (LIGHT_SOURCES_SHADOW_FEATURE == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT  ) )
in vec4 shadowCoord;
#endif

//}
//------------------------------------------------------------------------------------------------
//section 8: Fragment outputs
//{%shaderSpecificOutput}
//{
out vec4 finalLuminance;
//}


//------------------------------------------------------------------------------------------------
//section 9: subroutines (atm not in the GL4-style ;()
//{%subroutines}
//{
#ifdef SHADER_FEATURE_NORMAL_MAPPING
vec3 getNormalMappedNormal(vec3 normalWN)
{
	vec3 nonPerturbedNormal = texture(normalMap,texCoords.xy).xyz;

	vec3 tangentWN = normalize(tangentInWorldCoords);
	vec3 binormalWN = cross(normalWN,tangentWN);
	mat3 TBNMatrix(tangentWN,binormalWN,normalWN);

	return TBNMatrix * nonPerturbedNormal;
}
#endif

#	ifdef SHADER_FEATURE_SHADOWING
float getShadowAttenuation(int lightSourceIndex)
{
	float shadowAtt = minimalshadowAttenuation;

#	ifndef SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER
	vec4 shadowCoord =positionInWorldCoords * worldToShadowMapMatrix;
#	endif
	//divide by homogene coord:
	shadowCoord /= shadowCoord.w;
	//we don't want a squared shadow-throwing lightsource impression, but a circled one:	
	if( length(vec2(0.5 + shadowCoord.x, 0.5  shadowCoord.y)) < 0.5 )
	{
		shadowAtt = clamp(texture(shadowMap, shadowCoord.xyz),minimalshadowAttenuation , 1.0);	
	}
}
#	endif //SHADER_FEATURE_SHADOWING

//}


void main()
{
	finalLuminance = vec4(0.0,0.0,0.0,0.0);

	vec4 fragmentColor =
#		ifdef  SHADER_FEATURE_DECAL_TEXTURING
		texture(decalTexture,texCoords.xy);
#		else
		vec4(1.0,1.0,1.0,1.0);
#		endif

#	ifdef SHADER_FEATURE_LIGHTING
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#	ifdef SHADER_FEATURE_SHADOWING
	float shadowAttenuation = minimalshadowAttenuation;

#	ifndef SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER
	vec4 shadowCoord =positionInWorldCoords * worldToShadowMapMatrix;
#	endif
	//divide by homogene coord:
	shadowCoord /= shadowCoord.w;
	//we don't want a squared shadow-throwing lightsource impression, but a circled one:	
	if( length(vec2(0.5 + shadowCoord.x, 0.5  shadowCoord.y)) < 0.5 )
	{
		shadowAttenuation = clamp(texture(shadowMap, shadowCoord.xyz),minimalshadowAttenuation , 1.0);	
	}
#	endif //SHADER_FEATURE_SHADOWING
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	vec3 normalWN = normalize(normalInWorldCoords);
#	ifdef SHADER_FEATURE_NORMAL_MAPPING	
	//perturb normal
	normalWN = getNormalMappedNormal(normalWN);
#	endif

	vec3 fragToCamN = normalize(eyeVecInWorldCoords - positionInWorldCoords);

#	ifdef SHADER_FEATURE_MANY_LIGHTSOURCES
	for(int lightIndex = 0; lightIndex < NUM_LIGHTSOURCES ;lightIndex++);
	{
		//lets hope that there will be a component wise copy and NOT some C++-f***up about non-existing operator=() ;)
		LightSource lightSource = lightSources[lightIndex];
#	endif

		vec3 lightToFrag =   positionInWorldCoords - lightSource.position;
		vec3 lightToFragN = normalize(lightToFrag);

		float cosFragToLight_Normal = dot( (-1.0) * lightToFragN , normalWN);

		//is fragment facing to the light?
		if(cosFragToLight_Normal > 0)
		{
			//If the fragment is out of the light cone (test see below):
			//just leave the diffuse factor as the ambient factor;
			
			float diffuseFactor  = cosFragToLight_Normal;
			//diffuseFactor = clamp(diffuseFactor, lightSourceAmbientFactor, 1.0 );

			vec3 reflectedLightToFrag = reflect ( lightToFragN , normalWN);
			float cosFragToCam_reflectedLightToFrag = dot(fragToCamN,reflectedLightToFrag);
			float specularFactor = pow(cosFragToCam_reflectedLightToFrag, shininess);

			/*	distanceAttenuation formula hopefully more performant than the following:
				distanceAttenuation = 1.0 / (length(fragToLight)* length(fragToLight));
				(first take a sqrt, than take the reciprocal of its sqare... this is too much overhead :P)	*/
			float distanceAttenuation = 1.0 / ( 
				(fragToLight.x * fragToLight.x) + (fragToLight.y * fragToLight.y) + (fragToLight.z * fragToLight.z));


#			ifdef SHADER_FEATURE_USE_SPOTLIGHTS
			float spotLightConeAttenuation = 1.0;

			float cosLightDir_LightToFrag = dot( lightSource.direction , lightToFragN );
			float angleLightDir_LightToFrag_Radians = acos(cosLightDir_LightToFrag);	

			//is fragment within light cone?
			if (angleLightDir_LightToFrag_Radians <= lightSource.outerSpotCutOff_Radians)
			{
				spotLightConeAttenuation = pow(cosLightDir_LightToFrag, lightSource.spotExponent );
	
				//Does the fragment lie within the border region of the cone?
				//Then smooth out the lights' contribution by linear interpolating between lightSourceAmbientFactor and 1, 
				//and multiplicate this to spotLightConeAttenuation
				if (angleLightDir_LightToFrag_Radians > lightSource.innerSpotCutOff_Radians)
				{
					float fractionWithinBorder = 	
						(angleLightDir_LightToFrag_Radians - lightSource.innerSpotCutOff_Radians) 
						/ 
						(lightSource.outerSpotCutOff_Radians - lightSource.innerSpotCutOff_Radians);

					spotLightConeAttenuation *= mix(1.0, lightSourceAmbientFactor, fractionWithinBorder);
				}
			}
			else
			{
				spotLightConeAttenuation = lightSourceAmbientFactor;
			}
#			endif	//SHADER_FEATURE_USE_SPOTLIGHTS

	
			finalLuminance + =
				distanceAttenuation * 
#			ifdef SHADER_FEATURE_SHADOWING
				shadowAttenuation * 
#			endif
#			ifdef SHADER_FEATURE_USE_SPOTLIGHTS
				spotLightConeAttenuation *
#			endif	
				fragmentColor *
				(
					( vec4(lightSource.diffuseColor,1.0) *  diffuseFactor ) +
					( vec4(lightSource.specularColor,1.0) *  specularFactor )
				);

		} //endif(cosFragToLight_Normal > 0)	

#	ifdef SHADER_FEATURE_MANY_LIGHTSOURCES
	} //end of for-loop
#	endif

#	ifdef SHADER_FEATURE_CUBE_MAPPING
	//lerp between actual color and cubemap color
	finalLuminance = mix( 	finalLuminance, 
				texture(cubeMap, normalWN),
				cubeMapReflectivity );
#	endif


#	else 	//SHADER_FEATURE_LIGHTING
	finalLuminance = fragmentColor;
#	endif 	//SHADER_FEATURE_LIGHTING

}
