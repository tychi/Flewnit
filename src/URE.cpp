/*
 * URE.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#include "URE.h"

#include "Util/Log/Log.h"

#include <iostream>

namespace Flewnit {

//---------------------------------------------------------------------------------------------------------
///\brief static stuff
void URE::bootstrap()
{

}

void URE::cleanup()
{

}
//---------------------------------------------------------------------------------------------------------


URE::URE()
: BASIC_OBJECT_CONSTRUCTOR("URE", "URESingletonInstance", "do the unified Rendering")
{
	// TODO Auto-generated constructor stub


}

URE::~URE()
{
	// TODO Auto-generated destructor stub
}



bool URE::init(boost::filesystem::path pathToGlobalConfigFile)
{
	std::cout<<"here is the URE lib!!1\n";

	std::cout<<"The Config Path is"<< pathToGlobalConfigFile.string() <<"\n";

}

void URE::resetEngine()
{

}


void URE::stepSimulation(SimStepSettings const& stepSettings)
{

}


//this routine will call the loader, which will in turn use assimp to load .blend files directly
bool URE::loadScene(boost::filesystem::path pathToSceneFile)
{

}



bool URE::buildSimulationPipeLine(boost::filesystem::path pathToPipelineConfigFile)
{

}


}
