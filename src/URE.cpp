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
  mConfig(0),
  mLoader(0),
  mWindowManager(0),
  mInputManager(0),
  mGUI(0),
  mSimulationDataBase(0),
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

	mLoader->loadGlobalConfig(mConfig,pathToGlobalConfigFile);

#ifdef FLEWNIT_USE_GLFW
	//mWindowManager = FLEWNIT_INSTANTIATE(new GLFWWindowManager());
	//mInputManager =  FLEWNIT_INSTANTIATE(new GLFWInputManager());
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


	Log::getInstance()<<INFO_LOG_LEVEL<<"initializing done!\n";

	return mCorrectlyInitializedGuard;
}

bool URE::init()
{
	Path dummy(FLEWNIT_DEFAULT_CONFIG_PATH);
	init(dummy  );
}

void URE::resetEngine()
{
	Log::getInstance()<<INFO_LOG_LEVEL<< "resetting URE;\n";


//	delete mInputManager;
//	delete mWindowManager;
	delete mLoader;
	delete mConfig;
}

bool URE::enterMainLoop()
{

}


void URE::stepSimulation(SimStepSettings const& stepSettings)
{
	//TODO
	assert(mCorrectlyInitializedGuard);
}




bool URE::buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile)
{
	//TODO
	assert(mCorrectlyInitializedGuard);
}


}
