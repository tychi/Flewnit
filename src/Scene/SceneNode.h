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


private:

	//needed by SimPipelinestages to check if a node is interesting
	SceneNodeTypeFlags mTypeFlags;
	String mName;

	typedef Map<String, SceneNode* > Nodemap;
	Nodemap mChildren;

	Matrix4x4 mLocalTransform;
	//the AABB only defined by the pure vertex coordinates, has nothing to do with even the local transform;
	AABB mLocalAABB;

	Matrix4x4 mGlobalTransform;
	AABB mGlobalAABB;
	bool mGlobalAABBisValidFlag;

	SceneNode* mParent;

protected:

	//to be called by children to inform their parents about transform change, which means that the parents global AABB might have become invalid;
	void invalidateAABB();
	void setParent(SceneNode* newParent);
	//it is assumed that the parent's gloabal tranform is up to date!
	void updateGlobalAABB();


public:
	//parent is set by addScneno-function automatically
	SceneNode(String name, SceneNodeTypeFlags typeflags, Matrix4x4 localtransform = Matrix4x4() );
	virtual ~SceneNode();

	//implement accourding to underlying geometry (if it exists)
	virtual void updateLocalAABB(){}



	inline String getName() const {return mName;}
	inline SceneNodeTypeFlags getTypeFlags() const{return mTypeFlags;}
	inline SceneNode* getParent()const{return mParent;}
	bool isLeafNode()const{return (mChildren.size()==0);}

	//access for easier iteration
	Map<String, SceneNode* >& getChildren(){return mChildren;}

	//update own global transform and propagate change to own children; then collect children's gloabal AABB
	void parentTransformChanged();



	const Matrix4x4& getGlobalTransform();
	//overwrite + handle childen's global transform, AABB n stuffM
	void setGlobalTransform(const Matrix4x4& newTransform);
	const Matrix4x4& getLocalTransform();
	void setLocalTransform(const Matrix4x4& newTransform);

	const AABB& getLocalAABB();
	const AABB& getGlobalAABB();


	SceneNode* getChild(String name);
	SceneNode& operator[](String name);

	SceneNode* addChild(SceneNode* child);
	SceneNode* removeChild(String name);



};


}

