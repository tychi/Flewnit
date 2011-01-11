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
	vec4 position;
	vec4 diffuseColor;
	vec4 specularColor;

#ifdef SHADER_FEATURE_USE_SPOTLIGHTS
	//might not be needed, but you never know, 
	vec4 direction;
	float spotCosCutoff;
	float spotExponent;
	//alignment is everything :P
	float pad0;
	float pad1;
#endif

};
//}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//section 4:  uniform buffers:
//{%uniformBuffers}
//{
#ifdef SHADER_FEATURE_MANY_LIGHTSOURCES
layout(shared) uniform LightSourceBuffer
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
#ifndef SHADER_FEATURE_MANY_LIGHTSOURCES
uniform LightSource lightSource;
#endif

#ifdef	SHADER_FEATURE_SHADOWING
uniform mat4 camViewToShadowMapMatrix; //bias*perspLight*viewLight*(viewCam⁻1)
#endif

#ifdef SHADER_FEATURE_LIGHTING
uniform float shininess = 1.0;
#endif
//}


//section 6:  uniform samplers:
//{%uniformSamplers}
//{
#ifdef SHADER_FEATURE_SHADOWING
uniform sampler2DShadow shadowMap;
#endif

#ifdef SHADER_FEATURE_NORMAL_MAPPING
uniform sampler2D normalMap;
#endif

//}
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//section 7: Fragment inputs from Vertex/Geom/Tesseleation evaluation shader
//{%}
//{
in vec4 positionInViewCoords;
in vec4 normalInViewCoords;
in vec4 texCoords;
#ifdef SHADER_FEATURE_NORMAL_MAPPING
//create TBN-matrix in fragment shader due to the several lightsources;
//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
in vec4 biNormalInViewCoords;
#endif
//}
//------------------------------------------------------------------------------------------------
//section 8: Fragment outputs
//{%}
//{
out vec4 finalColor;
//}
