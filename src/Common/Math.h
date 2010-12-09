/*
 * Math.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

//--------------------------------------------------------------
#if FLEWNIT_USE_QT_MATH

#	include <QVector2D>
#	include <QVector3D>
#	include <QVector4D>


#	include <QMatrix3x3>
#	include <QMatrix4x4>

namespace Flewnit
{
	typedef QVector2D Vector2D;
	typedef QVector3D Vector3D;
	typedef QVector4D Vector4D;

	typedef QMatrix3x3 Matrix3x3;
	typedef QMatrix4x4 Matrix4x4;
}


//--------------------------------------------------------------
#elif FLEWNIT_USE_GLM_MATH

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
#endif //FLEWNIT_USE_QT_MATH

namespace Flewnit
{


class Math
{
public:
	Math();
	virtual ~Math();
};

}

