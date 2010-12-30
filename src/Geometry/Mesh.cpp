/*
 * Mesh.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 */

#include "Mesh.h"

namespace Flewnit {

Mesh::Mesh(String name, VBO* vbo)
: Geometry(TRIANGLE_MESH, name), mVBO(vbo)
{
	// TODO Auto-generated constructor stub

}

Mesh::~Mesh() {
	// TODO Auto-generated destructor stub
}

void Mesh::render()
{

}

}
