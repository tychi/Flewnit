/*
 * HardCodedSceneLoader.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: tychi
 */

#include "HardCodedSceneLoader.h"

#include "Util/Loader/Loader.h"

#include "URE.h"
#include "Scene/Scene.h"
#include "Simulator/SimulationResourceManager.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Util/HelperFunctions.h"
#include "Buffer/Texture.h"
#include "MPP/Shader/ShaderManager.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "WorldObject/PureVisualObject.h"
#include "WorldObject/SubObject.h"
#include "Geometry/Procedural/BoxGeometry.h"
#include "WorldObject/InstanceManager.h"
#include "WorldObject/SkyDome.h"




namespace Flewnit
{

HardCodedSceneLoader::HardCodedSceneLoader(ConfigStructNode& scenesGlobalSettings, ConfigStructNode& sceneConfig )
:
		SceneLoader(scenesGlobalSettings,sceneConfig, false)
{


}

HardCodedSceneLoader::~HardCodedSceneLoader()
{
}

void HardCodedSceneLoader::init(ConfigStructNode& sceneConfig)
{

}

void HardCodedSceneLoader::loadScene()
{
	createSceneNodeHierarchy();
	loadGeometries();





	Material* mat1 = SimulationResourceManager::getInstance().getMaterial("BunnyDecalMaterial");
	if(! mat1)
	{
		Texture* decalTex= URE_INSTANCE->getLoader()->loadTexture(
				String("bunnyDecalTex"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/bunny.png"),
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		);

		std::map<BufferSemantics,Texture*> myMap;
		myMap[DIFFUSE_COLOR_SEMANTICS] = decalTex;

		mat1 = new VisualMaterial("BunnyDecalMaterial",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
			),
			myMap,
			//SHADING_FEATURE_DIRECT_LIGHTING,
			//std::map<BufferSemantics,Texture*>(),
			VisualMaterialFlags(true,false,true,true,false,false),
			1000.0f,
			0.5f
		);
	}//endif !mat1


	Material* mat2 = SimulationResourceManager::getInstance().getMaterial("StoneBumpMaterial");
	if(! mat2)
	{
		Texture* decalTex= URE_INSTANCE->getLoader()->loadTexture(
				String("rockbumpDecal"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/rockbumpDecal.jpg"),
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		);
		Texture* normalMap= URE_INSTANCE->getLoader()->loadTexture(
				String("rockbumpNormalDisp"),
				//DIFFUSE_COLOR_SEMANTICS,
				NORMAL_SEMANTICS,
				Path("./assets/textures/rockbumpNormalDisp.png"),
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		);
		std::map<BufferSemantics,Texture*> myMap;
		myMap[DIFFUSE_COLOR_SEMANTICS] = decalTex;
		myMap[NORMAL_SEMANTICS] = normalMap;
		//myMap[DIFFUSE_COLOR_SEMANTICS] = normalMap;
		//myMap[DIFFUSE_COLOR_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("bunnyDecalTex");
		mat2 = new VisualMaterial("StoneBumpMaterial",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
					| SHADING_FEATURE_NORMAL_MAPPING
			),
			myMap,
			//SHADING_FEATURE_DIRECT_LIGHTING,
			//std::map<BufferSemantics,Texture*>(),
			VisualMaterialFlags(true,false,true,true,false,false),
			5.0f,
			0.1f
		);
	}//endif !mat2

	Material* stoneBumpTessMat = SimulationResourceManager::getInstance().getMaterial("StoneBumpTessMaterial");
	if(! stoneBumpTessMat)
	{
		std::map<BufferSemantics,Texture*> myMap;
		myMap[DIFFUSE_COLOR_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("rockbumpDecal");
		myMap[NORMAL_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("rockbumpNormalDisp");
		Texture* dispTex =  SimulationResourceManager::getInstance().getTexture("rockbumpDisp");
		if(!dispTex)
		{
			dispTex= URE_INSTANCE->getLoader()->loadTexture(
				String("rockbumpDisp"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/rockbumpDisp.png"),
				BufferElementInfo(1,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		 	 );
		}
		myMap[DISPLACEMENT_SEMANTICS] = dispTex;
		//myMap[DIFFUSE_COLOR_SEMANTICS] = normalMap;
		//myMap[DIFFUSE_COLOR_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("bunnyDecalTex");
		stoneBumpTessMat = new VisualMaterial("StoneBumpTessMaterial",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
					| SHADING_FEATURE_NORMAL_MAPPING
					| (mTesselateMeshesWithDisplacementMap ? SHADING_FEATURE_TESSELATION :0)
			),
			myMap,
			VisualMaterialFlags(true,false,true,true,false,false),
			100.0f,
			0.4f
		);
	}//endif !stoneBumpTessMat


	Material* blackNWhiteMat = SimulationResourceManager::getInstance().getMaterial("blackNWhiteMat");
	if(! blackNWhiteMat)
	{
		Texture* decalTex =  SimulationResourceManager::getInstance().getTexture("rockwallDispDecalTest");
		if(!decalTex)
		{
		 	 decalTex= URE_INSTANCE->getLoader()->loadTexture(
				String("rockwallDispDecalTest"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/rockbumpDisp.png"),
				BufferElementInfo(1,GPU_DATA_TYPE_UINT,8,true),
//				Path("./assets/blendfiles/raptor/textures/raptorDisp_32f.exr"),
//				BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
				true,
				false,
				true
		 	 );
		}
		std::map<BufferSemantics,Texture*> myMap;
		myMap[DIFFUSE_COLOR_SEMANTICS] = decalTex;
		blackNWhiteMat = new VisualMaterial("blackNWhiteMat",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
			),
			myMap,
			VisualMaterialFlags(true,false,true,true,false,false),
			1000.0f,
			0.5f
		);
	}//endif !blackNWhiteMat


	Material* matEnvMap = SimulationResourceManager::getInstance().getMaterial("EnvMapCloudyNoonMaterial");
	if(! matEnvMap)
	{
		Texture* envMapTex = 0;
		if(SimulationResourceManager::getInstance().getCurrentSkyDome())
		{
			envMapTex = SimulationResourceManager::getInstance().getCurrentSkyDome()->getCubeMap();
		}
		else
		{
			envMapTex = SimulationResourceManager::getInstance().getTexture("cloudy_noon");
			if(!envMapTex)
			{
				envMapTex= URE_INSTANCE->getLoader()->loadCubeTexture(
					Path("./assets/textures/cubeMaps/cloudy_noon.jpg"),
					ENVMAP_SEMANTICS,
					BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
					true,
					false
				);
			}
		}

		Texture* dispTex =  SimulationResourceManager::getInstance().getTexture("rockbumpDisp");
		if(!dispTex)
		{
			dispTex= URE_INSTANCE->getLoader()->loadTexture(
				String("rockbumpDisp"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/rockbumpDisp.png"),
				BufferElementInfo(1,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		 	 );
		}

		std::map<BufferSemantics,Texture*> myMap;
		myMap[ENVMAP_SEMANTICS] = envMapTex;


		myMap[DIFFUSE_COLOR_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("rockbumpDecal");
		myMap[NORMAL_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("rockbumpNormalDisp");
		myMap[DISPLACEMENT_SEMANTICS] = SimulationResourceManager::getInstance().getTexture("rockbumpDisp");

		matEnvMap = new VisualMaterial("EnvMapCloudyNoonMaterial",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
					| SHADING_FEATURE_NORMAL_MAPPING
					| SHADING_FEATURE_CUBE_MAPPING
					| (mTesselateMeshesWithDisplacementMap ? SHADING_FEATURE_TESSELATION : 0)
			),
			myMap,
			//not dyn. cubemap renderable
			VisualMaterialFlags(true,false,true,false,false,false));
	}//endif !matEnvMap



	dynamic_cast<WorldObject*>(mSceneRootNode->getChild("myDefaultBoxGeo1"))->addSubObject(
		new SubObject(
			"BoxSubObject1",
			VISUAL_SIM_DOMAIN,
			SimulationResourceManager::getInstance().getGeometry("myDefaultBoxGeo1"),
			mat1
		)
	);
	dynamic_cast<WorldObject*>(mSceneRootNode->getChild("myTessBoxGeo1"))->addSubObject(
		new SubObject(
			"myTessBoxGeo1SO",
			VISUAL_SIM_DOMAIN,
			SimulationResourceManager::getInstance().getGeometry("myTessBoxGeo1"),
			stoneBumpTessMat
		)
	);

	dynamic_cast<WorldObject*>(mSceneRootNode->getChild("myBlackNWhiteBox"))->addSubObject(
		new SubObject(
			"myBlackNWhiteBoxSO",
			VISUAL_SIM_DOMAIN,
			SimulationResourceManager::getInstance().getGeometry("blackNWhiteGeo"),
			blackNWhiteMat
		)
	);


	dynamic_cast<WorldObject*>(mSceneRootNode->getChild("myBoxAsPlane"))->addSubObject(
		new SubObject(
			"myBoxAsPlaneSubObject1",
			VISUAL_SIM_DOMAIN,
			SimulationResourceManager::getInstance().getGeometry("myBoxAsPlane"),
			mat2
		)
	);
	dynamic_cast<WorldObject*>(mSceneRootNode->getChild("myEnvmapBox"))->addSubObject(
		new SubObject(
			"MyEnvmapBoxSubObject1",
			VISUAL_SIM_DOMAIN,
			new BoxGeometry("myEnvMapBox",Vector3D(5,12.5,7.5f),true,
					mTesselateMeshesWithDisplacementMap
					 ? (WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4)
				     : false,
				     Vector4D(1,1.5,5,5)
			),
			matEnvMap
		)
	);

	//---------------------- begin instanced geometry -----------------------------------
	Geometry* boxForInstancingGeom = SimulationResourceManager::getInstance().getGeometry("boxForInstancingGeom");
	if(! boxForInstancingGeom)
	{
		boxForInstancingGeom = new BoxGeometry("boxForInstancingGeom",Vector3D(5.0f,25.0f,25.0f),true,
				//patch stuff only for GL versions 4.0 or higher
				mTesselateMeshesWithDisplacementMap
				 ? (WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4)
			     : false
		);
	}



	Material* matInstanced = SimulationResourceManager::getInstance().getMaterial("instancedRockWallMaterial");
	if(! matInstanced)
	{
		Texture* decalTex= URE_INSTANCE->getLoader()->loadTexture(
				String("rockWallDecal"),
				DIFFUSE_COLOR_SEMANTICS,
				Path("./assets/textures/rockwallDecal.jpg"),
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		);
		Texture* normalMap= URE_INSTANCE->getLoader()->loadTexture(
				String("rockWallNormalDisp"),
				//DIFFUSE_COLOR_SEMANTICS,
				NORMAL_SEMANTICS,
				Path("./assets/textures/rockwallNormalDisp.png"),
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false,
				true
		);

		Texture* dispTex= URE_INSTANCE->getLoader()->loadTexture(
			String("rockwallDisp.png"),
			DISPLACEMENT_SEMANTICS,
			Path("./assets/textures/rockwallDisp.png"),
			BufferElementInfo(1,GPU_DATA_TYPE_UINT,8,true),
			true,
			false,
			true
	 	 );


		std::map<BufferSemantics,Texture*> myMap;
		myMap[DIFFUSE_COLOR_SEMANTICS] = decalTex;
		myMap[NORMAL_SEMANTICS] = normalMap;
		myMap[DISPLACEMENT_SEMANTICS] = dispTex;
		matInstanced = new VisualMaterial("instancedRockWallMaterial",
			//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
			VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
			ShadingFeatures(
					SHADING_FEATURE_DIRECT_LIGHTING
					| SHADING_FEATURE_DIFFUSE_TEXTURING
					| SHADING_FEATURE_NORMAL_MAPPING
					| (mTesselateMeshesWithDisplacementMap ? SHADING_FEATURE_TESSELATION :0)
			),
			myMap,
			VisualMaterialFlags(true,false,true,true,
					true,//YES, is instanced!
					false),
			5.0f,
			0.1f
		);
	}//endif !matInstanced

	SubObject* drawableBoxSOforInstancing = new SubObject(
				"drawableBoxSOforInstancing",
				VISUAL_SIM_DOMAIN,
				boxForInstancingGeom,
				matInstanced
			);

	int numInstancesPerDimension = 6;
	InstanceManager* boxInstanceManager = new InstanceManager(
			"boxInstanceManager",
			drawableBoxSOforInstancing
	);


	SceneNode* instancesParentNode = mSceneRootNode->addChild(
			new SceneNode(
				"boxInstanceArmyParent",
				PURE_NODE,
				AmendedTransform(
					Vector3D(-80,0,-200),
					Vector3D(0.2,-0.6,-0.8),
					Vector3D(0,1,0),
					0.5
				)
			)
	);
	//Vector3D instanceArmyPosOffset(100,100,-200);
	Vector3D instanceArmyStride(50,50,50);

	for(int x= -numInstancesPerDimension/2; x< numInstancesPerDimension/2; x++)
	{
		for(int y= -numInstancesPerDimension/2; y< numInstancesPerDimension/2; y++)
		{
			for(int z= -numInstancesPerDimension/2; z< numInstancesPerDimension/2; z++)
			{
				PureVisualObject* pvo = new PureVisualObject(
					String("myInstancingTestBoxWO")
						+ String("x") + HelperFunctions::toString(x)
						+ String("y") + HelperFunctions::toString(y)
						+ String("z") + HelperFunctions::toString(z),
					AmendedTransform(
						/*instanceArmyPosOffset +*/ (instanceArmyStride * Vector3D(x,y,z))
						//Vector3D(0.3f,0.0f,-1.0f)
					)
				);
				pvo->addSubObject(boxInstanceManager->createInstance());
				instancesParentNode->addChild(pvo);
			}
		}
	}




	//---------------------- begin light sources -----------------------------------

	if(
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT )
		||
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
	)
	{
		LightSourceManager::getInstance().createPointLight(
				Vector4D(70.0f,45.0f,10.0f,1.0f),
				false,
				Vector4D(1.0f,1.0f,1.0f,1.0f),
				Vector4D(1.0f,1.0f,1.0f,1.0f)
		);

		if(
			(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
					== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
		)
		{
			if(ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources >= 2)
			{
				LightSourceManager::getInstance().createPointLight(
					Vector4D(-70.0f,25.0f,10.0f,1.0f),
					false,
					Vector4D(0.0f,0.0f,1.0f,1.0f),
					Vector4D(0.0f,0.0f,1.0f,1.0f)
				);
			}

			//two lights already created, hence the -2
			int numTotalLightSourcesToCreate = ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources -2;
			for(int i = 1; i <= numTotalLightSourcesToCreate; i++)
			{
				float fraction = (float)(i)/(float)(numTotalLightSourcesToCreate);
				LightSourceManager::getInstance().createPointLight(
					Vector4D(0.0f,15.0f,240.0f + (-480.0f) * fraction ,1.0f) ,
					false,
					Vector4D(0.0f,0.2f,0.0f,1.0f)/ numTotalLightSourcesToCreate,
					Vector4D((1.0f-fraction)*10.0f,fraction*50, fraction*50 ,1.0f)/ numTotalLightSourcesToCreate
				);
			}
		}
	}

	if(
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT )
		||
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	)
	{
		LightSourceManager::getInstance().createSpotLight(
				Vector4D(70.0f,45.0f,10.0f,1.0f),
				Vector4D(0.0f,-1.0f,-0.2f,0.0f),
				false,
				45.0f,
				47.5f,
				50.0f,
				Vector4D(1.0f,1.0f,1.0f,1.0f),
				Vector4D(1.0f,1.0f,1.0f,1.0f)
		);

		if(
			(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
		)
		{
			if(ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources >= 2)
			{
				LightSourceManager::getInstance().createSpotLight(
						Vector4D(-70.0f,25.0f,10.0f,1.0f),
						Vector4D(1.0f,-1.0f,-1.0f,0.0f),
						false,
						45.0f,
						60.5f,
						10.0f,
						Vector4D(0.0f,0.0f,1.0f,1.0f),
						Vector4D(0.0f,0.0f,1.0f,1.0f)
				);
			}

			//two lights already created, hence the -2
			int numTotalLightSourcesToCreate = ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources -2;
			for(int i = 1; i <= numTotalLightSourcesToCreate; i++)
			{
				float fraction = (float)(i)/(float)(numTotalLightSourcesToCreate);
				LightSourceManager::getInstance().createSpotLight(
					Vector4D(0.0f,15.0f,240.0f + (-480.0f) * fraction ,1.0f) ,
					Vector4D(0.0f,-1.0f,0.0f,0.0f),
					false,
					45.0f,
					47.5f,
					10.0f,
					Vector4D(0.0f,0.2f,0.0f,1.0f)/ numTotalLightSourcesToCreate,
					Vector4D((1.0f-fraction)*10.0f,fraction*50, fraction*50 ,1.0f)/ numTotalLightSourcesToCreate
				);
			}
		}
	}




	if(
			(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
								 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
	)
	{
		LightSourceManager::getInstance().createPointLight(
				Vector4D(70.0f,90.0f,10.0f,1.0f),
				false,
				Vector4D(1.0f,1.0f,1.0f,1.0f)*0.25,
				Vector4D(1.0f,1.0f,1.0f,1.0f)*0.25
		);
		LightSourceManager::getInstance().createSpotLight(
				Vector4D(-70.0f,25.0f,10.0f,1.0f),
				Vector4D(1.0f,-1.0f,-1.0f,0.0f),
				false,
				45.0f,
				60.5f,
				10.0f,
				Vector4D(0.0f,0.0f,1.0f,1.0f),
				Vector4D(0.0f,0.0f,1.0f,1.0f)
		);

		//two lights already created, hence the -2
		int numTotalLightSourcesToCreate = ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources -2;
		for(int i = 1; i <= numTotalLightSourcesToCreate; i++)
		{
			float fraction = (float)(i)/(float)(numTotalLightSourcesToCreate);
			if(i%2)
			{
				LightSourceManager::getInstance().createPointLight(
					Vector4D(0.0f,30.0f,240.0f + (-480.0f) * fraction ,1.0f) ,
					false,
					Vector4D(1.0f,1.2f,0.0f,1.0f)/ numTotalLightSourcesToCreate,
					Vector4D((1.0f-fraction)*10.0f,fraction*50, fraction*50 ,1.0f)/ numTotalLightSourcesToCreate
				);
			}
			else
			{
				LightSourceManager::getInstance().createSpotLight(
						Vector4D(0.0f,30.0f,240.0f + (-480.0f) * fraction ,1.0f) ,
						Vector4D(0.0f,-1.0f,0.0f,0.0f),
						false,
						15.0f,
						25.5f,
						10.0f,
						Vector4D(1.0f,1.2f,0.0f,1.0f)/ numTotalLightSourcesToCreate,
						Vector4D((1.0f-fraction)*10.0f,fraction*50, fraction*50 ,1.0f)/ numTotalLightSourcesToCreate
					);
			}

		}
	}


}




void HardCodedSceneLoader::createSceneNodeHierarchy()
{
	//create the first rendering, to see anything and to test the camera, the buffers, the shares and to overall architectural frame:
	//TODO
	mSceneRootNode->addChild(
		new PureVisualObject("myDefaultBoxGeo1",AmendedTransform(Vector3D(0,-10,-30), Vector3D(0.0,0.1,-1.0)))
	);
	mSceneRootNode->addChild(
		new PureVisualObject("myTessBoxGeo1",AmendedTransform(Vector3D(90,-10,50), Vector3D(0.0f,0.9f,0.1f),Vector3D(0,0,1),0.5))
	);
	mSceneRootNode->addChild(
		new PureVisualObject("myBlackNWhiteBox",AmendedTransform(Vector3D(-90,-10,50), Vector3D(0.0f,0.9f,0.1f),Vector3D(0,0,1),1.0))
	);
	mSceneRootNode->addChild(
		new PureVisualObject("myBoxAsPlane",AmendedTransform(Vector3D(0,-40,0), Vector3D(0,0,-1),Vector3D(0,1,0),3.0f))
	);
	mSceneRootNode->addChild(
		new PureVisualObject("myEnvmapBox",AmendedTransform(Vector3D(100,20,-80) )) //, Vector3D(0.0f,0.9f,0.1f),Vector3D(0,0,1)))
	);
}


void HardCodedSceneLoader::loadGeometries()
{
	Geometry* myDefaultBoxGeo1 = SimulationResourceManager::getInstance().getGeometry("myDefaultBoxGeo1");
	if(! myDefaultBoxGeo1)
	{
		myDefaultBoxGeo1 = new BoxGeometry("myDefaultBoxGeo1",Vector3D(30.0f,7.0f,150.0f),true,
				//patch stuff only for GL versions 4.0 or higher
				false
				//(WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4)
		);
	}
	Geometry* myTessBoxGeo1 = SimulationResourceManager::getInstance().getGeometry("myTessBoxGeo1");
	if(! myTessBoxGeo1)
	{
		myTessBoxGeo1 = new BoxGeometry("myTessBoxGeo1",Vector3D(10.0f,10.0f,10.0f),true,
				//patch stuff only for GL versions 4.0 or higher
				mTesselateMeshesWithDisplacementMap
				 ? (WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4)
			     : false
		);
	}
	Geometry* blackNWhiteGeo = SimulationResourceManager::getInstance().getGeometry("blackNWhiteGeo");
	if(! blackNWhiteGeo)
	{
		blackNWhiteGeo = new BoxGeometry("blackNWhiteGeo",Vector3D(20.0f,20.0f,20.0f),true, false);
	}
	Geometry* myBoxAsPlane = SimulationResourceManager::getInstance().getGeometry("myBoxAsPlane");
	if(! myBoxAsPlane)
	{
		myBoxAsPlane = new BoxGeometry("myBoxAsPlane",Vector3D(100.0f,2.0f,100.0f),true,
				//patch stuff only for GL versions 4.0 or higher
				false,
				//(WindowManager::getInstance().getAvailableOpenGLVersion().x >= 4)
				Vector4D(20,20,1,1)
		);
	}

}


void HardCodedSceneLoader::loadTextures()
{

}

void HardCodedSceneLoader::loadMaterials()
{

}


void HardCodedSceneLoader::createInstancingSetup(){}
void HardCodedSceneLoader::addSubObjectsWorldObjects(){}
void HardCodedSceneLoader::loadLightSources(){}
void HardCodedSceneLoader::loadCamera(){}


}
