/*
 * MPP.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: tychi
 */

#include "MPP.h"

#include "Simulator/SimulationResourceManager.h"
#include "Util/Log/Log.h"

namespace Flewnit
{

MPP::MPP(String name, SimulationDomain sd)
: SimulationObject(name,sd)
{
	SimulationResourceManager::getInstance().registerMPP(this);
}

MPP::~MPP()
{
	// TODO Auto-generated destructor stub
}

//for later inspection of the final code of a stage:
void MPP::writeToDisk(String sourceCode, Path where)
{
	std::fstream fileStream;
	fileStream.open(where.string().c_str(), std::ios::out);
	fileStream << sourceCode;
	fileStream.close();
}

}
