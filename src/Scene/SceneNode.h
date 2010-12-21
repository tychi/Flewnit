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
	AABB mLocalAABB;
	Matrix4x4 mGlobalTransform;
	AABB mGobalAABB;


	SceneNode* mParent;


public:
	//parent is set by addScneno-function automatically
	SceneNode(String name, Matrix4x4 localtransform = Matrix4x4() );
	virtual ~SceneNode();

	//
	virtual void updateLocalAABB(){}

	AABB& updateGlobalAABB();
	//update own global transform and propagate change to own children; then collect children's gloabal AABB
	void parentTransformChanged();

	bool isLeafNode()const{return (mChildren.size()==0);}
	SceneNodeTypeFlags getTypeFlags(){return mTypeFlags;}


	const Matrix4x4& getGlobalTransform();
	//overwrite + handle childen's global transform, AABB n stuffM
	void setGlobalTransform(const Matrix4x4& newTransform);
	void setLocalTransform(const Matrix4x4& newTransform);

	//access for easier iteration
	Map<String, SceneNode* >& getChildren(){return mChildren;}
	SceneNode* getChild(String name);
	SceneNode& operator[](String name);

	SceneNode* addChild(SceneNode* child);



};


}

