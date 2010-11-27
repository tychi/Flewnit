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

namespace Flewnit
{

//Media Layer Stuff; Shall be delegated by Cmake later ;)
//#define FLEWNIT_USE_SDL
//#define FLEWNIT_USE_XCB

#define FLEWNIT_DEFAULT_CONFIG_PATH "../config/FlewnitConfig.xml"

#define FLEWNIT_DEFAULT_LOG_FILEPATH "./flewnitLog.txt"


#define FLEWNIT_TRACK_MEMORY 1
#define FLEWNINT_DO_PROFILING 1

#if FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING
//we need the basic object for tracking purposes
// 	call INHERIT_BASIC_OBJECT(,) or INHERIT_BASIC_OBJECT(:) dependent on the position of (multiple) inheritance
//#	define INHERIT_BASIC_OBJECT(predecessor) #predecessor public BasicObject
#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) BasicObject(className, objectname, purposeDescription)
#else
//define the stuff to nothing in order to save overhead
//#	define INHERIT_BASIC_OBJECT(predecessor)
#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) BasicObject()
#endif

//default contructor for basic object; not works;
//#define BASIC_OBJECT_CONSTRUCTOR() BasicObject()



//-----------------------------------------------------------------------
//do some typedefs in order to be as independent from library specific types as possible
typedef float 				Scalar;
typedef std::string 		String;
typedef unsigned int 		ID;
typedef std::vector 		List;

//-----------------------------------------------------------------------



}



