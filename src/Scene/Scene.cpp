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
	mRootNode=FLEWNIT_INSTANTIATE(new SceneNode());
}

Scene::~Scene()
{
	delete mRootNode;
}

}
