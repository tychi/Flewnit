/*
 * URE.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#include "URE.h"

#include "Util/Log/Log.h"
#include "Common/Profiler.h"


#include "Util/Loader/Config.h"
#include "Util/Loader/Loader.h"

#ifdef FLEWNIT_USE_GLFW
#	include "UserInterface/WindowManager/GLFWWindowManager.h"
#else
	//nothing implemented yet ;)
#endif

#include "UserInterface/Input/InputManager.h"

#include "Util/Time/FPSCounter.h"


#include "Simulator/OpenCL_Manager.h"
#include "Simulator/SimulationResourceManager.h"


#include "Simulator/MechanicsSimulator/MechanicsSimulator.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"
#include "Simulator/SoundSimulator/SoundSimulator.h"




#include <iostream>


#include <boost/filesystem/path.hpp>







namespace Flewnit {

//---------------------------------------------------------------------------------------------------------
///\brief static stuff
void URE::bootstrap()
{
	//INSTANCIATE_SINGLETON(Log);
	new Log();
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	//INSTANCIATE_SINGLETON(Profiler);
	new Profiler();
	Profiler::getInstancePtr()->performBasicChecks();
#endif
}

void URE::cleanup()
{
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	DESTROY_SINGLETON(Profiler);
#endif
	DESTROY_SINGLETON(Log);
}
//---------------------------------------------------------------------------------------------------------


URE::URE()
//: BASIC_OBJECT_CONSTRUCTOR(URE, "URESingletonInstance", "do the unified Rendering")
: mCorrectlyInitializedGuard(false),
  mMainLoopQuitRequested(false),
  mConfig(0),
  mLoader(0),
  mWindowManager(0),
  mInputManager(0),
  mFPSCounter(0),
  mGUI(0),
  mOpenCL_Manager(0),
  mSimulationResourceManager(0),

  mGeometryConverter(0)
{
	mSimulators[MECHANICAL_SIM_DOMAIN]=0;
	mSimulators[VISUAL_SIM_DOMAIN]=0;
	mSimulators[ACUSTIC_SIM_DOMAIN]=0;
}

URE::~URE()
{
	resetEngine();
}



bool URE::init(Path& pathToGlobalConfigFile)
{
	resetEngine();

	Log::getInstance()<<INFO_LOG_LEVEL<<"initializing URE ...\n";
	Log::getInstance()<<DEBUG_LOG_LEVEL<<"The Config Path is"<< pathToGlobalConfigFile.string() <<"\n";

	mConfig = FLEWNIT_INSTANTIATE(new Config());
	mLoader= FLEWNIT_INSTANTIATE(new Loader());

	mLoader->loadGlobalConfig(*mConfig,pathToGlobalConfigFile);

#ifdef FLEWNIT_USE_GLFW
	mWindowManager = FLEWNIT_INSTANTIATE(new GLFWWindowManager());
#else
#	ifdef	FLEWNIT_USE_XCB
	assert(0 && "Sorry, XCB Windowmanager not implemented");
#	else
#		ifdef FLEWNIT_USE_SDL
			assert(0 && "Sorry, SDL Windowmanager not implemented");
#		else
			assert(0 && "You have to specifiy window manager with cmake!");
#		endif
#	endif
#endif

	mInputManager =  FLEWNIT_INSTANTIATE(new InputManager());
	mFPSCounter = FLEWNIT_INSTANTIATE(new FPSCounter());


	mOpenCL_Manager = new OpenCL_Manager();
	mSimulationResourceManager = new SimulationResourceManager();


	mSimulators[MECHANICAL_SIM_DOMAIN]= FLEWNIT_INSTANTIATE(new MechanicsSimulator());
	mSimulators[VISUAL_SIM_DOMAIN]=FLEWNIT_INSTANTIATE(new LightingSimulator());
	mSimulators[ACUSTIC_SIM_DOMAIN]=FLEWNIT_INSTANTIATE(new SoundSimulator());

	for(int runner = 0; runner < __NUM_SIM_DOMAINS__; runner ++)
	{
		mSimulators[runner] -> initPipeLine();
	}

	for(int runner = 0; runner < __NUM_SIM_DOMAINS__; runner ++)
	{
		mSimulators[runner] -> validatePipeLine();
	}


#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	Profiler::getInstance().updateMemoryTrackingInfo();
#endif


	Log::getInstance()<<INFO_LOG_LEVEL<<"initializing done!\n";

	//hack
	mCorrectlyInitializedGuard = true;

	return mCorrectlyInitializedGuard;
}

bool URE::init()
{
	Path dummy(FLEWNIT_DEFAULT_CONFIG_PATH);
	return init(dummy  );
}

void URE::createOpenCLContext()
{
	//TODO
	LOG<<DEBUG_LOG_LEVEL << " OpenCL Context Setup TODO ;(; ";
}


void URE::resetEngine()
{
	Log::getInstance()<<INFO_LOG_LEVEL<< "resetting URE;\n";

	Profiler::getInstance().updateMemoryTrackingInfo();

	mMainLoopQuitRequested = false;



	for(int runner =0; runner < __NUM_SIM_DOMAINS__ ; runner ++)
	{
    	delete mSimulators[runner];
	}

	//TODO delete classes
	//delete mGeometryConverter;

	delete mSimulationResourceManager;
	delete mOpenCL_Manager;


	delete mFPSCounter;

	delete mInputManager;
	delete mWindowManager;
	delete mLoader;
	delete mConfig;

	Profiler::getInstance().printRegisteredObjects();
}

bool URE::enterMainLoop()
{
	//TODO
	bool sucess =true;

	while (sucess &&  (! mMainLoopQuitRequested))
	{
		sucess = stepSimulation();
	}

	return sucess;
}


bool URE::stepSimulation()
{
	bool success =true;
	assert(mCorrectlyInitializedGuard);

	mFPSCounter->newFrameStarted();

	//TODO

	success = mSimulators[MECHANICAL_SIM_DOMAIN] -> stepSimulation();

	success = mSimulators[VISUAL_SIM_DOMAIN] -> stepSimulation();

	success =mSimulators[ACUSTIC_SIM_DOMAIN] -> stepSimulation();

	mWindowManager->swapBuffers();

	success = mInputManager->processInput();


	mFPSCounter->frameEnded();

	//LOG<< INFO_LOG_LEVEL << mFPSCounter->getFPS(false) << "last FPS;\n";
	//LOG<< INFO_LOG_LEVEL << mFPSCounter->getFPS(true) << " average FPS;\n";

	mMainLoopQuitRequested &= mWindowManager->windowIsOpened();

	return success;

}


void URE::setInputInterpreter(InputInterpreter* interpreter)
{
	mInputManager->setInputInterpreter(interpreter);
}

InputInterpreter* URE::getInputInterpreter()const
{
	if(mInputManager)
	{
		return mInputManager->getInputInterpreter();
	}
	else
	{
		return NULL;
	}
}


//bool URE::buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile)
//{
//	//TODO
//	assert(mCorrectlyInitializedGuard);
//
//	return false;
//}


}
