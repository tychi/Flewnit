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
#include <stack>

namespace Flewnit
{

//Media Layer Stuff; Shall be delegated by Cmake later ;)
//#define FLEWNIT_USE_SDL
//#define FLEWNIT_USE_XCB

#define FLEWNIT_DEFAULT_CONFIG_PATH "../config/FlewnitConfig.xml"

#define FLEWNIT_DEFAULT_LOG_FILEPATH "./flewnitLog.txt"

#define FLEWNIT_UNSPECIFIED_NAME "none_specified"
//convert c++-code to a string ;)
#define FLEWNIT_STRINGIFY(word) Flewnit::String(#word)

#define FLEWNIT_FLAGIFY(value) (1 << value)



//-----------------------------------------------------------------------
//do some typedefs in order to be as independent from library specific types as possible
typedef float 				Scalar;
typedef std::string 		String;
//for "invalid ID" tagging, we use -1, so we typedef the ID stuff to int instead of insigned int
typedef int 				ID;
typedef unsigned int		uint;
#define List std::vector
#define Map std::map
#define Stack std::stack

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
///\brief profiling stuff
#define FLEWNIT_TRACK_MEMORY 1
#define FLEWNIT_DO_PROFILING 1
#define FLEWNIT_DO_CONSOLE_DEBUG_OUTPUT 1
//-----------------------------------------------------------------------

//there is no alternatic in sight now, but let's consider there might be one ;)
#define FLEWNIT_USE_QT_MATH 1


}



