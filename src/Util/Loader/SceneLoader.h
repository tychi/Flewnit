/*
 * SceneLoader.h
 *
 *  Created on: Mar 10, 2011
 *      Author: tychi
 */

#pragma once

//#include "Common/BasicObject.h"

#include "Simulator/SimulatorForwards.h"

namespace Assimp
{
	class Importer;
}

class aiScene;

namespace Flewnit
{

class SceneLoader
//: public BasicObject
{
//	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//all info comes from global config, hence no constructor params needed;
	SceneLoader(ConfigStructNode& scenesGlobalSettings, ConfigStructNode& sceneConfig, bool initialize = true );
	virtual ~SceneLoader();

	virtual void loadScene();

protected:
	virtual void init(ConfigStructNode& sceneConfig);

	virtual void loadTextures();
	virtual void loadMaterials();
	virtual void loadGeometries();
	virtual void createSceneNodeHierarchy(); //parse the assimp scene nodes
	virtual void createInstancingSetup();
	virtual void addSubObjectsWorldObjects();
	virtual void loadLightSources();
	virtual void loadCamera();

	SceneNode* parseSceneNode(ConfigStructNode& configNode);

	bool mTesselateMeshesWithDisplacementMap;

	//is appended to the gloabel root scene node automatically
	SceneNode* mSceneRootNode;

	Assimp::Importer* mImporter;
	const aiScene* mAssimpScene;

	//to be loaded first, as Meshes depend on them in assimp;
	std::vector<VisualMaterial*> mMaterials;
	//in assimp, there is a 1:1 association between mesh and material, hence in Flewnit, the
	//SubObject is the most appropriate equivalent;
	std::vector<SubObject*> mSubObjects;
	//contains how often a mesh is used in the scene;
	//if the count surpasses a threshold, the geometry will be managed by an InstancedManager
	int mReferenceCountThresholdForInstancedRendering;
	std::vector<int> mGeometryReferenceCounters;
	std::vector<InstanceManager*> mInstanceManagers;

};

}
