/*
 * Math.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#include "Math.h"

namespace Flewnit
{

//Math::Math()
//{
//	// TODO Auto-generated constructor stub
//
//}
//
//Math::~Math()
//{
//	// TODO Auto-generated destructor stub
//}


void AABB::include(const AABB& other)
{
	include(other.getMin());
	include(other.getMax());
}

void AABB::include(const Vector4D& other)
{
	mMax = glm::max(mMax, other);
	mMin = glm::max(mMin, other);
}

void AABB::reset()
{
	mMax = Vector4D(0);
	mMin = Vector4D(0);
}


}
