/*
 * SimulationResourceManager.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "SimulatorForwards.h"

namespace Flewnit
{

class SimulationResourceManager
:	public Singleton<SimulationResourceManager>,
	public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SimulationResourceManager();
	virtual ~SimulationResourceManager();



	RenderTarget* getGlobalRenderTarget()const;

	Scene* getScene()const;

	void registerInstanceManager(InstanceManager* im);
	InstanceManager* getInstanceManager(String name);
	//when a Simulation pass nears its end, it should let do the instance managers the
	//"compiled rendering", as render()-calls to instanced geometry only registers drawing needs
	//to its instance manager; For every  (at least non-deferred non-Skybox Lighting-) rendering pass,
	//call this routine after scene graph traversal
	void executeInstancedRendering();

	//automatically called by BufferInterface constructor
	void registerBufferInterface(BufferInterface* bi);
	//be very careful with this function, as ther may be serveral references
	//and I don't work with smart pointers; so do a manual deletion only if you are absolutely sure
	//that the buffer is't used by other objects;
	//void deleteBufferInterface(BufferInterface* bi);

	void registerTexture(Texture* tex);
	//void deleteTexture(Texture* tex);

	void registerMPP(MPP* mpp);
	//void deleteMPP(MPP* mpp);

	void registerMaterial(Material* mat);
	//void deleteMaterial(Material* mat);

	void registerGeometry(Geometry* geo);
	//void deleteGeometry(Geometry* geo);


private:

	friend class URE;


	Scene* mScene;
	RenderTarget* mGlobalRenderTarget;


	Map<String, InstanceManager*> mInstanceManagers;

	//Map<String, SimulationPipelineStage*> mRegisteredPipeLineStages;


	//WorldObjects and SubObjects have a unique 1:3 posession-relationship,
	//i.e. both Object types are't shared, so central maintainance is not necessary;
	//Map<ID, WorldObject*> mWorldObjects;
	//Map<ID, SubObject*>	mSubObjects;

	//Material can be used by multiple subobjects, so there must be a central maintainance
	Map<String, Material*> mMaterials;
	//Geometry can be used by up to two SubObjects without instancing,
	//namely the mechanical and the lighting representation of a worldobject
	//(CL_GL-Buffer Sharing)
	//WITHOUT usage of instancing the is no limit of re-usage of Geometry by several WorldObjects
	Map<String, Geometry*> mGeometries;


	//generic buffers are also shared, but rather on a local level; They are
	//stored centrally to resolve "posession" ambiguities;
	Map<ID, BufferInterface* > mBuffers;

	//Textures can be globally shared (as Rendertarget for multiple simulation stages, or by multiple materials),
	//and hence are stored centrally in order to
	//be dereferenced by name by a material; This is no "owning" data structure but
	//a subset of mBuffers for easier reference.
	Map<String, Texture*> mTextures;

	//OpenGL kernels and OpenGL Shaders stored centrally for multiple usage
	Map<String, MPP*>		mMPPs;
};

}

