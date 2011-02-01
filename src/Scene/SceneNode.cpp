/*
 * SceneNode.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SceneNode.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

SceneNode::SceneNode(String name, SceneNodeTypeFlags typeflags,
		const AmendedTransform& localTransform)
: mIsCurrentlyUpdating(false), mTypeFlags(typeflags), mLocalTransform(localTransform), mGlobalAABBisValidFlag(true), mParent(0)
{
	mLocalTransform.setOwningSceneNode(this);
}

SceneNode::~SceneNode()
{
	BOOST_FOREACH(Nodemap::value_type nodepair, mChildren)
	{
		delete nodepair.second;
	}
}

void SceneNode::invalidateAABB()
{
	mGlobalAABBisValidFlag = false;
	if(mParent) mParent->invalidateAABB();
}

//it is assumed that the parent's and its own local/global transform are up to date!
void SceneNode::updateGlobalAABB()
{
	if (!mGlobalAABBisValidFlag)
	{
		//reset own AABB:
		mGlobalAABB.reset();
		//expand own global AABB to include all children's AABBs
		BOOST_FOREACH(Nodemap::value_type nodepair, mChildren)
		{
			//let the child update itself first:
			nodepair.second->updateGlobalAABB();
			mGlobalAABB.include(nodepair.second->getGlobalAABB());
		}

		//a very naive and iniefficient way to include the local AABB into the global one, but optimaziation comes later;( :

		//include own local AABB: take all transformed vertices and include them:
		Vector4D currentLocalAABBVertex;

		currentLocalAABBVertex = Vector4D(mLocalAABB.getMin().x, mLocalAABB.getMin().y, mLocalAABB.getMin().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMin().x, mLocalAABB.getMin().y, mLocalAABB.getMax().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMin().x, mLocalAABB.getMax().y, mLocalAABB.getMin().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMin().x, mLocalAABB.getMax().y, mLocalAABB.getMax().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMax().x, mLocalAABB.getMin().y, mLocalAABB.getMin().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMax().x, mLocalAABB.getMin().y, mLocalAABB.getMax().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMax().x, mLocalAABB.getMax().y, mLocalAABB.getMin().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);
		currentLocalAABBVertex = Vector4D(mLocalAABB.getMax().x, mLocalAABB.getMax().y, mLocalAABB.getMax().z, 1.0f);
		mGlobalAABB.include(mGlobalTransform.getTotalTransform() * currentLocalAABBVertex);

		mGlobalAABBisValidFlag = true;
	}
}

//update own global transform and propagate change to own children; then collect children's global AABB
void SceneNode::parentTransformChanged()
{
	assert("parent exists" && mParent != 0);

	mGlobalAABBisValidFlag=false;

	mGlobalTransform = mParent->getGlobalTransform() * mLocalTransform;

	BOOST_FOREACH(Nodemap::value_type nodepair, mChildren)
	{
		//let the child update itself first:
		nodepair.second->parentTransformChanged();
	}
}

void SceneNode::transformChanged()
{
	if(mIsCurrentlyUpdating) return;

	mIsCurrentlyUpdating = true;

	BOOST_FOREACH(Nodemap::value_type nodepair, mChildren)
	{
		//let the child update itself first:
		nodepair.second->parentTransformChanged();
	}

	if(mParent) mParent->invalidateAABB();

	mIsCurrentlyUpdating=false;
}

const AmendedTransform& SceneNode::getLocalTransform()const
{
	return mLocalTransform;
}

void SceneNode::setLocalTransform(const AmendedTransform& newTransform)
{
	mGlobalAABBisValidFlag = false;

	mLocalTransform = newTransform;
	if(mParent)
	{
		mGlobalTransform = mParent->getGlobalTransform() * newTransform;
	}
	else
	{
		mGlobalTransform = newTransform;
	}

	transformChanged();
}

const AmendedTransform& SceneNode::getGlobalTransform()const
{
	return mGlobalTransform;
}

//overwrite + handle childen's global transform, AABB n stuffM
void SceneNode::setGlobalTransform(const AmendedTransform& newTransform)
{
	mGlobalTransform = newTransform;
	if(mParent)
	{
		//mLocalTransform = glm :: inverse( mParent->getGlobalTransform() ) * newTransform;
		mLocalTransform =  mParent->getGlobalTransform().getInverse() * newTransform;
	}
	else
	{
		mLocalTransform = newTransform;
	}

	transformChanged();
}


const AABB& SceneNode::getLocalAABB()
{
	return mLocalAABB;
}

const AABB& SceneNode::getGlobalAABB()
{
	//lazy evaluation: update only if value is needed AND if it is tagged as invalid;
	if(mGlobalAABBisValidFlag)
	{
		updateGlobalAABB();
	}
	return mGlobalAABB;
}


SceneNode* SceneNode::getChild(String name)
{
	assert(mChildren.find(name) != mChildren.end());

	return mChildren[name];
}

SceneNode& SceneNode::operator[](String name)
{
	assert(mChildren.find(name) != mChildren.end());
	return *(mChildren[name]);
}

SceneNode* SceneNode::addChild(SceneNode* child)
{
	assert("child doesn't exist yet;" && mChildren.find(child->getName()) == mChildren.end());
	mChildren[child->getName()] = child;
	child -> setParent( this );

	mGlobalAABBisValidFlag = false;
	if(mParent) mParent->invalidateAABB();

	return child;

}

SceneNode* SceneNode::removeChild(String name)
{
	assert("child exists" && mChildren.find(name) != mChildren.end());

	SceneNode* childToRemove = mChildren[name];
	childToRemove->setParent(NULL);
	mChildren.erase(name);

	if(mParent) mParent->invalidateAABB();

	return childToRemove;
}

void SceneNode::setParent(SceneNode* newParent)
{
	assert("node has no current parent or is about to be set parentless" && ( (mParent == NULL)  || (newParent==NULL)));

	mParent = newParent;

	if(mParent)
	{
		mGlobalTransform = mParent->getGlobalTransform() *mLocalTransform;
	}
	else
	{
		mGlobalTransform = mLocalTransform;
	}

	BOOST_FOREACH(Nodemap::value_type nodepair, mChildren)
	{
		//let the child update itself first:
		nodepair.second->parentTransformChanged();
	}
}


}


