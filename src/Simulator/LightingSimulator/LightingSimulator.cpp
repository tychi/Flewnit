/*
 * LightingSimulator.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "LightingSimulator.h"

#include "URE.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "MPP/Shader/ShaderManager.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"

#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"
#include "Buffer/Texture.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"

#include "MPP/Shader/GenericLightingUberShader.h"


#include "Simulator/LightingSimulator/LightingStages/SkyBoxRenderer.h"
#include "Simulator/LightingSimulator/LightingStages/ShadowMapGenerator.h"
#include "Simulator/LightingSimulator/LightingStages/DefaultLightingStage.h"



#include "Common/Profiler.h"
#include "Util/Log/Log.h"
#include "Util/Loader/LoaderHelper.h"

#include <boost/foreach.hpp>

#include <typeinfo>
#include "Simulator/ParallelComputeManager.h"






namespace Flewnit
{

LightingSimulator::LightingSimulator(ConfigStructNode* simConfigNode)
:SimulatorInterface(VISUAL_SIM_DOMAIN,simConfigNode),
	mGlobalRenderTarget(
			new RenderTarget(
					String("globalRenderTarget"),
					WindowManager::getInstance().getWindowResolution(),
					//TODO make customizable
					TEXTURE_TYPE_2D_RECT,
					DEPTH_RENDER_BUFFER
					)
	),
	mMainCamera(
		new Camera(
			"mainCamera",
			AmendedTransform(
				//Vector3D(0.0f,0.0f,5.0f)
				ConfigCaster::cast<Vector3D>(
					 simConfigNode->get("generalSettings",0).get("Camera",0).get("position",0)
				),
				ConfigCaster::cast<Vector3D>(
					 simConfigNode->get("generalSettings",0).get("Camera",0).get("direction",0)
				),
				ConfigCaster::cast<Vector3D>(
					 simConfigNode->get("generalSettings",0).get("Camera",0).get("up",0)
				)
			),
			ConfigCaster::cast<float>(
				 simConfigNode->get("generalSettings",0).get("Camera",0).get("openingAngleVerticalDegrees",0)
			)
		)
	),
	mCurrentlyActiveCamera(mMainCamera),
	//mShaderManager(new ShaderManager()),
	mShaderManager(
		new ShaderManager(
			parseGlobalShaderFeatureFromConfig(),
			parseSupportedShadingFeaturesFromConfig(),
			Path( 	ConfigCaster::cast<String>(simConfigNode->get("generalSettings",0).
					  get("GlobalShadingFeatures",0).get("shaderCodeDirectory",0))		)
		)
	),
	mLightSourceManager(new LightSourceManager())//TODO
{
	//every further init in init() routine :P
}


ShadingFeatures LightingSimulator::parseSupportedShadingFeaturesFromConfig()
{
	ConfigStructNode& node = mSimConfigNode->get("generalSettings",0).
				get("supportedShadingFeatures",0);

	return ShadingFeatures(
			( ConfigCaster::cast<bool>(node.get("directLighting",0)) ? SHADING_FEATURE_DIRECT_LIGHTING :0 )
		| 	( ConfigCaster::cast<bool>(node.get("diffuseTexturing",0)) ? SHADING_FEATURE_DIFFUSE_TEXTURING :0 )
		| 	( ConfigCaster::cast<bool>(node.get("normalMapping",0)) ? SHADING_FEATURE_NORMAL_MAPPING :0 )
		| 	( ConfigCaster::cast<bool>(node.get("cubeMapping",0)) ? SHADING_FEATURE_CUBE_MAPPING :0 )
		| 	( ConfigCaster::cast<bool>(node.get("tessellation",0)) ? SHADING_FEATURE_TESSELATION :0 )
	);
}


ShaderFeaturesGlobal LightingSimulator::parseGlobalShaderFeatureFromConfig()
{
	ShaderFeaturesGlobal sfg;

	ConfigStructNode& node = mSimConfigNode->get("generalSettings",0).
			get("GlobalShadingFeatures",0);

	String lightSourcesLightingFeatureString =
		ConfigCaster::cast<String>(	node.get("lightSourcesLightingFeature",0));

	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_NONE")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_NONE;
	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT;
	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT;
	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS;
	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS;
	if(lightSourcesLightingFeatureString == "LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS")
		sfg.lightSourcesLightingFeature = LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS;


	String lightSourcesShadowFeatureString = ConfigCaster::cast<String>(	node.get("lightSourcesShadowFeature",0));
	if(lightSourcesShadowFeatureString == "LIGHT_SOURCES_SHADOW_FEATURE_NONE")
			sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_NONE;
	if(lightSourcesShadowFeatureString == "LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT")
			sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT;
	if(lightSourcesShadowFeatureString == "LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT")
			sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT;
	if(lightSourcesShadowFeatureString == "LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS")
			sfg.lightSourcesShadowFeature = LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS;

	String shadowTechniqueString =	ConfigCaster::cast<String>(	node.get("shadowTechnique",0));
	if(shadowTechniqueString == "SHADOW_TECHNIQUE_NONE")
			sfg.shadowTechnique = SHADOW_TECHNIQUE_NONE;
	if(shadowTechniqueString == "SHADOW_TECHNIQUE_DEFAULT")
			sfg.shadowTechnique = SHADOW_TECHNIQUE_DEFAULT;
	if(shadowTechniqueString == "SHADOW_TECHNIQUE_PCFSS")
			sfg.shadowTechnique = SHADOW_TECHNIQUE_PCFSS;

	sfg.numMaxLightSources =	ConfigCaster::cast<int>(node.get("numMaxLightSources",0));
	sfg.numMaxShadowCasters =	ConfigCaster::cast<int>(node.get("numMaxShadowCasters",0));
	sfg.numMaxInstancesRenderable =	ConfigCaster::cast<int>(node.get("numMaxInstancesRenderable",0));


	String GBufferTypeString =
		ConfigCaster::cast<String>(	node.get("shadowTechnique",0));

	if(GBufferTypeString == "TEXTURE_TYPE_2D")
			sfg.GBufferType = TEXTURE_TYPE_2D;
	if(GBufferTypeString == "TEXTURE_TYPE_2D_RECT")
			sfg.GBufferType = TEXTURE_TYPE_2D_RECT;
	if(GBufferTypeString == "TEXTURE_TYPE_2D_ARRAY")
			sfg.GBufferType = TEXTURE_TYPE_2D_ARRAY;
	if(GBufferTypeString == "TEXTURE_TYPE_2D_MULTISAMPLE")
			sfg.GBufferType = TEXTURE_TYPE_2D_MULTISAMPLE;
	if(GBufferTypeString == "TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE")
			sfg.GBufferType = TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE;

	sfg.numMultiSamples = ConfigCaster::cast<int> (
							URE_INSTANCE->getConfig().root().
							get("UI_Settings",0).
							get("windowSettings",0).
							get("numMultiSamples",0));

	return sfg;
}

LightingSimulator::~LightingSimulator()
{

	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
	{
		delete stage;
	}

	delete mGlobalRenderTarget;
	delete mMainCamera;
	delete mLightSourceManager;
	delete mShaderManager;


}


inline ShaderManager* LightingSimulator::getShaderManager()const
{
	assert(mShaderManager);
	return mShaderManager;
}


RenderTarget* LightingSimulator::getGlobalRenderTarget()const
{
	return mGlobalRenderTarget;
}

LightSourceManager*  LightingSimulator::getLightSourceManager()const
{
	return mLightSourceManager;
}
Camera*  LightingSimulator::getMainCamera()const
{
	return mMainCamera;
}





//build pipeline according to config;
bool LightingSimulator::initPipeLine()  throw(SimulatorException)
{
	mLightSourceManager->init();

	unsigned int numStages = mSimConfigNode->get("SimulationPipelineStage").size();

	for(unsigned int i =0 ; i < numStages; i++)
	{
		String stageType = ConfigCaster::cast<String>(
				mSimConfigNode->get("SimulationPipelineStage",i).get("Type",0));


//		if(stageType  == "SkyBoxRenderer")
//		{
//			mSimStages.push_back(new SkyBoxRenderer( &( mSimConfigNode->get("SimulationPipelineStage",i) ) ) );
//		}

		if(stageType  == "ShadowMapGenerator")
		{
			mSimStages.push_back(new ShadowMapGenerator( &( mSimConfigNode->get("SimulationPipelineStage",i) ) ) );
		}

		if(stageType  == "DefaultLightingStage")
		{
			mSimStages.push_back(new DefaultLightingStage( &( mSimConfigNode->get("SimulationPipelineStage",i) ) ) );
		}

	}

	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
	{
		stage->initStage();
	}

	RenderTarget::renderToScreen();
	GUARD(glClearColor(0.3,0.2,0.1,0));
	GUARD(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	GUARD(glEnable(GL_DEPTH_TEST));
	GUARD(glDepthFunc(GL_LESS));

	GUARD(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

	//GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
	GUARD(glPointSize(10.0f));
	GUARD(glEnable(GL_LINE_SMOOTH));
	GUARD(glLineWidth(0.01f));

	//GUARD(glPolygonMode(GL_FRONT, GL_FILL));
    GUARD(glEnable(GL_CULL_FACE));
    //glCullFace(GL_FRONT); <-- good for skydome rendering

	String polygonMode = ConfigCaster::cast<String>(
			 mSimConfigNode->get("generalSettings",0).get("polygonMode",0)  );
	GLenum glPolyMode = 0;
	if(polygonMode == "POINT") { glPolyMode = GL_POINT;}
	else {
		if(polygonMode == "LINE") { glPolyMode = GL_LINE;}
		else{
			if(polygonMode == "FILL") { glPolyMode = GL_FILL;}
			else { assert(0&&"wrong polygon mode!");}
		}
	}
	GUARD(glPolygonMode(GL_FRONT_AND_BACK, glPolyMode));


	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine() \n";
	return true;


}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool LightingSimulator::validatePipeLine()  throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine() \n";
	return true;
}


bool LightingSimulator::stepSimulation()  throw(SimulatorException)
{
	//render to and clear screen
	RenderTarget::renderToScreen();
	GUARD(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera()
		->setGLViewPort(
				Vector2Di(0,0),
				Vector2Di(WindowManager::getInstance().getWindowResolution())
	);

	//update the uniform buffers of the light sources and the shadow map matrices
	//mLightSourceManager->updateLightSourcesUniformBuffer(mMainCamera);
	//mLightSourceManager->updateShadowMapMatricesUniformBuffer(mMainCamera);



	for(unsigned int i=0; i< mSimStages.size(); i++)
	{
		mSimStages[i]->stepSimulation();
	}

	WindowManager::getInstance().swapBuffers();

	return true;
}

//##################################################################################


void LightingSimulator::testStuff()
{
	///\{ TEST STUFF DEBUG


//	Shader* testShader = new GenericLightingUberShader(
//			ShaderManager::getInstance().getShaderCodeDirectory(),ShaderFeaturesLocal());
//	Profiler::getInstance().updateMemoryTrackingInfo();
//	delete testShader;
//
//
//
//
//	mGlobalRenderTarget->bind();
//	mGlobalRenderTarget->requestCreateAndStoreColorTexture(FINAL_RENDERING_SEMANTICS);
//	mGlobalRenderTarget->attachStoredColorTexture(FINAL_RENDERING_SEMANTICS, 0);
//	mGlobalRenderTarget->renderToScreen();
//
//	int bufferSize = WindowManager::getInstance().getWindowResolution().x
//			* WindowManager::getInstance().getWindowResolution().y;
//	Vector4D* myTestData = new Vector4D[bufferSize];
//	for (int i = 0; i < bufferSize; i++)
//	{
//		myTestData[i] = Vector4D(1, 2, 3, i);
//	}
//
//	Texture2D* testTex = new Texture2D("FUtestTex", CUSTOM_SEMANTICS,
//			WindowManager::getInstance().getWindowResolution().x,
//			WindowManager::getInstance().getWindowResolution().y, BufferElementInfo(4,
//					GPU_DATA_TYPE_FLOAT, 32, false), true,
//			//clinterop
//			true, false, myTestData, false);
//
//	delete[] myTestData;
//
//	//test AmendedTransform:------------------------------------
//	//Vector3D pos(1.0f,4.0f,6.0f);
//	Vector3D pos(3.0f,-41.034f,1.0f);
//	//Vector3D dir(1.0f,0.0f,-1.0f);//-45° around y axis from the negative z axis
//	Vector3D dir(-glm::sin(glm::radians(25.0f)),0.0f,-glm::cos(glm::radians(25.0f)));
//	Vector3D up(0.0f,1.0f,0.0f);
//	float scale = 453.5f;
//
//	Matrix4x4 testTranslateMat= Matrix4x4();
//	testTranslateMat = glm::translate(testTranslateMat, pos);
//	Matrix4x4 testRotateMat= Matrix4x4();
//	//testRotateMat = glm::rotate(testRotateMat,-45.0f,Vector3D(0.0f,1.0f,0.0f));
//	testRotateMat = glm::rotate(testRotateMat,25.0f,Vector3D(0.0f,1.0f,0.0f));
//	Matrix4x4 testScaleMat= Matrix4x4(Matrix3x3(scale));
//	Matrix4x4 testAccumMat = testTranslateMat * testRotateMat * testScaleMat;
//
//	AmendedTransform testAmendedTrans= AmendedTransform(pos,dir,up,scale);
//
//	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat : "<<testAccumMat<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		: "<<testAmendedTrans.getTotalTransform()<<";\n";
//	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));
//
//	assert("lookAt-matrices are equal" &&
//			AmendedTransform::matricesAreEqual(
//					testAmendedTrans.getLookAtMatrix(),
//					glm::lookAt(pos, pos+dir, up )
//			)
//	);
//
//	//checks itself in constructor
//	AmendedTransform amendFromMatrix(testAccumMat);
//
//	testAccumMat = glm::inverse(testAccumMat);
//	testAmendedTrans = testAmendedTrans.getInverse();
//	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat inverse: "<<testAccumMat<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
//	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));
//
//
//	//LOG<<DEBUG_LOG_LEVEL<<"lookat of                  inverse: "<<testAccumMat<<";\n";
//	//LOG<<DEBUG_LOG_LEVEL<<"lookat of AmendedTrans mat inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
//	assert("lookAt-matrices of inverse are equal" &&
//			AmendedTransform::matricesAreEqual(
//					testAmendedTrans.getLookAtMatrix(),
//					glm::lookAt(
//						testAmendedTrans.getPosition(),
//						testAmendedTrans.getPosition()+testAmendedTrans.getDirection(),
//						testAmendedTrans.getUpVector()
//					)
//
//			)
//	);
//
//
//	//checks itself in constructor
//	AmendedTransform amendFromInverseMatrix(testAccumMat);
//
//	testAccumMat = testAccumMat * testAccumMat;
//	testAmendedTrans = testAmendedTrans * testAmendedTrans;
//	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat SQUARED: "<<testAccumMat<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		SQUARED: "<<testAmendedTrans.getTotalTransform()<<";\n";
//	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));
//	//checks itself in constructor
//	AmendedTransform amendFromInverseSquaredMatrix(testAccumMat);
//
//	//--------------
//
//	Vector3D pos2(121.0f,40.6f,36.98f);
//	Vector3D dir2(1.0f,0.0f,-1.0f);//-45° around y axis from the negative z axis
//	Vector3D up2(0.0f,1.0f,0.0f);
//	float scale2 = 49.457f;
//
//	Matrix4x4 testTranslateMat2= Matrix4x4();
//	testTranslateMat2 = glm::translate(testTranslateMat2, pos2);
//	Matrix4x4 testRotateMat2 = glm::rotate(-45.0f,up2);
//	Matrix4x4 testScaleMat2= Matrix4x4(Matrix3x3(scale2));
//
//	Matrix4x4 testAccumMat2 = testTranslateMat2 * testRotateMat2 * testScaleMat2;
//	AmendedTransform testAmendedTrans2= AmendedTransform(pos2,dir2,up2,scale2);
//
//	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat 2 : "<<testAccumMat2<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat	2	: "<<testAmendedTrans2.getTotalTransform()<<";\n";
//	assert( AmendedTransform::matricesAreEqual(testAmendedTrans2.getTotalTransform(), testAccumMat2));
//
//	assert(
//		AmendedTransform::matricesAreEqual(
//			(testAmendedTrans2 * testAmendedTrans).getTotalTransform(),
//			testAccumMat2 * testAccumMat
//		)
//	);
//
//	assert(
//		AmendedTransform::matricesAreEqual(
//			( 		(testAmendedTrans2 * testAmendedTrans) *
//					(testAmendedTrans2 * testAmendedTrans).getInverse()  ).getTotalTransform(),
//			Matrix4x4()
//		)
//	);
//
//	//assert(0 && "just stop for further debugging");
//	///\} END TEST STUFF DEBUG
}






}
