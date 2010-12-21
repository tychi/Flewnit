/*
 * Scene.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include "SceneNode.h"

namespace Flewnit
{


class Scene
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	SceneNode* mRootNode;

	//necessary for initialization: <-- NOT ;(
	//friend class Loader;
	//ConfigStructNode* & rootPtr(){return mRootNode;}



public:
	Scene();

	virtual ~Scene();

	SceneNode & root(){return *mRootNode;}

};

}

