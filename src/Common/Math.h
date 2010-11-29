/*
 * Math.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

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

