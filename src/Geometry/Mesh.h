/*
 * Mesh.h
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 */

#pragma once

#include "Geometry.h"

namespace Flewnit
{

class Mesh
:public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	VBO* mVBO;


public:
	Mesh(String name, VBO* vbo);
	virtual ~Mesh();

	virtual void render();

};

}

