/*
 * BufferHelperUtils.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Math.h"
#include "Buffer/BufferSharedDefinitions.h"

#include <typeinfo>
#include "Common/BasicObject.h"


namespace Flewnit
{




class BufferHelper
{
public:
	static bool isPowerOfTwo(int value);

	static String BufferSemanticsToString(BufferSemantics bs);

	static size_t elementSize(Type type);


};


class UniformBufferMetaInfo
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	UniformBufferMetaInfo(	GLint numMaxArrayElements, String bufferName, String arrayName, std::vector<String> memberStrings, Shader* queryShader);
	virtual ~UniformBufferMetaInfo();

	GLint	mNumArrayElements; //store this value to be independent of ShaderManager deletion (critical when engine shuts down)
	//String 	mArrayName;
	std::vector<String> mMemberStrings;

	GLint 	mRequiredBufferSize;
	GLint	mNumMemberElements;
	GLint** mBufferOffsets;
};

template<typename T>
class TypeInitializer
{
public:
	Type getTypeEnum()const
	{
			//not all types define in teh Type-enum are checked, but only the most common;

			if(typeid(T) == typeid(bool))
				return TYPE_BOOL;

			if(typeid(T) == typeid(signed char))
				return TYPE_INT8;
			//to be sure without "signed" tag:
			if(typeid(T) == typeid(char))
				return TYPE_INT8;

			if(typeid(T) == typeid(unsigned char))
				return TYPE_UINT8;

			if(typeid(T) == typeid(short))
				return TYPE_INT16;
			if(typeid(T) == typeid(unsigned short))
				return TYPE_UINT16;
			if(typeid(T) == typeid(int))
				return TYPE_INT32;
			if(typeid(T) == typeid(unsigned int))
				return TYPE_UINT32;
			if(typeid(T) == typeid(long))
				return TYPE_INT64;
			if(typeid(T) == typeid(long))
				return TYPE_INT64;
			if(typeid(T) == typeid(unsigned long))
				return TYPE_UINT64;
			if(typeid(T) == typeid(float))
				return TYPE_FLOAT;
			if(typeid(T) == typeid(double))
				return TYPE_DOUBLE;
			if(typeid(T) == typeid(Vector2D))
				return TYPE_VEC2F;
			if(typeid(T) == typeid(Vector3D))
				return TYPE_VEC3F;
			if(typeid(T) == typeid(Vector4D))
				return TYPE_VEC4F;

			if(typeid(T) == typeid(Vector2Di))
				return TYPE_VEC2I32;
			if(typeid(T) == typeid(Vector3Di))
				return TYPE_VEC3I32;
			if(typeid(T) == typeid(Vector4Di))
				return TYPE_VEC4I32;

			if(typeid(T) == typeid(Vector2Dui))
				return TYPE_VEC2UI32;
			if(typeid(T) == typeid(Vector3Dui))
				return TYPE_VEC3UI32;
			if(typeid(T) == typeid(Vector4Dui))
				return TYPE_VEC4UI32;

			//-----------------------
			//smaller types, for textures etc; experimental!
			///\{
			if(typeid(T) == typeid(Vector2D8i))
				return TYPE_VEC2I8;
			if(typeid(T) == typeid(Vector2D8ui))
				return TYPE_VEC2UI8;

			if(typeid(T) == typeid(Vector3D8i))
				return TYPE_VEC3I8;
			if(typeid(T) == typeid(Vector3D8ui))
				return TYPE_VEC3UI8;

			if(typeid(T) == typeid(Vector4D8i))
				return TYPE_VEC4I8;
			if(typeid(T) == typeid(Vector4D8ui))
				return TYPE_VEC4UI8;

			if(typeid(T) == typeid(Vector2D16i))
				return TYPE_VEC2I16;
			if(typeid(T) == typeid(Vector2D16ui))
				return TYPE_VEC2UI16;

			if(typeid(T) == typeid(Vector3D16i))
				return TYPE_VEC3I16;
			if(typeid(T) == typeid(Vector3D16ui))
				return TYPE_VEC3UI16;

			if(typeid(T) == typeid(Vector4D16i))
				return TYPE_VEC4I16;
			if(typeid(T) == typeid(Vector4D16ui))
				return TYPE_VEC4UI16;

			if(typeid(T) == typeid(Half))
				return TYPE_HALF_FLOAT;
			if(typeid(T) == typeid(Vector2Dhalf))
				return TYPE_VEC2F16;
			if(typeid(T) == typeid(Vector3Dhalf))
				return TYPE_VEC3F16;
			if(typeid(T) == typeid(Vector4Dhalf))
				return TYPE_VEC4F16;
			///\}
			//-----------------------

			if(typeid(T) == typeid(Matrix3x3))
				return TYPE_MATRIX33F;
			if(typeid(T) == typeid(Matrix4x4))
				return TYPE_MATRIX44F;
			if(typeid(T) == typeid(Quaternion))
				return TYPE_QUAT4F;

			if(typeid(T) == typeid(String))
				return TYPE_STRING;



			//default
			return  TYPE_UNDEF;
	}
};

}

