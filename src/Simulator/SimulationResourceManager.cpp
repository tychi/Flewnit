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
	mCamera(0), //TODO
	mLighSourceManager(0)//TODO
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
	///\} END TEST STUFF DEBUG

	// TODO Auto-generated constructor stub

}

SimulationResourceManager::~SimulationResourceManager()
{
	delete mScene;
	delete mGlobalRenderTarget;
	delete mCamera;
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
Camera*  SimulationResourceManager::getCamera()const
{
	return mCamera;
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

void SimulationResourceManager::registerInstanceManager(InstanceManager* im)
{
//	assert("Object with specified name may not already exist!"
//			&& mInstanceManagers.find(im->ge));
//	TOCONTINUE

	if(getInstanceManager(im->getName()))
	{
		assert(0 && "Oject with specified name already exists!");
	}
}


//automatically called by BufferInterface constructor
void SimulationResourceManager::registerBufferInterface(BufferInterface* bi)
{
	//TODO
}
//be very careful with this function, as ther may be serveral references
//and I don't work with smart pointers; so do a manual deletion only if you are absolutely sure
//that the buffer is't used by other objects;
//void deleteBufferInterface(BufferInterface* bi);

void SimulationResourceManager::registerTexture(Texture* tex)
{
	//TODO
}
//void SimulationResourceManager::deleteTexture(Texture* tex);

void SimulationResourceManager::registerMPP(MPP* mpp)
{
	//TODO
}

//void SimulationResourceManager::SimulationResourceManager::deleteMPP(MPP* mpp);

void SimulationResourceManager::registerMaterial(Material* mat)
{
	//TODO
}
//void SimulationResourceManager::deleteMaterial(Material* mat);

void SimulationResourceManager::registerGeometry(Geometry* geo)
{
	//TODO
}
//void SimulationResourceManager::deleteGeometry(Geometry* geo);


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

}
