/*
 * URE.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 *
 *	\brief The heart of Flewnit: The Rending Engine trying a "unified approach";
 *
 *	\details
 *	The class with the overblown name "URE", short for "Unified Rendering Engine"; Why unified? Because the long term goal of this Engine is to overcome
 *	the asymmetric treatment of "Rendering" and "Physics Simulation"; We should either talk about Image rendering and mechanical rendering or about lighting simulation
 *	and mechanical simulation; Hence, it is tried to keep the data structures as symmetric as possible,Geometry, Material, and so on;
 *	In the far future, one could try to realize and conntect differen Physics Simulation techniques, like Soft Bodies, Cloth, Hair etc. The rendering pipleline shoul allow this.
 *
 *
 *	Furthermore, it is tried to treat the "buffer concept" as generic as possible, i.e. tu nuify arrays,textures, vertex buffers, OpenCL buffers etc.
 *
 *	It is also tried to keep the Rendering pipeline as flexible as possible, so that one can write Pipeline-Stage-Plugins (at the moment just realised as Classes, atual Plugin stuff
 *	won't be implemented in the first place), which validate themselves against the rest of the Pipeline, so that various simulations can be established;
 *
 *
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"
#include "Common/Singleton.h"
#include "Common/BasicObject.h"


#include "Simulator/SimulatorForwards.h"

#include <boost/filesystem/path.hpp>

namespace Flewnit {

//shortcut for instance Pointer
#define URE_INSTANCE Flewnit::URE::getInstancePtr()





class URE :
	public Singleton<URE>,
	public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:
    static void bootstrap();
    static void cleanup();

    URE();
    virtual ~URE();

    bool init(boost::filesystem::path pathToGlobalConfigFile = boost::filesystem::path(FLEWNIT_DEFAULT_CONFIG_PATH));

    void resetEngine();
    void stepSimulation(SimStepSettings const& stepSettings);
    bool enterMainLoop();


    inline GUI* getGUI()const{return mGUI;}
    inline GeometryConverter* getGeometryConverter()const{return mGeometryConverter;}
    inline Loader* getLoader()const{return mLoader;}
    inline WindowManager* getWindowManager()const{return mWindowManager;}
    inline SimulationDataBase* getSimulationDataBase()const{return mSimulationDataBase;}
    inline SimulatorInterface* getSimulator(SimulationDomain which)const{ assert(which < __NUM_SIM_DOMAINS__); return mSimulators[which];}




private:

    //this routine will call the loader, which will in turn use assimp to load .blend files directly;
    //bool loadScene(boost::filesystem::path pathToSceneFile);

    bool buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile);


    bool 					mCorrectlyInitializedGuard;
    Loader*					mLoader;

    WindowManager* 			mWindowManager;
    InputManager*			mInputManager;
    GUI* 					mGUI;

    SimulatorInterface*		mSimulators[__NUM_SIM_DOMAINS__];
    SimulationDataBase*		mSimulationDataBase;


	GeometryConverter* 		mGeometryConverter;



};

}

