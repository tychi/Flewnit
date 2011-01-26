/*
 * SimulationResourceManager.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SimulationResourceManager.h"
#include "Scene/Scene.h"



namespace Flewnit
{

SimulationResourceManager::SimulationResourceManager()
:mScene(new Scene()), mGlobalRenderTarget(new RenderTarget())
{
	// TODO Auto-generated constructor stub

}

SimulationResourceManager::~SimulationResourceManager()
{
	// TODO Auto-generated destructor stub
}


RenderTarget* SimulationResourceManager::getGlobalRenderTarget()const
{
	return mGlobalRenderTarget;
}

Scene* SimulationResourceManager::getScene()const
{
	return mScene;
}

void SimulationResourceManager::registerInstanceManager(InstanceManager* im)
{
	//TODO
}

InstanceManager* SimulationResourceManager::getInstanceManager(String name)
{
	//TODO
}

//when a Simulation pass nears its end, it should let do the instance managers the
//"compiled rendering", as render()-calls to instanced geometry only registers drawing needs
//to its instance manager; For every  (at least non-deferred non-Skybox Lighting-) rendering pass,
//call this routine after scene graph traversal
void SimulationResourceManager::executeInstancedRendering()
{
	//TODO
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

}
