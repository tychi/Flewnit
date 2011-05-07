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


#include "Simulator/ParallelComputeManager.h"
#include "Simulator/SimulationResourceManager.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"


#include "Simulator/MechanicsSimulator/MechanicsSimulator.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"
#include "Simulator/SoundSimulator/SoundSimulator.h"

#include "Util/Loader/LoaderHelper.h"



#include <iostream>


#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>






namespace Flewnit {

//---------------------------------------------------------------------------------------------------------
///\brief static stuff
void URE::bootstrap(bool disableMemoryTrackLogging)
{
	//INSTANCIATE_SINGLETON(Log);
	new Log();

	if(disableMemoryTrackLogging)
	{
		LOG.disableLogLevel(MEMORY_TRACK_LOG_LEVEL);
	}

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

  mDoValidateSimulationStepResults(false),
  mDoProfileAndOptimizeSettings(false),
  mDoProfilePerformance(false),
  mDoDebugDraw(false),

  mConfig(0),
  mLoader(0),
  mWindowManager(0),
  mInputManager(0),
  mFPSCounter(0),
  mGUI(0),
  mParallelComputeManager(0),
  mSimulationResourceManager(0),
  mCLProgramManager(0),

  mGeometryTransformer(0)
{
//	mSimulators[MECHANICAL_SIM_DOMAIN]=0;
//	mSimulators[VISUAL_SIM_DOMAIN]=0;
//	mSimulators[ACUSTIC_SIM_DOMAIN]=0;
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

	mConfig = new Config();
	mLoader= new Loader();

	mLoader->loadGlobalConfig(*mConfig,pathToGlobalConfigFile);

#ifdef FLEWNIT_USE_GLFW
	mWindowManager = new GLFWWindowManager();
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

	mInputManager =  new InputManager();
	mFPSCounter = new FPSCounter();


	mParallelComputeManager = new ParallelComputeManager();


	mSimulationResourceManager =  new SimulationResourceManager();

	mCLProgramManager = new CLProgramManager(
		ConfigCaster::cast<bool>( mConfig->root().get("generalSettings",0).
				get("useCacheUsingOpenCLImplementation",0) )
	);

	if(mConfig->root().childExists("generalSettings",0))
	{
		mDoValidateSimulationStepResults = ConfigCaster::cast<bool>
			(mConfig->root().get("generalSettings",0).get("doValidateSimulationStepResults",0) );
		mDoProfileAndOptimizeSettings = ConfigCaster::cast<bool>
			(mConfig->root().get("generalSettings",0).get("doProfileAndOptimizeSettings",0) );
		mDoProfilePerformance = ConfigCaster::cast<bool>
			(mConfig->root().get("generalSettings",0).get("doProfilePerformance",0) );

		mDoDebugDraw = ConfigCaster::cast<bool>
		(mConfig->root().get("generalSettings",0).get("doDebugDraw",0) );
	}


	if(mConfig->root().childExists("simulators",0))
	{
		ConfigStructNode& simulatorsConfigNode = mConfig->root().get("simulators",0);


		//BOOST_FOREACH( ConfigMap::value_type & singleSimulatorConfigNode, simulatorsConfigNode.getChildren() )
		for(unsigned int i = 0 ; i < simulatorsConfigNode.get("Simulator").size() ;i++)
		{
			mSimulators.push_back(
					SimulatorInterface::create(
												simulatorsConfigNode.get("Simulator")[i]
					)
			);

//			assert("no double definition of a simulator allowed" && singleSimulatorConfigNode.second.size()== 1 );
//			mSimulators[singleSimulatorConfigNode.first] =
//					SimulatorInterface::create(singleSimulatorConfigNode.second[0]);
		}
	}
	else
	{
		assert("no simulators defined in config" && 0);

	}

	//DEBUG:
	getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->testStuff();

	//load the scene before the pipeline are initialized so that the pipelines have some
	//world objects/materials/geometry to grab for shader/kernel generation
	mLoader->loadScene();

	BOOST_FOREACH(SimulatorInterface* simulator, mSimulators)
	{
		simulator->initPipeLine();
	}

	BOOST_FOREACH(SimulatorInterface* simulator, mSimulators)
	{
		simulator->validatePipeLine();
	}
//	BOOST_FOREACH( SimulatorMap::value_type & simPair, mSimulators)
//	{
//		simPair.second -> initPipeLine();
//	}
//
//	BOOST_FOREACH( SimulatorMap::value_type & simPair, mSimulators)
//	{
//		simPair.second -> validatePipeLine();
//	}


	mCLProgramManager->getInstance().buildProgramsAndCreateKernels();


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



void URE::resetEngine()
{
	Log::getInstance()<<INFO_LOG_LEVEL<< "resetting URE;\n";

	Profiler::getInstance().updateMemoryTrackingInfo();

	mMainLoopQuitRequested = false;



//	for(int runner =0; runner < __NUM_SIM_DOMAINS__ ; runner ++)
//	{
//    	delete mSimulators[runner];
//	}

	BOOST_FOREACH(SimulatorInterface* simulator, mSimulators)
	{
		delete simulator;
	}

//	BOOST_FOREACH( SimulatorMap::value_type & simPair, mSimulators)
//	{
//		delete simPair.second ;
//	}

	//TODO delete classes
	//delete mGeometryConverter;

	delete mCLProgramManager;
	delete mSimulationResourceManager;
	delete mParallelComputeManager;


	delete mFPSCounter;

	delete mInputManager;
	delete mWindowManager;
	delete mLoader;
	delete mConfig;

	Profiler::getInstance().printRegisteredObjects();
}


SimulatorInterface* URE::getSimulator(SimulationDomain sd)const throw(SimulatorException)
{
	BOOST_FOREACH(SimulatorInterface* s, mSimulators)
	//for(int i=0; i< mSimulators.size();i++)
	{
		if(s->getSimDomain() == sd)
		//if(mSimulators[i]->getSimDomain() ==sd)
		{
			//return mSimulators[i];
			return s;
		}
	}
	throw(SimulatorException("Simulator with specified domain does not exist."));
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


	LOG<<INFO_LOG_LEVEL<<"Frame number: "<<mFPSCounter->getTotalRenderedFrames() <<"; ";

	mFPSCounter->newFrameStarted();

	//TODO


	BOOST_FOREACH(SimulatorInterface* simulator, mSimulators)
	{
		simulator->stepSimulation();
	}





	success = mInputManager->processInput();


	mFPSCounter->frameEnded();

	LOG<< INFO_LOG_LEVEL << "average FPS: " << mFPSCounter->getFPS(true) <<";\n";
	//LOG<< INFO_LOG_LEVEL << mFPSCounter->getFPS(false) << "last FPS;\n";
	//LOG<< INFO_LOG_LEVEL << mFPSCounter->getFPS(true) << " average FPS;\n";

	mMainLoopQuitRequested |= ( ! mWindowManager->windowIsOpened() );

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

Camera* URE::getCurrentlyActiveCamera()const
{
	return getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getCurrentlyActiveCamera();
}
void URE::setCurrentlyActiveCamera(Camera* cam)
{
	getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->setCurrentlyActiveCamera(cam);
}

//debug
bool URE::bufferDumpCondition()
{
	return
		false;
//			(mFPSCounter->getTotalRenderedFrames() <= 0)
//			||
//			(mFPSCounter->getTotalRenderedFrames() == 7)
//			||
//			(mFPSCounter->getTotalRenderedFrames() == 8)
//			;
//		(mFPSCounter->getTotalRenderedFrames() <= 2)
//		||
//		(
//			mFPSCounter->getTotalRenderedFrames()
//			>
//			130
//		);
}



}




