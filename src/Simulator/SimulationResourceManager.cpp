/*
 * SimulationResourceManager.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SimulationResourceManager.h"
#include "Scene/Scene.h"

#include "WorldObject/InstanceManager.h"
#include "Material/Material.h"
#include "Geometry/Geometry.h"
#include "Buffer/Texture.h"
#include "MPP/MPP.h"

#include "Util/Log/Log.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

//TODO init according to config file
SimulationResourceManager::SimulationResourceManager() :
	mScene(new Scene())
{

}

SimulationResourceManager::~SimulationResourceManager()
{
	//delete the "small" objects first, then the containers/managers

	typedef std::map<String, InstanceManager*> InstanceManagerMap;
	BOOST_FOREACH( InstanceManagerMap::value_type & pair, mInstanceManagers)
	{
		delete pair.second;
	}

	typedef std::map<String, Material*> MaterialMap;
	BOOST_FOREACH( MaterialMap::value_type & pair, mMaterials)
	{
		delete pair.second;
	}

	typedef std::map<String, Geometry*> GeometryMap;
	BOOST_FOREACH( GeometryMap::value_type & pair, mGeometries)
	{
		delete pair.second;
	}

	typedef std::map<String, BufferInterface* > BufferMap;
	BOOST_FOREACH( BufferMap::value_type & pair, mBuffers)
	{
		delete pair.second;
	}

	typedef std::map<String, MPP*> MPPMap;
	BOOST_FOREACH( MPPMap::value_type & pair, mMPPs)
	{
		delete pair.second;
	}


	delete mScene;

}


Scene* SimulationResourceManager::getScene()const
{
	return mScene;
}



//when a Simulation pass nears its end, it should let do the instance managers the
//"compiled rendering", as render()-calls to instanced geometry only registers drawing needs
//to its instance manager; For every  (at least non-deferred non-Skybox Lighting-) rendering pass,
//call this routine after scene graph traversal
void SimulationResourceManager::executeInstancedRendering()
{
	typedef std::map<String, InstanceManager*> InstanceManagerMap;
	BOOST_FOREACH( InstanceManagerMap::value_type & pair, mInstanceManagers)
	{
		pair.second->drawRegisteredInstances();
	}
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


}

