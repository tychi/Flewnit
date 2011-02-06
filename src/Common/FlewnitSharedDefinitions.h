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
#define FLEWNIT_DEFAULT_SHADER_SOURCES_PATH String(".src/MPP/Shader/ShaderSources/")

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

//stuff from time where i wasn't sure what math lib to use
//#define FLEWNIT_USE_QT_MATH 0
//#define FLEWNIT_USE_GLM_MATH 1


enum Access
{
	ACCESS_NONE,  //forbidden for buffer-to-host-mapping
	ACCESS_READ,
	ACCESS_WRITE, //forbidden for GUI stuff
	ACCESS_READWRITE
};

//no compressed/asymmetricly packed (e.g. GL_UNSIGNED_INT_10_10_10_2) types are supported in this framework
//WARNING: not all types enumerated here are guaranteed to be supported; this is rater a brainstorming list;
enum Type
{
	TYPE_UNDEF,

	TYPE_STRUCTURE,

	TYPE_BOOL,

	//stuff maybe needed for TextureBuffers
	TYPE_BOOL8,
	TYPE_BOOL16,
	TYPE_BOOL32,
	TYPE_BOOL64,

	TYPE_INT8, //signed char, 1 Byte
	TYPE_UINT8,

	TYPE_INT16,
	TYPE_UINT16,

	TYPE_INT32,
	TYPE_UINT32,

	TYPE_INT64,
	TYPE_UINT64,

	TYPE_HALF_FLOAT,
	TYPE_FLOAT,
	TYPE_DOUBLE,

	TYPE_STRING,

	//-----------------
	//all permutations of integer Vectors to come

	//type for (un)signed normalized RGBA Texel data
	TYPE_VEC2I8,
	TYPE_VEC2UI8,

	TYPE_VEC2I16,
	TYPE_VEC2UI16,

	TYPE_VEC2I32,
	TYPE_VEC2UI32,

	TYPE_VEC2I64,
	TYPE_VEC2UI64,

	TYPE_VEC3I8,
	TYPE_VEC3UI8,

	TYPE_VEC3I16,
	TYPE_VEC3UI16,

	TYPE_VEC3I32,
	TYPE_VEC3UI32,

	TYPE_VEC3I64,
	TYPE_VEC3UI64,

	//typical type for (un)signed normalized RGBA Texel data
	TYPE_VEC4I8,
	TYPE_VEC4UI8,

	TYPE_VEC4I16,
	TYPE_VEC4UI16,

	TYPE_VEC4I32,
	TYPE_VEC4UI32,

	TYPE_VEC4I64,
	TYPE_VEC4UI64,
	//-----------------
	//half type
	TYPE_VEC2F16,
	TYPE_VEC3F16,
	TYPE_VEC4F16,
	//-----------------

	TYPE_VEC2F,
	TYPE_VEC2D,

	TYPE_VEC3F,
	TYPE_VEC3D,

	TYPE_VEC4F,
	TYPE_VEC4D,

	TYPE_QUAT4F, // 4 floats encoding a quaternion {qx,qy,qz,qs}
	TYPE_QUAT4D, // 4 doubles encoding a quaternion {qx,qy,qz,qs}

	TYPE_MATRIX33F,
	TYPE_MATRIX33D,

	TYPE_MATRIX44F,
	TYPE_MATRIX44D,

	//semantics and encoding are treated otherwise ;)
//	TYPE_COLOR32, // 32 bits color. Order is RGBA if API is OpenGL or Direct3D10, and inversed if API is Direct3D9 (can be modified by defining 'colorOrder=...', see doc)
//	TYPE_COLOR3F, // 3 floats color. Order is RGB.
//	TYPE_COLOR4F, // 4 floats color. Order is RGBA.
//
//	TYPE_DIRECTION3F, // direction vector represented by 3 floats
//	TYPE_DIRECTION3D // direction vector represented by 3 doubles


};


}



