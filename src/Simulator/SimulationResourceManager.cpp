/*
 * SimulationResourceManager.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SimulationResourceManager.h"
#include "Scene/Scene.h"
#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"
#include "UserInterface/WindowManager/WindowManager.h"

#include <boost/foreach.hpp>
#include "WorldObject/InstanceManager.h"
#include "Material/Material.h"
#include "Geometry/Geometry.h"
#include "Buffer/Texture.h"
#include "MPP/MPP.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Util/Log/Log.h"

namespace Flewnit
{

SimulationResourceManager::SimulationResourceManager() :
	mScene(new Scene()),
	mGlobalRenderTarget(
			new RenderTarget(
					String("globalRenderTarget"),
					WindowManager::getInstance().getWindowResolution(),
					TEXTURE_TYPE_2D_RECT,
					DEPTH_RENDER_BUFFER
					)
	),
	mMainCamera(new Camera("mainCamera")), //TODO
	mLightSourceManager(0)//TODO
{
	testStuff();
}

SimulationResourceManager::~SimulationResourceManager()
{
	delete mScene;
	delete mGlobalRenderTarget;
	delete mMainCamera;
	delete mLightSourceManager;


	typedef Map<String, InstanceManager*> InstanceManagerMap;
	BOOST_FOREACH( InstanceManagerMap::value_type & pair, mInstanceManagers)
	{
		delete pair.second;
	}

	typedef Map<String, Material*> MaterialMap;
	BOOST_FOREACH( MaterialMap::value_type & pair, mMaterials)
	{
		delete pair.second;
	}

	typedef Map<String, Geometry*> GeometryMap;
	BOOST_FOREACH( GeometryMap::value_type & pair, mGeometries)
	{
		delete pair.second;
	}

	typedef Map<String, BufferInterface* > BufferMap;
	BOOST_FOREACH( BufferMap::value_type & pair, mBuffers)
	{
		delete pair.second;
	}

	typedef Map<String, MPP*> MPPMap;
	BOOST_FOREACH( MPPMap::value_type & pair, mMPPs)
	{
		delete pair.second;
	}

}

RenderTarget* SimulationResourceManager::getGlobalRenderTarget()const
{
	return mGlobalRenderTarget;
}

Scene* SimulationResourceManager::getScene()const
{
	return mScene;
}

LightSourceManager*  SimulationResourceManager::getLightSourceManager()const
{
	return mLightSourceManager;
}
Camera*  SimulationResourceManager::getMainCamera()const
{
	return mMainCamera;
}


//when a Simulation pass nears its end, it should let do the instance managers the
//"compiled rendering", as render()-calls to instanced geometry only registers drawing needs
//to its instance manager; For every  (at least non-deferred non-Skybox Lighting-) rendering pass,
//call this routine after scene graph traversal
void SimulationResourceManager::executeInstancedRendering()
{
	//TODO
}

//---------------------------------------------------------------

#define FLEWNIT_INTERNAL_REGISTER_MACRO(instanceName,getterFunction,containerMember) \
		if(getterFunction(instanceName->getName())) \
		{ \
			assert(0 && "Object with specified name already exists!"); \
		} \
		else \
		{ \
			containerMember[instanceName->getName()]=instanceName; \
		} \

void SimulationResourceManager::registerInstanceManager(InstanceManager* im)
{
//	assert("Object with specified name may not already exist!"
//			&& (mInstanceManagers.find(im->getName()) == mInstanceManagers.end()) );

	FLEWNIT_INTERNAL_REGISTER_MACRO(im, getInstanceManager, mInstanceManagers)

//	if(getInstanceManager(im->getName()))
//	{
//		assert(0 && "Object with specified name already exists!");
//	}
//	else
//	{
//		mInstanceManagers[im->getName()]=im;
//	}
}


//automatically called by BufferInterface constructor
void SimulationResourceManager::registerBufferInterface(BufferInterface* bi)
{
	FLEWNIT_INTERNAL_REGISTER_MACRO(bi, getBufferInterface, mBuffers)
}
//be very careful with this function, as ther may be serveral references
//and I don't work with smart pointers; so do a manual deletion only if you are absolutely sure
//that the buffer is't used by other objects;
//void deleteBufferInterface(BufferInterface* bi);

void SimulationResourceManager::registerTexture(Texture* tex)
{
	FLEWNIT_INTERNAL_REGISTER_MACRO(tex, getTexture, mTextures)
}
//void SimulationResourceManager::deleteTexture(Texture* tex);

void SimulationResourceManager::registerMPP(MPP* mpp)
{
	FLEWNIT_INTERNAL_REGISTER_MACRO(mpp, getMPP, mMPPs)
}

//void SimulationResourceManager::SimulationResourceManager::deleteMPP(MPP* mpp);

void SimulationResourceManager::registerMaterial(Material* mat)
{
	FLEWNIT_INTERNAL_REGISTER_MACRO(mat, getMaterial, mMaterials)
}
//void SimulationResourceManager::deleteMaterial(Material* mat);

void SimulationResourceManager::registerGeometry(Geometry* geo)
{
	FLEWNIT_INTERNAL_REGISTER_MACRO(geo, getGeometry, mGeometries)
}
//void SimulationResourceManager::deleteGeometry(Geometry* geo);


#undef FLEWNIT_INTERNAL_REGISTER_MACRO


//---------------------------------------------------------------
#define FLEWNIT_INTERNAL_FIND_MACRO(mapMemberName) \
		if(mapMemberName.find(name) == mapMemberName.end()) \
		{\
			return 0;\
		}\
		else\
		{\
			return mapMemberName[name];\
		}

InstanceManager* SimulationResourceManager::getInstanceManager(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mInstanceManagers)
}

BufferInterface* SimulationResourceManager::getBufferInterface(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mBuffers)
}

Texture* SimulationResourceManager::getTexture(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mTextures)
}

MPP* SimulationResourceManager::getMPP(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mMPPs)
}

Material* SimulationResourceManager::getMaterial(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mMaterials)
}

Geometry* SimulationResourceManager::getGeometry(String name)
{
	FLEWNIT_INTERNAL_FIND_MACRO(mGeometries)
}

#undef FLEWNIT_INTERNAL_FIND_MACRO


void SimulationResourceManager::testStuff()
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

	//checks itself in constructor
	AmendedTransform amendFromMatrix(testAccumMat);

	testAccumMat = glm::inverse(testAccumMat);
	testAmendedTrans = testAmendedTrans.getInverse();
	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat inverse: "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));

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

