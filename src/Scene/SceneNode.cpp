/*
 * SceneNode.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SceneNode.h"

namespace Flewnit
{

SceneNode::SceneNode(String name, Matrix4x4 localtransform  )

: mTypeFlags(PURE_NODE)
{}

SceneNode::~SceneNode()
{
	// TODO Auto-generated destructor stub
}

}
