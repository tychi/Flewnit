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
#include "Buffer/BufferHelperUtils.h"

#include <boost/foreach.hpp>
#include "Geometry/VertexBasedGeometry.h"
#include "Buffer/Buffer.h"
#include "WorldObject/SubObject.h"
#include "MPP/Shader/ShaderManager.h"



namespace Flewnit
{

SceneLoader::SceneLoader(ConfigStructNode& scenesGlobalSettings, ConfigStructNode& sceneConfig, bool initialize)
:
		mRootSceneNode(0),
		mImporter(0),
		mAssimpScene(0)
{
	ShaderManager::getInstance().setEnableShadingFeatures( SHADING_FEATURE_TESSELATION,
			ConfigCaster::cast<bool>(scenesGlobalSettings.get("tesselateMeshesWithDisplacementMap",0)));

	mReferenceCountThresholdForInstancedRendering =  ConfigCaster::cast<int>(scenesGlobalSettings.get("referenceCountThresholdForInstancedRendering",0));

	mRootSceneNode = parseSceneNode(sceneConfig.get("SceneNode",0));
	SimulationResourceManager::getInstance().getScene()->root().addChild(mRootSceneNode);

	if(initialize)
	{
		init(sceneConfig);
	}
}

SceneLoader::~SceneLoader()
{
	delete mImporter;

	BOOST_FOREACH(SubObject* so, mSubObjects)
	{
		//Subobjects are not shareable, hence they are only dummys here to associate geometry to materials
		delete so;
		so = 0;
	}
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
	createSceneNodeHierarchy();

	//loadTextures();
	//loadMaterials();
	loadGeometries();

	addSubObjectsToWorldObjects();


}

void SceneLoader::loadTextures(){}
void SceneLoader::loadMaterials(){}


void SceneLoader::loadGeometries()
{
	const unsigned int numNeededVertsPerface = 3;

	for(unsigned int meshRunner = 0; meshRunner < mAssimpScene->mNumMeshes; meshRunner++)
	{
		aiMesh* assimpMesh = mAssimpScene->mMeshes[meshRunner];

		LOG<<INFO_LOG_LEVEL<<"Loading Mesh from Assimp named "<< String(assimpMesh->mName.data)<<";\n"
				<<"\tnumPositionAttributes: "<< assimpMesh->mNumVertices<<";\n"
				<<"\ttexCoord channels on layer 0:   "<< assimpMesh->mNumUVComponents[0]<<";\n"
				<<"\ttexCoord channels on layer 1:   "<< assimpMesh->mNumUVComponents[1]<<";\n"
				<<"\ttexCoord channels on layer 2:   "<< assimpMesh->mNumUVComponents[2]<<";\n"
				<<"\ttexCoord channels on layer 3:   "<< assimpMesh->mNumUVComponents[3]<<";\n"
				<<"\tnumFaces: "<< assimpMesh->mNumFaces<<";\n";

		assert("at least one 2-channel texcoord layer must exist!" &&
				(assimpMesh->mNumUVComponents[0] >= 2));

		std::vector<Vector4D> positions;
		positions.resize(assimpMesh->mNumVertices, Vector4D(0.0f));
		std::vector<Vector4D> normals;
		normals.resize(assimpMesh->mNumVertices, Vector4D(0.0f));
		std::vector<Vector4D> tangents;
		tangents.resize(assimpMesh->mNumVertices, Vector4D(0.0f));
		std::vector<Vector4D> texCoords;
		texCoords.resize(assimpMesh->mNumVertices, Vector4D(0.0f));
		std::vector<GLuint> indices;
		indices.resize(assimpMesh->mNumFaces * numNeededVertsPerface);

		//memset(texCoords.data(),0, BufferHelper::elementSize(TYPE_VEC4F) * assimpMesh->mNumVertices);

		for (unsigned vertRunner = 0; vertRunner < assimpMesh->mNumVertices; vertRunner++)
		{
//			assert("all texcoords shall be initialized to zero because of memset"
//					&&  (texCoords[vertRunner].x == 0.0f)
//					&&  (texCoords[vertRunner].y == 0.0f)
//					&&  (texCoords[vertRunner].z == 0.0f)
//					&&  (texCoords[vertRunner].w == 0.0f) );

			aiVector3D assimpPos = assimpMesh->mVertices[vertRunner];
			aiVector3D assimpNormal;
			if(assimpMesh->HasNormals())
			{
				assimpNormal = assimpMesh->mNormals[vertRunner];
			}
			aiVector3D assimpTangent;
			if(assimpMesh->HasTangentsAndBitangents())
			{
				assimpTangent = assimpMesh->mTangents[vertRunner];
			}

			positions[vertRunner].x = assimpPos.x;
			positions[vertRunner].y = assimpPos.y;
			positions[vertRunner].z = assimpPos.z;
			positions[vertRunner].w = 1.0f;

			normals[vertRunner].x = assimpNormal.x;
			normals[vertRunner].y = assimpNormal.y;
			normals[vertRunner].z = assimpNormal.z;
			normals[vertRunner].w = 0.0f;

			tangents[vertRunner].x = assimpTangent.x;
			tangents[vertRunner].y = assimpTangent.y;
			tangents[vertRunner].z = assimpTangent.z;
			tangents[vertRunner].w = 0.0f;

			//---------- texcoords ----------------------------------------------
			aiVector3D assimpTexCoord0;
			aiVector3D assimpTexCoord1;
			if( assimpMesh->HasTextureCoords(0) )
			{
				assimpTexCoord0 = assimpMesh->mTextureCoords[0][vertRunner];
			}
			if( assimpMesh->HasTextureCoords(1) )
			{
				assimpTexCoord1 = assimpMesh->mTextureCoords[1][vertRunner];
			}

			texCoords[vertRunner].x =assimpTexCoord0.x;
			texCoords[vertRunner].y =assimpTexCoord0.y;
			texCoords[vertRunner].z =assimpTexCoord1.x;
			texCoords[vertRunner].w =assimpTexCoord1.y;
		}


		//------------------------------------------------------------------------


		for (unsigned int  faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
		{
			aiFace* assimpFace = &(assimpMesh->mFaces[faceIndex]);
			assert(assimpFace->mNumIndices == numNeededVertsPerface);

			for (unsigned faceVertIndex = 0; faceVertIndex < assimpFace->mNumIndices; faceVertIndex++)
			{
				unsigned int currentVertexIndex = assimpFace->mIndices[faceVertIndex];

				indices[(faceIndex * numNeededVertsPerface) + faceVertIndex]=currentVertexIndex;
			}
		}


		//===================================================================================
		//create the "native" Flewnit geometry from the parsed vectors:


		VertexBasedGeometry* flewnitGeo = new VertexBasedGeometry(
			String(assimpMesh->mName.data)+String("Geometry"),
			VERTEX_BASED_TRIANGLE_PATCHES //HARDCODE TODO REFACTOR
		);
		BufferInfo bufferi(
				String(assimpMesh->mName.data) + String("PositionBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				POSITION_SEMANTICS,
				TYPE_VEC4F,
				positions.size(),
				BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
				VERTEX_ATTRIBUTE_BUFFER_TYPE,
				NO_CONTEXT_TYPE
		);

		flewnitGeo->setAttributeBuffer(new Buffer(bufferi,false, positions.data()));

		bufferi.name = String(assimpMesh->mName.data) + String("NormalBuffer"),
		bufferi.bufferSemantics = NORMAL_SEMANTICS;
		flewnitGeo->setAttributeBuffer(new Buffer(bufferi,false,normals.data()));

		if(assimpMesh->HasTangentsAndBitangents())
		{
			bufferi.name = String(assimpMesh->mName.data) + String("TangentBuffer"),
			bufferi.bufferSemantics = TANGENT_SEMANTICS;
			flewnitGeo->setAttributeBuffer( new Buffer( bufferi, false, tangents.data() )	);
		}

		bufferi.name = String(assimpMesh->mName.data) + String("TexCoordBuffer"),
		bufferi.bufferSemantics = TEXCOORD_SEMANTICS;
		flewnitGeo->setAttributeBuffer(new Buffer(bufferi,false, texCoords.data()));

		flewnitGeo->setIndexBuffer(
			new Buffer(
				BufferInfo(
					String(assimpMesh->mName.data) + String("IndexBuffer"),
					ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
					INDEX_SEMANTICS,
					TYPE_UINT32,
					indices.size(),
					BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
					VERTEX_INDEX_BUFFER_TYPE,
					NO_CONTEXT_TYPE
				),
				false,
				indices.data()
			)
		);



		mSubObjects.push_back(new SubObject(
				String(assimpMesh->mName.data),
				VISUAL_SIM_DOMAIN,
				flewnitGeo,
				SimulationResourceManager::getInstance().getMaterial("raptorTessMat")//HARDCODE TODO REMOVE
		));

	}

}


//parse the assimp scene nodes
void SceneLoader::createSceneNodeHierarchy()
{
	//HARDCODE TEST TODO IMPLEMENT CORRECTLY

	PureVisualObject* testRaptorFromBlend = new PureVisualObject("testRaptorFromBlend");
	mRootSceneNode->addChild(testRaptorFromBlend);
}

void SceneLoader::createInstancingSetup(){}

void SceneLoader::addSubObjectsToWorldObjects()
{
	//HARDCODE TEST TODO IMPLEMENT CORRECTLY
	assert(mRootSceneNode->findNode("testRaptorFromBlend"));
	assert(dynamic_cast<WorldObject*>(mRootSceneNode->findNode("testRaptorFromBlend")));

	dynamic_cast<WorldObject*>(mRootSceneNode->findNode("testRaptorFromBlend"))->addSubObject(
		new SubObject(
			"testRaptorFromBlendSO",VISUAL_SIM_DOMAIN,
			mSubObjects[0]->getGeometry(),
			mSubObjects[0]->getMaterial()
		)
	);
}

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
