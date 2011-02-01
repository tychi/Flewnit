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
					//hardcode use rectangle
					true,
					DEPTH_RENDER_BUFFER,
					0)
	),
	mMainCamera(0), //TODO
	mLighSourceManager(0)//TODO
{
	testStuff();
}

SimulationResourceManager::~SimulationResourceManager()
{
	delete mScene;
	delete mGlobalRenderTarget;
	delete mMainCamera;
	delete mLighSourceManager;


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

LightSourceManager*  SimulationResourceManager::getLighSourceManager()const
{
	return mLighSourceManager;
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
	mGlobalRenderTarget->requestCreateAndStoreTexture(FINAL_RENDERING_SEMANTICS);
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
	Vector3D pos(3.0f,1.0f,1.0f);
	Vector3D dir(1.0f,0.0f,-1.0f);//-45° around y axis from the negative z axis
	Vector3D up(0.0f,1.0f,0.0f);
	float scale = 4.5f;

	Matrix4x4 testTranslateMat= Matrix4x4();
	testTranslateMat = glm::translate(testTranslateMat, pos);

	Matrix4x4 testRotateMat= Matrix4x4();
	testRotateMat = glm::rotate(testRotateMat,-45.0f,Vector3D(0.0f,1.0f,0.0f));

	Matrix4x4 testScaleMat= Matrix4x4(Matrix3x3(scale));

	Matrix4x4 testAccumMat = testTranslateMat * testRotateMat * testScaleMat;

	AmendedTransform testAmendedTrans= AmendedTransform(pos,dir,up,scale);

	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat : "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		: "<<testAmendedTrans.getTotalTransform()<<";\n";

	testAccumMat = glm::inverse(testAccumMat);
	testAmendedTrans = testAmendedTrans.getInverse();
	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat inverse: "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		inverse: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));

	testAccumMat = testAccumMat * testAccumMat;
	testAmendedTrans = testAmendedTrans * testAmendedTrans;
	LOG<<DEBUG_LOG_LEVEL<<"self constructed mat SQUARED: "<<testAccumMat<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"AmendedTrans mat		SQUARED: "<<testAmendedTrans.getTotalTransform()<<";\n";
	assert( AmendedTransform::matricesAreEqual(testAmendedTrans.getTotalTransform(), testAccumMat));


	//assert(0 && "just stop for further debugging");
	///\} END TEST STUFF DEBUG
}

}

