/*
 * Scene.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "Scene.h"

namespace Flewnit
{

Scene::Scene(): mRootNode(0)
{
	//TODO
	mRootNode=FLEWNIT_INSTANTIATE(new SceneNode("root",PURE_NODE));
}

Scene::~Scene()
{
	delete mRootNode;
}

}
