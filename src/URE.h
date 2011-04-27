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

#include "Simulator/SimulatorMetaInfo.h"



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
    static void bootstrap(bool disableMemoryTrackLogging = false);
    static void cleanup();

    URE();
    virtual ~URE();

    //call after instantiation and after a successful reset, e.g. to parse a new config file and set everything up at runtime (if possible)
    bool init(Path& pathToGlobalConfigFile);
    bool init();

    void setInputInterpreter(InputInterpreter* interpreter);
    InputInterpreter* getInputInterpreter()const;

    void resetEngine();

    //library user's choice if she wants to control the steps herself or to enter a main loop;
    bool stepSimulation();

    bool enterMainLoop();
    //called by InputInterpreter:
    void requestMainLoopQuit(){mMainLoopQuitRequested = true;}


    inline GUI* getGUI()const{return mGUI;}
    inline GeometryTransformer* getGeometryConverter()const{return mGeometryTransformer;}
    inline Loader* getLoader()const{return mLoader;}
    inline WindowManager* getWindowManager()const{return mWindowManager;}
    inline SimulationResourceManager* getSimulationResourceManager()const{return mSimulationResourceManager;}
    inline int getNumSimulators()const{return mSimulators.size();}
    inline const FPSCounter* getFPSCounter()const{return mFPSCounter;}

    inline bool doValidateSimulationStepResults()const{return mDoValidateSimulationStepResults;}
    inline bool doProfileAndOptimizeSettings()const{return mDoProfileAndOptimizeSettings;}
    inline bool doProfilePerformance()const{return mDoProfilePerformance;}
    inline bool doDebugDraw()const{return mDoDebugDraw;}

    SimulatorInterface* getSimulator(SimulationDomain sd)const throw(SimulatorException);

    //intitialization per config won't be realized via annoying pointer passing to the constructors of all classes;
    //instead, the classes grab the Config from the URE Singleton if they need it.
    //cons: bad encapsulation, path to the desired information within the config-tree must be known by all classes;
    //pros: have lesser parameters to pass to many constructors;
    inline Config& getConfig()const{return *mConfig;}

    //shortcut to getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getCurrentlyActiveCamera()
    Camera* getCurrentlyActiveCamera()const;
    void setCurrentlyActiveCamera(Camera* cam);


    //inline cl::Context* getOpenCLContext()const{return mOpenCLContext;}


private:


    //bool buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile);


    bool mCorrectlyInitializedGuard;
    bool mMainLoopQuitRequested;

	bool mDoValidateSimulationStepResults;
	bool mDoProfileAndOptimizeSettings;
	bool mDoProfilePerformance;
	bool mDoDebugDraw;

    Config*					mConfig;
    Loader*					mLoader;

    WindowManager* 			mWindowManager;
    InputManager*			mInputManager;
    FPSCounter*				mFPSCounter;
    GUI* 					mGUI;

    //cl::Context*			mOpenCLContext;
    ParallelComputeManager* 		mParallelComputeManager;
    SimulationResourceManager*		mSimulationResourceManager;
    CLProgramManager*				mCLProgramManager;


    //typedef Map<String,SimulatorInterface*> SimulatorMap;
   	std::vector<SimulatorInterface*>	mSimulators;



	GeometryTransformer* 		mGeometryTransformer;


};

}

