/*
 * Math.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

//ok, the math stuff is too depp in code to wrap it, i'll take glm now, without any fallback to switch to another lib easily ;)
////--------------------------------------------------------------
//#if FLEWNIT_USE_QT_MATH
//
//#	include <QVector2D>
//#	include <QVector3D>
//#	include <QVector4D>
//
//
//#	include <QMatrix3x3>
//#	include <QMatrix4x4>
//
//namespace Flewnit
//{
//	typedef QVector2D Vector2D;
//	typedef QVector3D Vector3D;
//	typedef QVector4D Vector4D;
//
//	typedef QMatrix3x3 Matrix3x3;
//	typedef QMatrix4x4 Matrix4x4;
//
//	//typedefs not complete, but I don't plan to use QT math anyway; just if glm should suck inexpectedly...
//}
//
//
////--------------------------------------------------------------
//#elif FLEWNIT_USE_GLM_MATH

#	include  <glm/setup.hpp>
//no swizzling for the moment
//#	define GLM_SWIZZLE  GLM_SWIZZLE_FULL
#	include  <glm/glm.hpp>

//extensions:
#	include <glm/gtc/matrix_transform.hpp>
#	include <glm/gtc/matrix_projection.hpp>
#	include <glm/gtc/quaternion.hpp>

namespace Flewnit
{
	typedef glm::vec2 Vector2D;
	typedef glm::vec3 Vector3D;
	typedef glm::vec4 Vector4D;

	typedef glm::ivec2 Vector2Di;
	typedef glm::ivec3 Vector3Di;
	typedef glm::ivec4 Vector4Di;

	typedef glm::uvec2 Vector2Dui;
	typedef glm::uvec3 Vector3Dui;
	typedef glm::uvec4 Vector4Dui;


	typedef glm::mat3x3 Matrix3x3;
	typedef glm::mat4x4 Matrix4x4;

	typedef glm::quat Quaternion;
}

//--------------------------------------------------------------
//#endif //FLEWNIT_USE_QT_MATH


namespace Flewnit
{
	class AABB
	{
	public:
		AABB(): mMin(0),mMax(0){}
		AABB(const Vector4D min, const Vector4D max  ): mMin(min),mMax(max){}

		inline const Vector4D& getMin()const{return mMin;}
		inline const Vector4D& getMax()const{return mMax;}

		void include(const AABB& other);
		void include(const Vector4D& other);

		void reset();
	private:
		Vector4D mMin;
		Vector4D mMax;
	};
}

