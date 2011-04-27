/*
 * BasicCLProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "BasicCLProgram.h"

namespace Flewnit
{


BasicCLProgram::BasicCLProgram(
		Path sourceFileName,
		SimulationDomain sd,
		Path codeDirectory,
		Path programCodeSubFolderName
)
: CLProgram(sourceFileName, sd, codeDirectory, programCodeSubFolderName)
{
	//nothing to do
}

BasicCLProgram::~BasicCLProgram()
{
	//everything done by base class
}

void BasicCLProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	//TODO
	assert(0&&"TODO implement");
}

}
