/*
 * GLFWWindowManager.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */


#include "GLFWWindowManager.h"

#ifdef FLEWNIT_USE_GLFW

#include "GL/glfw.h"

#include "URE.h"
#include "Util/Loader/Config.h"
#include "Common/Math.h"

#include "Util/Timer.h"

namespace Flewnit
{

GLFWWindowManager::GLFWWindowManager()
{
	init();

}

GLFWWindowManager::~GLFWWindowManager()
{
	cleanup();
}

void GLFWWindowManager::init()
{
	glfwInit();
	mTimer = Timer::create();

	//TODO

//	Vector2Di oglVersion = URE_INSTANCE->getConfig().root()["OpenGL_Settings"][0]["contextVersion"][0];
//
//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,
//			);
}

void GLFWWindowManager::cleanup()
{
	glfwTerminate();
}


void GLFWWindowManager::swapBuffers()
{

}


void GLFWWindowManager::toggleFullScreen()
{

}

void GLFWWindowManager::toggleMouseGrab()
{

}


void GLFWWindowManager::setWindowTitle(String title)
{

}


float GLFWWindowManager::getLastFrameDuration()
{

}

float GLFWWindowManager::getFPS(bool averaged )
{

}

void GLFWWindowManager::createWindow(bool fullScreen, const Vector2Di& position, const Vector2Di& resolution)
{

}


}

#endif //FLEWNIT_USE_GLFW
