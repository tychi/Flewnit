/*
 * HardCodedSceneLoader.h
 *
 *  Created on: Mar 10, 2011
 *      Author: tychi
 *
 *  For quick n dirty feature testing;
 *  Outsourced into this class to not pullute the "real" Loader classes
 */

#pragma once

#include "SceneLoader.h"


namespace Flewnit
{

class HardCodedSceneLoader:
	public SceneLoader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	HardCodedSceneLoader();
	virtual ~HardCodedSceneLoader();

protected:

	virtual void init();

	virtual void loadMaterials();
	virtual void loadGeometries();
	virtual void createSceneNodeHierarchy(); //parse the assimp scene nodes
	virtual void createInstancingSetup();
	virtual void addSubObjectsWorldObjects();
	virtual void loadLightSources();
	virtual void loadCamera();
};

}
