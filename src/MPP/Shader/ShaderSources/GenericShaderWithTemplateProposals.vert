//A first concept of a vertex shader template with example filled code

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

//----------------------------------------------------------------------------------------------------

//section 3: Data type definitions:
//{%typeDefinitions}
//{

//}

//----------------------------------------------------------------------------------------------------


//section 4:  uniform buffers:
//{%uniformBuffers}
//{

#ifdef SHADER_FEATURE_INSTANCING
layout(shared) uniform ModelMatrixBuffer
{
	mat4 modelMatrices[NUM_INSTANCES];
	//as long you do nothing else but rotate, translate and homogene scale, normalMatrices[i]==modelMatrices[i];
	//but do we dare to rely on this?
	mat4 normalMatrices[NUM_INSTANCES];	
}
#endif

//}

//section 5: standard uniforms:
//{%standardUniforms}
//{
#ifndef SHADER_FEATURE_INSTANCING
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
//uniform mat4 modelViewMatrix;
//uniform mat4 modelViewProjectionMatrix;
#endif

//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;
uniform mat4 viewProjectionMatrix;

//}


//section 6:  uniform samplers:
//{%uniformSamplers}
//{

//normally, there is no texturing in the vertex shader;

//}


//----------------------------------------------------------------------------------------------------


//section 7: shader specific input:
//{%vertShaderInput}
//{
layout(location = POSITION_SEMANTICS ) 	in vec4 inVPosition;
layout(location = NORMAL_SEMANTICS ) 	in vec4 inVNormal;
#ifdef SHADER_FEATURE_NORMAL_MAPPING
layout(location = TANGENT_SEMANTICS ) 	in vec4 inVTangent;
#endif

#ifdef SHADER_FEATURE_DECAL_TEXTURING
layout(location = TEXCOORD_SEMANTICS ) 	in vec4 inVTexCoord;
#endif

//future use when doing fluid stuff
#if 0
layout(location = VELOCITY_SEMANTICS ) 	in vec4 inVVelocity;
layout(location = MASS_SEMANTICS ) 	in vec4 inVMass;
layout(location = DENSITY_SEMANTICS ) 	in vec4 inVDensity;
layout(location = PRESSURE_SEMANTICS ) 	in vec4 invPressure;
layout(location = FORCE_SEMANTICS ) 	in vec4 inVForce;
layout(location = Z_INDEX_SEMANTICS ) 	in vec4 inVZIndex;
#endif

//}

//----------------------------------------------------------------------------------------------------


//section 8:  shader specific output:
//{%vertShaderOutput}
//{
in vec4 positionInWorldCoords;
in vec4 normalInWorldCoords;
in vec4 texCoords;
#ifdef SHADER_FEATURE_NORMAL_MAPPING
//create TBN-matrix in fragment shader due to the several lightsources;
//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
out vec4 biNormalInWorldCoords;

//}

//----------------------------------------------------------------------------------------------------



void main()
{

}
