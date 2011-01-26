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


namespace Flewnit
{


class BufferHelper
{
public:
	static bool isPowerOfTwo(int value);

	static String BufferSemanticsToString(BufferSemantics bs);

	static size_t elementSize(Type type)
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
};

}

