/*
 * SceneLoader.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: tychi
 */

#include "SceneLoader.h"

#include "Util/Loader/Config.h"
#include "Scene/SceneNode.h"

#include "Util/Loader/LoaderHelper.h"
#include "Simulator/SimulationResourceManager.h"
#include "Scene/Scene.h"

#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>
#include "WorldObject/PureVisualObject.h"



namespace Flewnit
{

SceneLoader::SceneLoader(ConfigStructNode& scenesGlobalSettings, ConfigStructNode& sceneConfig, bool initialize)
:
		mTesselateMeshesWithDisplacementMap(false),
		mSceneRootNode(0),
		mImporter(0),
		mAssimpScene(0)
{
	mTesselateMeshesWithDisplacementMap = ConfigCaster::cast<bool>(scenesGlobalSettings.get("tesselateMeshesWithDisplacementMap",0));
	mReferenceCountThresholdForInstancedRendering =  ConfigCaster::cast<int>(scenesGlobalSettings.get("referenceCountThresholdForInstancedRendering",0));

	mSceneRootNode = parseSceneNode(sceneConfig.get("SceneNode",0));
	SimulationResourceManager::getInstance().getScene()->root().addChild(mSceneRootNode);

	if(initialize)
	{
		init(sceneConfig);
	}
}

SceneLoader::~SceneLoader()
{
	delete mImporter;
}

void SceneLoader::init(ConfigStructNode& sceneConfig)
{

	Path sceneFilePath = Path(
			ConfigCaster::cast<String>( sceneConfig.get("sceneFile",0) )
	);

	mImporter = new Assimp::Importer();

	mAssimpScene = mImporter->ReadFile(
			sceneFilePath.string(),
			aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			//| aiProcess_JoinIdenticalVertices
			//| aiProcess_SortByPType
			);

}

void SceneLoader::loadScene()
{
	PureVisualObject* testRaptorFromBlend = new PureVisualObject("testRaptorFromBlend");
	mSceneRootNode->addChild(testRaptorFromBlend);


}

void SceneLoader::loadTextures(){}
void SceneLoader::loadMaterials(){}
void SceneLoader::loadGeometries(){}
void SceneLoader::createSceneNodeHierarchy(){} //parse the assimp scene nodes
void SceneLoader::createInstancingSetup(){}
void SceneLoader::addSubObjectsWorldObjects(){}
void SceneLoader::loadLightSources(){}
void SceneLoader::loadCamera(){}



//--------------------------------------------
SceneNode* SceneLoader::parseSceneNode(ConfigStructNode& configNode)
{
	String sceneNodeTypeString = ConfigCaster::cast<String>(configNode.get("Type",0));
	assert((sceneNodeTypeString == "PURE_NODE") && "no fancy xml scene node parsing implemented yet");

	return new SceneNode(
			ConfigCaster::cast<String>(configNode.get("name",0)),
			PURE_NODE,
			AmendedTransform(
				ConfigCaster::cast<Vector3D>(configNode.get("position",0)),
				ConfigCaster::cast<Vector3D>(configNode.get("direction",0)),
				ConfigCaster::cast<Vector3D>(configNode.get("up",0)),
				ConfigCaster::cast<float>(configNode.get("scale",0))
			)
	);
}


}
