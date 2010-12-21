/*
 * SceneNode.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "Common/BasicObject.h"
#include "Common/Math.h"

#include "Simulator/SimulatorMetaInfos.h"


namespace Flewnit
{



class SceneNode
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


protected:

	//needed by SimPipelinestages to check if a node is interesting
	SceneNodeTypeFlags mTypeFlags;
	String mName;

	Map<String, SceneNode* > mChildren;

	Matrix4x4 mLocalTransform;
	Matrix4x4 mTotalTransform;
	AABB mAABB;


	SceneNode* mParent;



public:

	SceneNode();
	virtual ~SceneNode();


	bool isLeafNode()const{return (mChildren.size()==0);}
	SceneNodeTypeFlags getTypeFlags(){return mTypeFlags;}

	SceneNode* operator[](String name);

	//access for easier iteration
	Map<String, SceneNode* > getChildren(){return mChildren;}



};


}

