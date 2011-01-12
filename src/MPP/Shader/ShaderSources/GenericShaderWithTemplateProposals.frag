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
//
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

//position and normal are always used;
//the rest must be explicitely enabled

#define SHADER_FEATURE_LIGHTING
#define SHADER_FEATURE_USE_SPOTLIGHTS
#define SHADER_FEATURE_MANY_LIGHTSOURCES

#define	SHADER_FEATURE_SHADOWING
#define SHADER_FEATURE_NORMAL_MAPPING
#define SHADER_FEATURE_CUBE_MAPPING

#define SHADER_FEATURE_DECAL_TEXTURING


#define SHADER_FEATURE_INSTANCING
#define NUM_INSTANCES 4
#define NUM_LIGHTSOURCES 4

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
	vec3 direction;
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;
	float outerSpotCutOff_Radians;
	float spotExponent;
	//alignment is everything :P
	//value in [0..1] at which the attenuation factor due to shadowing is clamped
	float ambientFactor;

	//align to 64 bytes;
	//float pad0;
};
//}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//section 4:  uniform buffers:
//{%uniformBuffers}
//{
#ifdef SHADER_FEATURE_MANY_LIGHTSOURCES
layout(shared) uniform lightSourceBuffer
{
	LightSource lightSources[NUM_LIGHTSOURCES];
};
#endif
//}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//section 5: standard uniforms:
//{%standardUniforms}
//{
uniform vec4 eyeVecInWorldCoords;

#ifndef SHADER_FEATURE_MANY_LIGHTSOURCES
uniform LightSource lightSource;
#endif

#ifdef	SHADER_FEATURE_SHADOWING
uniform mat4 WorldToShadowMapMatrix; //bias*perspLight*viewLight
//clamp the attenuation due to shadowmapping to [minimalshadowAttenuation, 1.0]
uniform float minimalshadowAttenuation = 0.2;
#endif


#ifdef SHADER_FEATURE_LIGHTING
uniform float shininess = 1.0;
#endif
#ifdef SHADER_FEATURE_CUBE_MAPPING
uniform float cubeMapReflectivity = 0.5;
#endif
//}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//section 6:  uniform samplers:
//{%uniformSamplers}
//{
#ifdef SHADER_FEATURE_LIGHTING
uniform sampler2D decalTexture;
#endif

#ifdef SHADER_FEATURE_SHADOWING
uniform sampler2DShadow shadowMap;
#endif

#ifdef SHADER_FEATURE_NORMAL_MAPPING
uniform sampler2D normalMap;
#endif

#ifdef SHADER_FEATURE_CUBE_MAPPING
uniform samplerCube cubeMap;
#endif
//}
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//section 7: Fragment inputs from Vertex/Geom/Tesseleation evaluation shader
//{%}
//{
in vec4 positionInWorldCoords;
in vec4 normalInWorldCoords;
in vec4 texCoords;
#ifdef SHADER_FEATURE_NORMAL_MAPPING
//create TBN-matrix in fragment shader due to the several lightsources;
//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
in vec4 tangentInWorldCoords;
#endif
//}
//------------------------------------------------------------------------------------------------
//section 8: Fragment outputs
//{%}
//{
out vec4 finalLuminance;
//}


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
	vec3 fragToCamN = normalize(eyeVecInWorldCoords - positionInWorldCoords);

	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#	ifdef SHADER_FEATURE_SHADOWING
	float shadowAttenuation = minimalshadowAttenuation;
	vec4 shadowCoord =positionInWorldCoords * WorldToShadowMapMatrix;
	//divide by homogene coord:
	shadowCoord /= shadowCoord.w;
	//we don't want a squared shadow-throwing lightsource impression, but a circled one:	
	if( length(vec2(shadowCoord.x, shadowCoord.y)) < 1.0 )
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
			//diffuseFactor = clamp(diffuseFactor, lightSource.ambientFactor, 1.0 );

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
				//Then smooth out the lights' contribution by linear interpolating between ambientFactor and 1, 
				//and multiplicate this to spotLightConeAttenuation
				if (angleLightDir_LightToFrag_Radians > lightSource.innerSpotCutOff_Radians)
				{
					float fractionWithinBorder = 	
						(angleLightDir_LightToFrag_Radians - lightSource.innerSpotCutOff_Radians) 
						/ 
						(lightSource.outerSpotCutOff_Radians - lightSource.innerSpotCutOff_Radians);

					spotLightConeAttenuation *= mix(1.0, ambientFactor, fractionWithinBorder);
				}
			}
			else
			{
				spotLightConeAttenuation = lightSource.ambientFactor;
			}
#			endif	//SHADER_FEATURE_USE_SPOTLIGHTS

	
			finalLuminance + =
				distanceAttenuation * 
#			ifdef SHADER_FEATURE_SHADOWING
				shadowAttenuation * 
#			endif
#			ifdef SHADER_FEATURE_USE_SPOTLIGHTS
				spotLightConeAttenuation
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
