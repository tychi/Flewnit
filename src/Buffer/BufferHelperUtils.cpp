/*
 * BufferHelperUtils.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#include "BufferHelperUtils.h"
#include "MPP/Shader/Shader.h"
#include "Util/Log/Log.h"
#include "Util/HelperFunctions.h"
#include "Simulator/ParallelComputeManager.h"

#include "Common/CL_GL_Common.h"
//#include "../../include/GL3/gl3.h"



namespace Flewnit
{

bool BufferHelper::isPowerOfTwo(int value)
{
	if(value <0 ) return false;

	int cnt=0;
	for (unsigned int i=0;i< sizeof(int)*8 ; i++)
	{
		if( ( value & (1<<i) ) !=0) cnt++;
	}
	return (cnt <= 1);
}


size_t BufferHelper::elementSize(Type type)
	{
		//not all types define in teh Type-enum are checked, but only the most common;


		if(type == TYPE_BOOL)
			return sizeof(bool);

		if(type == TYPE_UINT8)
			return sizeof(unsigned char);
		if(type == TYPE_INT8)
			return sizeof(signed char);

		if(type == TYPE_INT16)
			return sizeof(short);
		if(type == TYPE_UINT16)
			return sizeof(unsigned short);
		if(type == TYPE_INT32)
			return sizeof(int) ;
		if(type == TYPE_UINT32)
			return sizeof(unsigned int);
		if(type == TYPE_INT64 )
			return sizeof(long) ;
		if(type == TYPE_UINT64 )
			return sizeof(unsigned long);
		if(type == TYPE_FLOAT)
			return  sizeof(float);
		if(type == TYPE_DOUBLE )
			return sizeof(double);
		if(type == TYPE_VEC2F )
			return sizeof(Vector2D);
		if(type == TYPE_VEC3F)
			return  sizeof(Vector3D);
		if(type == TYPE_VEC4F )
			return sizeof(Vector4D);

		if(type == TYPE_VEC2I32 )
			return sizeof(Vector2Di);
		if(type == TYPE_VEC3I32 )
			return sizeof(Vector3Di);
		if(type == TYPE_VEC4I32 )
			return sizeof(Vector4Di);

		if(type == TYPE_VEC2UI32 )
			return sizeof(Vector2Dui);
		if(type == TYPE_VEC3UI32 )
			return sizeof(Vector3Dui);
		if(type == TYPE_VEC4UI32)
			return  sizeof(Vector4Dui);

		if(type == TYPE_MATRIX33F )
			return sizeof(Matrix3x3);
		if(type == TYPE_MATRIX44F )
			return sizeof(Matrix4x4);
		if(type == TYPE_QUAT4F )
			return sizeof(Quaternion);

		if(type == TYPE_STRING )
			return sizeof(String);

		//-----------------------
		//smaller types, for textures etc; experimental!
		///\{
		if(type == TYPE_VEC2I8 )
			return sizeof(Vector2D8i);
		if(type == TYPE_VEC2UI8 )
			return sizeof(Vector2D8ui);

		if(type == TYPE_VEC3I8 )
			return sizeof(Vector3D8i);
		if(type == TYPE_VEC3UI8 )
			return sizeof(Vector3D8ui);

		if(type == TYPE_VEC4I8 )
			return sizeof(Vector4D8i);
		if(type == TYPE_VEC4UI8 )
			return sizeof(Vector4D8ui);

		if(type == TYPE_VEC2I16 )
			return sizeof(Vector2D16i);
		if(type == TYPE_VEC2UI16 )
			return sizeof(Vector2D16ui);

		if(type == TYPE_VEC3I16 )
			return sizeof(Vector3D16i);
		if(type == TYPE_VEC3UI16 )
			return sizeof(Vector3D16ui);

		if(type == TYPE_VEC4I16 )
			return sizeof(Vector4D16i);
		if(type == TYPE_VEC4UI16 )
			return sizeof(Vector4D16ui);


		if(type == TYPE_HALF_FLOAT )
			return sizeof(Half);
		if(type == TYPE_VEC2F16 )
			return sizeof(Vector2Dhalf);
		if(type == TYPE_VEC3F16 )
			return sizeof(Vector3Dhalf);
		if(type == TYPE_VEC4F16 )
			return sizeof(Vector4Dhalf);
		///\}
		//-----------------------

		//default
		assert("no matchtinh yet supported type identifier" && 0);
		return 0;
}



String BufferHelper::BufferSemanticsToString(BufferSemantics bs)
{
	static const String BufferSemanticsStrings[] =
	{
			// following semantics generally used in
			//Generic Vertex Attribute Buffers and/or OpenCL (interop) Buffers
			///\{
			"POSITION_SEMANTICS",
			"NORMAL_SEMANTICS",
			"TANGENT_SEMANTICS",
			//"BINORMAL_SEMANTICS",
			"TEXCOORD_SEMANTICS",

			"VELOCITY_SEMANTICS",
			"MASS_SEMANTICS",
			"DENSITY_SEMANTICS",
			"PRESSURE_SEMANTICS",
			"FORCE_SEMANTICS",

			"Z_INDEX_SEMANTICS",

			"DIFFUSE_COLOR_SEMANTICS",

			"CUSTOM_SEMANTICS",

			//we need tha value to have static-length arrays holding VBO maintainance information
			"__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__",

			//Semantic of the index buffer from a VBO used via glDrawElements()
			"INDEX_SEMANTICS",

			//for a uniform buffer for matrices of instanced rendering
			"TRANSFORMATION_MATRICES_SEMANTICS",
			"LIGHT_SOURCE_BUFFER_SEMANTICS",
			///\}

			//following texture-only semantics; Texture can also have the above Semantics

			"DISPLACEMENT_SEMANTICS", //normal-depth or normal map
			"ENVMAP_SEMANTICS",
			"MATERIAL_ID_SEMANTICS",
			"PRIMITIVE_ID_SEMANTICS",
			"SHADOW_MAP_SEMANTICS",
			"AMBIENT_OCCLUSION_SEMANTICS",
			"DETAIL_TEXTURE_SEMANTICS", //for stuff like terrain to hide low res decal texture
			"NOISE_SEMANTICS",
			"DEPTH_BUFFER_SEMANTICS",
			"STENCIL_BUFFER_SEMANTICS",
			"INTERMEDIATE_RENDERING_SEMANTICS",
			"FINAL_RENDERING_SEMANTICS",

			//actually real amount is one less than this valu, but that doesnt matter
			"__NUM_TOTAL_SEMANTICS__" ,

			//indicator for "empty" stuff, e.g. an empty Color Attachment slot in an FBO
			"INVALID_SEMANTICS"
	};

	return BufferSemanticsStrings[bs];
}


//-----------------------------------------------------------------------------

UniformBufferMetaInfo::UniformBufferMetaInfo( GLint numMaxArrayElements, String bufferName, String arrayName,
		std::vector<String> memberStrings, Shader* queryShader)
:
	mNumArrayElements(numMaxArrayElements),
	//mArrayName(arrayName),
	mMemberStrings(memberStrings),
	mRequiredBufferSize(0),
	mNumMemberElements(memberStrings.size() == 0 ? 1 : memberStrings.size() ),
	mBufferOffsets(0)
{
	LOG<<DEBUG_LOG_LEVEL<<bufferName<<"\n";
	GLuint shaderGLProgramHandle = queryShader->getGLProgramHandle();
	GLuint uniBlockIndex = GUARD( glGetUniformBlockIndex(shaderGLProgramHandle, bufferName.c_str()) );
	//query needed buffer size
	GUARD(
		glGetActiveUniformBlockiv(
			shaderGLProgramHandle,
			uniBlockIndex,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			& mRequiredBufferSize
		)
	);

	//--------------------------------------------------------------------------------

	mBufferOffsets = new GLint*[mNumArrayElements];

//	const String memberStrings[] =
//		{
//		  "position","diffuseColor","specularColor","direction",
//		  "innerSpotCutOff_Radians","outerSpotCutOff_Radians","spotExponent","shadowMapLayer"
//		};

	const char** indexQuery_C_StringArray= new const char*[mNumMemberElements];
	String* indexQueryStringArray= new String[mNumMemberElements];
	GLuint* currentUniformIndices= new GLuint[mNumMemberElements];
	for(int arrayElementRunner=0; arrayElementRunner< mNumArrayElements; arrayElementRunner++)
	{
		String baseString =
			arrayName +
			String("[")
				+ HelperFunctions::toString(arrayElementRunner)
			+ String("]") ;


			mBufferOffsets[arrayElementRunner]= new GLint[mNumMemberElements];
			if(memberStrings.size() != 0)
			{
				//we have a structure as array elements; construct the GL referencation strings:
				for(int memberRunner=0; memberRunner< mNumMemberElements; memberRunner++)
				{
					indexQueryStringArray[memberRunner]= String(baseString+ String(".") + memberStrings[memberRunner]);
					indexQuery_C_StringArray[memberRunner]= indexQueryStringArray[memberRunner].c_str();
				}
			}
			else
			{
				//the array element constist of a single built-in type, i.e. the GL referencation strings
				//are a single string, beeing the base string:
				indexQuery_C_StringArray[0]= baseString.c_str();
			}

			//first, get indices of current lightsource members:
			GUARD(
				glGetUniformIndices(
					shaderGLProgramHandle,
					mNumMemberElements,
					indexQuery_C_StringArray,
					currentUniformIndices
				)
			);

			//second, get offset in buffer for those members, indentified by the queried indices:
			GUARD(
				glGetActiveUniformsiv(
					shaderGLProgramHandle,
					mNumMemberElements,
					currentUniformIndices,
					GL_UNIFORM_OFFSET,
					mBufferOffsets[arrayElementRunner]
				)
			);


//			for(int memberRunner=0; memberRunner< mNumMemberElements; memberRunner++)
//			{
//				LOG<<DEBUG_LOG_LEVEL << String(indexQuery_C_StringArray[memberRunner])<<" ;\n";
//				LOG<<DEBUG_LOG_LEVEL <<"uniform index: "<<  currentUniformIndices[memberRunner] <<" ;\n";
//				LOG<<DEBUG_LOG_LEVEL <<"uniform offset: "<<  mBufferOffsets[arrayElementRunner][memberRunner] <<" ;\n";
//				assert( "member should be active in shader, otherwise uniform buffer filling would turn out even more complicated :@"
//						&&  ( currentUniformIndices[memberRunner] != GL_INVALID_INDEX) );
//			}
	} //endfor
	delete[] indexQuery_C_StringArray;
	delete[] indexQueryStringArray;
	delete[] currentUniformIndices;
}

UniformBufferMetaInfo::~UniformBufferMetaInfo()
{
	for(int elementRunner=0; elementRunner< mNumArrayElements; elementRunner++)
	{
		delete[] mBufferOffsets[elementRunner];
	}
	delete[] mBufferOffsets;
}

}
