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

//OpenCL context forward:
namespace cl
{
	class Context;
}


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

    //call after instantiation and after a successful reset, e.g. to parse a new config file and set everything up at runtime (if possible)
    bool init(Path& pathToGlobalConfigFile);
    bool init();

    void registerInputInterpreter(InputInterpreter* interpreter);

    void resetEngine();

    //library user's choice if she wants to control the steps herself or to enter a main loop;
    bool stepSimulation();

    bool enterMainLoop();
    //called by InputInterpreter:
    void requestMainLoopQuit(){mMainLoopQuitRequested = true;}


    inline GUI* getGUI()const{return mGUI;}
    inline GeometryConverter* getGeometryConverter()const{return mGeometryConverter;}
    inline Loader* getLoader()const{return mLoader;}
    inline WindowManager* getWindowManager()const{return mWindowManager;}
    inline SimulationDataBase* getSimulationDataBase()const{return mSimulationDataBase;}
    inline SimulatorInterface* getSimulator(SimulationDomain which)const{ assert(which < __NUM_SIM_DOMAINS__); return mSimulators[which];}

    //intitialization per config won't be realized via annoying pointer passing to the constructors of all classes;
    //instead, the classes grab the Config from the URE Singleton if the need it^^.
    //cons: bad encapsulation;
    //pros: have lesser parameters to pass to many constructors;
    inline const Config& getConfig()const{return *mConfig;}

    inline cl::Context* getOpenCLContext()const{return mOpenCLContext;}


private:

    void createOpenCLContext();
	//not necessary due to the OO-c++-binding :)
	//void releaseOpenCLContext();

    bool buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile);


    bool 					mCorrectlyInitializedGuard;
    bool					mMainLoopQuitRequested;

    Config*					mConfig;
    Loader*					mLoader;

    WindowManager* 			mWindowManager;
    InputManager*			mInputManager;
    FPSCounter*				mFPSCounter;
    GUI* 					mGUI;

    cl::Context*			mOpenCLContext;

    SimulatorInterface*		mSimulators[__NUM_SIM_DOMAINS__];
    SimulationDataBase*		mSimulationDataBase;


	GeometryConverter* 		mGeometryConverter;



};

}

