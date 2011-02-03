//Second concept of a fragment shader template


//{%versionTag}

//{%precisionTag}

//{%persistentdefines}
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//section 3: Data type definitions:
//{%typeDefinitions}
//{

//}
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//section 4:  uniform buffers:
//{%uniformBuffers}
//{

#ifdef SHADER_FEATURE_INSTANCING
layout(shared) uniform ModelMatrixBuffer
{
	mat4 modelMatrices[NUM_MAX_INSTANCES];
	//as long you do nothing else but rotate, translate and homogene scale, normalMatrices[i]==modelMatrices[i];
	//but do we dare to rely on this?
	mat4 normalMatrices[NUM_MAX_INSTANCES];	
}
#endif

//}
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
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

#if (defined(SHADER_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER) && defined(SHADER_FEATURE_SHADOWING) )
uniform mat4 worldToShadowMapMatrix; //bias*perspLight*viewLight
#endif
//}
//----------------------------------------------------------------------------------------------------

//section 6:  uniform samplers:
//{%uniformSamplers}
//{
//normally, there is no texturing in the vertex shader;
//}
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//section 7: shader specific input:
//{%shaderSpecificInput}
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
layout(location = DECAL_COLOR_SEMANTICS) in vec4 inVDecalColor;
layout(location = CUSTOM_SEMANTICS ) 	in vec4 inVCustomAttribute;
#endif

//}
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//section 8:  shader specific output:
//{%shaderSpecificOutput}
//{
out vec4 positionInWorldCoords;

out vec4 normalInWorldCoords;
#ifdef SHADER_FEATURE_DECAL_TEXTURING
out vec4 texCoords;
#endif
#ifdef SHADER_FEATURE_NORMAL_MAPPING
//create TBN-matrix in fragment shader due to the several lightsources;
//we transform from tangent space to view space via putting the vectors column-wise into a 3x3-matrix: mat3(t,b,n);
out vec4 tangentInWorldCoords;
#endif

#ifdef GBUFFER_INDEX_RENDERING
out ivec4 genericIndices;
#endif

#if (defined(SHADER_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER) && defined(SHADER_FEATURE_SHADOWING) )
out vec4 shadowCoord;
#endif
//}
//----------------------------------------------------------------------------------------------------



void main()
{
#	ifdef SHADER_FEATURE_INSTANCING
	mat4 modelMatrix = modelMatrices[gl_InstanceID];

	mat4 normalMatrix = normalMatrices[gl_InstanceID];
#	endif

	positionInWorldCoords =  	modelMatrix * inVPosition;
	normalInWorldCoords = 		normalMatrix * inVNormal;

#	ifdef SHADER_FEATURE_NORMAL_MAPPING
	tangentInWorldCoords = 		normalMatrix * inVTangent;
#	endif

#	ifdef SHADER_FEATURE_DECAL_TEXTURING
	texCoords = inVTexCoord;
#	endif

#if 	(defined(SHADER_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER) && defined(SHADER_FEATURE_SHADOWING) )
	shadowCoord = worldToShadowMapMatrix *  positionInWorldCoords;
#	endif

	gl_Position = viewProjectionMatrix * positionInWorldCoords;

}
