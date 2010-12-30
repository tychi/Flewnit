/*
 * PointCloud.h
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 */

#pragma once

#include "Geometry.h"

namespace Flewnit
{

class PointCloud
:public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	VBO* mVBO;


public:
	PointCloud(String name, VBO* vbo);
	virtual ~PointCloud();

	virtual void render();

};

}
