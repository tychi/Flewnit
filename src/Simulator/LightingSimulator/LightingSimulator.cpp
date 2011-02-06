/*
 * LightingSimulator.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "LightingSimulator.h"

#include "UserInterface/WindowManager/WindowManager.h"

#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"
#include "MPP/Shader/ShaderManager.h"

#include "Buffer/Texture.h"

#include "Util/Log/Log.h"

#include <typeinfo>



namespace Flewnit
{

LightingSimulator::LightingSimulator(ConfigStructNode* simConfigNode)
:SimulatorInterface(VISUAL_SIM_DOMAIN,simConfigNode),
	mGlobalRenderTarget(
			new RenderTarget(
					String("globalRenderTarget"),
					WindowManager::getInstance().getWindowResolution(),
					TEXTURE_TYPE_2D_RECT,
					DEPTH_RENDER_BUFFER
					)
	),
	mMainCamera(new Camera("mainCamera", AmendedTransform(Vector3D(0.0f,0.0f,5.0f)))),
	mLightSourceManager(0),//TODO
	mShaderManager(new ShaderManager())
{
	// TODO Auto-generated constructor stub
	testStuff();
}

LightingSimulator::~LightingSimulator()
{
	// TODO Auto-generated destructor stub

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



bool LightingSimulator::stepSimulation()  throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	//LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";
	glClearColor(1,1,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}


//build pipeline according to config;
bool LightingSimulator::initPipeLine()  throw(SimulatorException)
{
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






void LightingSimulator::testStuff()
{
	///\{ TEST STUFF DEBUG
	mGlobalRenderTarget->bind();
	mGlobalRenderTarget->requestCreateAndStoreColorTexture(FINAL_RENDERING_SEMANTICS);
	mGlobalRenderTarget->attachStoredColorTexture(FINAL_RENDERING_SEMANTICS, 0);
	mGlobalRenderTarget->renderToScreen();

	int bufferSize = WindowManager::getInstance().getWindowResolution().x
			* WindowManager::getInstance().getWindowResolution().y;
	Vector4D* myTestData = new Vector4D[bufferSize];
	for (int i = 0; i < bufferSize; i++)
	{
		myTestData[i] = Vector4D(1, 2, 3, i);
	}

	Texture2D* testTex = new Texture2D("FUtestTex", CUSTOM_SEMANTICS,
			WindowManager::getInstance().getWindowResolution().x,
			WindowManager::getInstance().getWindowResolution().y, TexelInfo(4,
					GPU_DATA_TYPE_FLOAT, 32, false), true,
			//clinterop
			true, false, myTestData, false);

	delete[] myTestData;

	//test AmendedTransform:------------------------------------
	//Vector3D pos(1.0f,4.0f,6.0f);
	Vector3D pos(3.0f,-41.034f,1.0f);
	//Vector3D dir(1.0f,0.0f,-1.0f);//-45° around y axis from the negative z axis
	Vector3D dir(-glm::sin(glm::radians(25.0f)),0.0f,-glm::cos(glm::radians(25.0f)));
	Vector3D up(0.0f,1.0f,0.0f);
	float scale = 453.5f;

	Matrix4x4 testTranslateMat= Matrix4x4();
	testTranslateMat = glm::translate(testTranslateMat, pos);
	Matrix4x4 testRotateMat= Matrix4x4();
	//testRotateMat = glm::rotate(testRotateMat,-45.0f,Vector3D(0.0f,1.0f,0.0f));
	testRotateMat = glm::rotate(testRotateMat,25.0f,Vector3D(0.0f,1.0f,0.0f));
	Matrix4x4 testScaleMat= Matrix4x4(Matrix3x3(scale));
	Matrix4x4 testAccumMat = testTranslateMat * testRotateMat * testScaleMat;

	AmendedTransform testAmendedTrans= AmendedTransform(pos,dir,up,scale);

	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat : "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));

	assert("lookAt-matrices are equal" &&
			AmendedTransform::matricesAreEqual(
					testAmendedTrans.getLookAtMatrix(),
					glm::lookAt(pos, pos+dir, up )
			)
	);

	//checks itself in constructor
	AmendedTransform amendFromMatrix(testAccumMat);

	testAccumMat = glm::inverse(testAccumMat);
	testAmendedTrans = testAmendedTrans.getInverse();
	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat inverse: "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));


	//LOG<<DEBUG_LOG_LEVEL<<"lookat of                  inverse: "<<testAccumMat<<";\n";
	//LOG<<DEBUG_LOG_LEVEL<<"lookat of AmendedTrans mat inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert("lookAt-matrices of inverse are equal" &&
			AmendedTransform::matricesAreEqual(
					testAmendedTrans.getLookAtMatrix(),
					glm::lookAt(
						testAmendedTrans.getPosition(),
						testAmendedTrans.getPosition()+testAmendedTrans.getDirection(),
						testAmendedTrans.getUpVector()
					)

			)
	);


	//checks itself in constructor
	AmendedTransform amendFromInverseMatrix(testAccumMat);

	testAccumMat = testAccumMat * testAccumMat;
	testAmendedTrans = testAmendedTrans * testAmendedTrans;
	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat SQUARED: "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		SQUARED: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));
	//checks itself in constructor
	AmendedTransform amendFromInverseSquaredMatrix(testAccumMat);

	//--------------

	Vector3D pos2(121.0f,40.6f,36.98f);
	Vector3D dir2(1.0f,0.0f,-1.0f);//-45° around y axis from the negative z axis
	Vector3D up2(0.0f,1.0f,0.0f);
	float scale2 = 49.457f;

	Matrix4x4 testTranslateMat2= Matrix4x4();
	testTranslateMat2 = glm::translate(testTranslateMat2, pos2);
	Matrix4x4 testRotateMat2 = glm::rotate(-45.0f,up2);
	Matrix4x4 testScaleMat2= Matrix4x4(Matrix3x3(scale2));

	Matrix4x4 testAccumMat2 = testTranslateMat2 * testRotateMat2 * testScaleMat2;
	AmendedTransform testAmendedTrans2= AmendedTransform(pos2,dir2,up2,scale2);

	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat 2 : "<<testAccumMat2<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat	2	: "<<testAmendedTrans2.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans2.getTotalTransform(), testAccumMat2));

	assert(
		AmendedTransform::matricesAreEqual(
			(testAmendedTrans2 * testAmendedTrans).getTotalTransform(),
			testAccumMat2 * testAccumMat
		)
	);

	assert(
		AmendedTransform::matricesAreEqual(
			( 		(testAmendedTrans2 * testAmendedTrans) *
					(testAmendedTrans2 * testAmendedTrans).getInverse()  ).getTotalTransform(),
			Matrix4x4()
		)
	);

	//assert(0 && "just stop for further debugging");
	///\} END TEST STUFF DEBUG
}






}
