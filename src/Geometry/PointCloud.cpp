/*
 * PointCloud
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 */

#include "PointCloud.h"

namespace Flewnit {

PointCloud::PointCloud(String name, VBO* vbo)
: Geometry(TRIANGLE_MESH, name), mVBO(vbo)
{
	// TODO Auto-generated constructor stub

}

PointCloud::~PointCloud() {
	// TODO Auto-generated destructor stub
}

void PointCloud::render()
{

}

}
