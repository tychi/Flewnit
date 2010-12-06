/*
 * URE.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#include "URE.h"

#include "Util/Log/Log.h"


#include "Common/Profiler.h"


#include <iostream>

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
	// TODO Auto-generated destructor stub
}



bool URE::init(boost::filesystem::path pathToGlobalConfigFile)
{

	Log::getInstance()<<INFO_LOG_LEVEL<<"here is the URE lib!!1\n";

	//mLoader=FLEWNIT_INSTANTIATE(new Loader,(pathToGlobalConfigFile));
	sizeof(*(new float()));

	Log::getInstance()<<DEBUG_LOG_LEVEL<<"The Config Path is"<< pathToGlobalConfigFile.string() <<"\n";


	return mCorrectlyInitializedGuard;
}

void URE::resetEngine()
{
	//TODO
}


void URE::stepSimulation(SimStepSettings const& stepSettings)
{
	//TODO
}




bool URE::buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile)
{
	//TODO
}


}
