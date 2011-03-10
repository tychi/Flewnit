/*
 * SceneNode.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "Common/BasicObject.h"
#include "Common/Math.h"

#include "Simulator/SimulatorMetaInfo.h"
#include "Common/AmendedTransform.h"


namespace Flewnit
{

class SceneNodeVisitor
{
public:
	virtual void visitSceneNode(SceneNode* node)=0;
};




class SceneNode
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	//parent is set by addScenenode-function automatically;
	//global transform is set to the local one initially;
	SceneNode(String name, SceneNodeTypeFlags typeflags,
			const AmendedTransform& localTransform = AmendedTransform());

//			const Vector3D& localPosition = Vector3D(0.0f,0.0f,0.0f),
//			//(0,0,-1) is assumed as initial orientation, extress any deviation in euler angles(radians)
//			const Vector3D& localOrientationEulerAngles = Vector3D(0.0f,0.0f,0.0f),
//			float localScale = 1.0f);

	virtual ~SceneNode();

	void traverse(SceneNodeVisitor* visitor);

	//implement according to underlying geometry (if it exists)
	virtual void updateLocalAABB(){}

	virtual void animate(){}



	inline String getName() const {return mName;}
	inline SceneNodeTypeFlags getTypeFlags() const{return mTypeFlags;}
	inline SceneNode* getParent()const{return mParent;}
	bool isLeafNode()const{return (mChildren.size()==0);}

	//access for easier iteration
	std::map<String, SceneNode* >& getChildren(){return mChildren;}



	AmendedTransform& getGlobalTransform();
	//overwrite + handle childen's global transform, AABB n stuffM
	void setGlobalTransform(const AmendedTransform& newTransform);
	AmendedTransform& getLocalTransform();
	void setLocalTransform(const AmendedTransform& newTransform);

	const AABB& getLocalAABB();
	const AABB& getGlobalAABB();

	//recursively searches in the child trees; returns 0 if node with name does not exist in own descendants
	SceneNode* findNode(String name);

	SceneNode* getChild(String name);
	SceneNode& operator[](String name);

	SceneNode* addChild(SceneNode* child);
	SceneNode* removeChild(String name);

protected:


	//make virtual to be overridden by Camera, so that it can adapt its view matrix
	virtual void setParent(SceneNode* newParent);
	//update own global transform and propagate change to own children; then collect children's gloabal AABB
	//make virtual to be overridden by Camera, so that it can adapt its view matrix
	virtual void parentTransformChanged();

	//to be called by children to inform their parents about transform change, which means that the parents global AABB might have become invalid;
	void invalidateAABB();
	//it is assumed that the parent's global tranform is up to date!
	void updateGlobalAABB();



private:

	friend class AmendedTransform;
	//callback if AmendedTransform is manipulated directly so that the owning node can update itself;
	void transformChanged(bool global);

	String mName;

	//needed by SimPipelinestages to check if a node is interesting
	SceneNodeTypeFlags mTypeFlags;

	typedef std::map<String, SceneNode* > Nodemap;
	Nodemap mChildren;

	AmendedTransform mLocalTransform;
	//the AABB only defined by the pure vertex coordinates, has nothing to do with even the local transform;
	AABB mLocalAABB;

	AmendedTransform mGlobalTransform;
	AABB mGlobalAABB;
	bool mGlobalAABBisValidFlag;

	SceneNode* mParent;

	//guard to omit endless recursions of transformChanged();
	bool mIsCurrentlyUpdating;
};



}

