/*
 * BufferHelperUtils.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Math.h"

#include <typeinfo>

namespace Flewnit
{


class BufferHelper
{
public:
	static size_t elementSize(Type type)
	{
		//not all types define in teh Type-enum are checked, but only the most common;

		if(type == TYPE_BOOL)
			return sizeof(bool);
		if(type == TYPE_CHAR)
			return sizeof(unsigned char);
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

		if(		type == TYPE_PACKED_VEC3_AS_UINT16 ||
				type == TYPE_PACKED_VEC4_AS_UINT16)
			return sizeof(unsigned short);

		if(		type == TYPE_PACKED_VEC3_AS_INT16 ||
				type == TYPE_PACKED_VEC4_AS_INT16)
			return sizeof(short);

		if(		type == TYPE_PACKED_VEC3_AS_UINT32 ||
				type == TYPE_PACKED_VEC4_AS_UINT32)
			return sizeof(unsigned int);

		if(		type == TYPE_PACKED_VEC3_AS_INT32 ||
				type == TYPE_PACKED_VEC4_AS_INT32)
			return sizeof(int);

		//default
		assert("no mathichn type identifier" && 0);
		return 0;
	}
};

}

