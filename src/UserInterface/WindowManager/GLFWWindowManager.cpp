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

#include "Util/Time/FPSCounter.h"

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
	mFPSCounter = new FPSCounter();

	//TODO

//	Vector2Di oglVersion = URE_INSTANCE->getConfig().root()["OpenGL_Settings"][0]["contextVersion"][0];
//
//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,
//			);

	//call this now already to assure a valid initial counter state, even if it distorts the first FPS values
	mFPSCounter->newFrameStarted();
}

void GLFWWindowManager::cleanup()
{
	glfwTerminate();
}


void GLFWWindowManager::swapBuffers()
{
	mFPSCounter->frameEnded();

	glfwSwapBuffers();

	mFPSCounter->newFrameStarted();
}


void GLFWWindowManager::toggleFullScreen()
{

}

void GLFWWindowManager::setMouseGrab(bool value)
{
	if(value)
		glfwEnable(GLFW_MOUSE_CURSOR);
	else
		glfwDisable(GLFW_MOUSE_CURSOR);
}


void GLFWWindowManager::setWindowTitle(String title)
{

}


float GLFWWindowManager::getLastFrameDuration()
{

}

float GLFWWindowManager::getFPS(bool averaged )
{
	return mFPSCounter->getFPS(averaged);
}

void GLFWWindowManager::createWindow(bool fullScreen, const Vector2Di& position, const Vector2Di& resolution)
{

}


}

#endif //FLEWNIT_USE_GLFW
