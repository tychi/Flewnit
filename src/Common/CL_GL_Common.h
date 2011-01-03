/*
 * CL_GL_Common.h
 *
 *  Created on: Dec 19, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

//#define GL_VERSION_3_3
#define GL3_PROTOTYPES 1
#include <GL3/gl3.h>
//omit inclusion of content of  "normal" GL/gl.h to enforce absence of non-gl3-core functions;
#define __gl_h_

//enable exception throwing, so that one gets immediatly informed about some weird stuff;
//this way, there is less danger to forget some "active" error polling;
//but be careful: gl error checking still has to be done the "good old way" ;(
#define __CL_ENABLE_EXCEPTIONS 1
#include <CL/cl.hpp>

#ifdef FLEWNIT_USE_GLFW
#	include <GL/glfw.h>
#endif


