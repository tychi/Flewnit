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

//gloabal defines; hack in cmake script in order to provide code completion in an IDE dependent of user's build choice;
#include "cmakeGeneratedDefinitions.h"

#include <boost/filesystem/path.hpp>
//forward hack in order not to pollute ALL headers in project with the big boost header stuff; Compile and link time is everything ;)
//namespace boost {
//	namespace filesystem {
//		template<class,class> class basic_path;
//		struct path_traits;
//		typedef basic_path< std::string, path_traits > path;
//	}
//}




namespace Flewnit
{

typedef boost::filesystem::path Path;

//Media Layer Stuff; delegated by cmake;
//but let's define it here anyway in order to have some code completion;
//#define FLEWNIT_USE_GLFW
//#define FLEWNIT_USE_XCB
//#define FLEWNIT_USE_SDL


#define FLEWNIT_DEFAULT_CONFIG_PATH String("./config/FlewnitConfig.xml")

#define FLEWNIT_DEFAULT_LOG_FILEPATH "./flewnitLog.txt"

#define FLEWNIT_INVALID_ID ((ID) (-1))
#define FLEWNIT_UNSPECIFIED_NAME "none_specified"

//convert c++-code to a string ;)
#define FLEWNIT_STRINGIFY(word) Flewnit::String(#word)

#define FLEWNIT_FLAGIFY(value) (1 << value)

//if this value is exceeded, some methods may issue a warning, an error or trunk their timing valus (e.g. to to omit degenerate input interpretation)
#define FLEWNIT_NON_RESPONSIVE_WARNING_TIME 0.5



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
#define Pair std::pair


//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
///\brief profiling stuff
#define FLEWNIT_TRACK_MEMORY 1
#define FLEWNIT_DO_PROFILING 1
#define FLEWNIT_DO_CONSOLE_DEBUG_OUTPUT 1
//-----------------------------------------------------------------------

//there is no alternatic in sight now, but let's consider there might be one ;)
#define FLEWNIT_USE_QT_MATH 0
#define FLEWNIT_USE_GLM_MATH 1


}



