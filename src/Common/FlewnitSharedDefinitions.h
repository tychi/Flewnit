/*
 * FlewnitSharedDefinitions.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 *
 *  Contains definitions used throughout the whole project and using applications.
 *
 */

#pragma once

//maybe we want use qt string or boost strings later..
#include <string>
//maybe there are better containers than std::vector, but to begin...
#include <vector>
#include <map>

namespace Flewnit
{

//Media Layer Stuff; Shall be delegated by Cmake later ;)
//#define FLEWNIT_USE_SDL
//#define FLEWNIT_USE_XCB

#define FLEWNIT_DEFAULT_CONFIG_PATH "../config/FlewnitConfig.xml"

#define FLEWNIT_DEFAULT_LOG_FILEPATH "./flewnitLog.txt"




//-----------------------------------------------------------------------
//do some typedefs in order to be as independent from library specific types as possible
typedef float 				Scalar;
typedef std::string 		String;
typedef unsigned int 		ID;
typedef unsigned int		uint;
#define List std::vector
#define Map std::map

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
///\brief profinling stuff
#define FLEWNIT_TRACK_MEMORY 1
#define FLEWNINT_DO_PROFILING 1
//-----------------------------------------------------------------------




}



