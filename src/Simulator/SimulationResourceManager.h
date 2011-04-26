/*
 * SimulationResourceManager.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 *
 *  Manager and Container for all shared Objects.
 *  FYI: All getters return NULL if an object with the requested name doesn't exist;
 *  This will omit further boilerplate "xyExists(String name)" routines;
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



	SceneGraph* getSceneGraph()const;


	//when a Simulation pass nears its end, it should let do the instance managers the
	//"compiled rendering", as render()-calls to instanced geometry only registers drawing needs
	//to its instance manager; For every  (at least non-deferred non-Skybox Lighting-) rendering pass,
	//call this routine after scene graph traversal
	void executeInstancedRendering(SimulationPipelineStage* currentStage);

	void registerInstanceManager(InstanceManager* im);
	InstanceManager* getInstanceManager(String name);

	//automatically called by BufferInterface constructor
	void registerBufferInterface(BufferInterface* bi);
	BufferInterface* getBufferInterface(String name);
	//be very careful with this function, as there may be several references
	//and I (unfortunately!) don't work with smart pointers;
	//So do a manual deletion only if you are absolutely sure
	//that the buffer isn't used by other objects; This is true for all other shared objects
	//void deleteBufferInterface(BufferInterface* bi);

	void registerTexture(Texture* tex);
	Texture* getTexture(String name);
	//void deleteTexture(Texture* tex);
	void registerMPP(MPP* mpp);
	MPP* getMPP(String name);
	//void deleteMPP(MPP* mpp);
	void registerMaterial(Material* mat);
	Material* getMaterial(String name);
	//void deleteMaterial(Material* mat);
	void registerGeometry(Geometry* geo);
	Geometry* getGeometry(String name);
	//void deleteGeometry(Geometry* geo);

	//called by SkyDome constructor automatically;
	void registerSkydome(SkyDome* skyDome);
	inline SkyDome* getCurrentSkyDome()const{return mCurrentSkyDome;}

private:

	friend class URE;


	SceneGraph* mScene;
	IntermediateResultBuffersManager* mIntermediateResultsBuffersManager;


	std::map<String, InstanceManager*> mInstanceManagers;

	//Map<String, SimulationPipelineStage*> mRegisteredPipeLineStages;


	//WorldObjects and SubObjects have a unique 1:3 posession-relationship,
	//i.e. both Object types are't shared, so central maintainance is not necessary;
	//Map<ID, WorldObject*> mWorldObjects;
	//Map<ID, SubObject*>	mSubObjects;

	//Material can be used by multiple subobjects, so there must be a central maintainance
	std::map<String, Material*> mMaterials;
	//Geometry can be used by up to two SubObjects without instancing,
	//namely the mechanical and the lighting representation of a worldobject
	//(CL_GL-Buffer Sharing)
	//WITHOUT usage of instancing the is no limit of re-usage of Geometry by several WorldObjects
	std::map<String, Geometry*> mGeometries;


	//generic buffers are also shared, but rather on a local level; They are
	//stored centrally to resolve "posession" ambiguities between simulation domains;
	std::map<String, BufferInterface* > mBuffers;


	//Textures can be globally shared (as Rendertarget for multiple simulation stages, or by multiple materials),
	//and hence are stored centrally in order to
	//be dereferenced by name by a material; This is no "owning" data structure but
	//a subset of mBuffers for easier reference.
	std::map<String, Texture*> mTextures;

	//OpenGL kernels and OpenGL Shaders stored centrally for multiple usage
	std::map<String, MPP*>		mMPPs;


	SkyDome* mCurrentSkyDome;

};

}

