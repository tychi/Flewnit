/*
 * SceneGraph.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SceneGraph.h"

namespace Flewnit
{

SceneGraph::SceneGraph()
	:SceneRepresentation(SCENE_GRAPH_REPRESENTATION),
	 mRootNode(0)
{
	//TODO
	mRootNode=new SceneNode("root",PURE_NODE);
}

SceneGraph::~SceneGraph()
{
	delete mRootNode;
}

void SceneGraph::traverse(SceneNodeVisitor* visitor)
{
	mRootNode->traverse(visitor);
}


}
