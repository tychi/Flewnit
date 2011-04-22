/*
 * SceneGraph.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "SceneRepresentation.h"
#include "SceneNode.h"

namespace Flewnit
{



class SceneGraph
: public SceneRepresentation
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


public:
	SceneGraph();

	virtual ~SceneGraph();

	SceneNode & root(){return *mRootNode;}

	//iterates in depth-first, left-first order over the whole scene graph and calls
	//visitor->visitSceneNode(SceneNode* node) for every node; this way,
	//for example a SimulationPipeLineStage can be derived from SceneNodeVisitor and call
	//its simulation code in visitSceneNode() for every node;
	void traverse(SceneNodeVisitor* visitor);

private:

	SceneNode* mRootNode;
};

}

