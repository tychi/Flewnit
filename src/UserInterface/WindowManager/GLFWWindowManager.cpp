/*
 * GLFWWindowManager.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */


#include "GLFWWindowManager.h"

#ifdef FLEWNIT_USE_GLFW

//#include <GL/glfw.h>
#include "Common/CL_GL_Common.h"

#include "URE.h"
#include "Util/Loader/Config.h"
#include "Util/Loader/LoaderHelper.h"
#include "Common/Math.h"

#include "Util/Time/FPSCounter.h"
#include "Util/HelperFunctions.h"

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


bool GLFWWindowManager::windowIsOpened()
{
	return	(glfwGetWindowParam(GLFW_OPENED)  == GL_TRUE ) ? true : false;
}


void GLFWWindowManager::init()
{
	glfwInit();
	mFPSCounter = new FPSCounter();

	//TODO

	//all invalid or values we don't want (in order to check config pasring success)
	Vector2Di oglVersion(0,0);
	Vector2Di winPos(0,0);
	Vector2Di winRes(300,500);
	bool enforceCoreProfile = false;
	bool grabMouse = false;
	bool fullscreen = false;
	String windowTitle = "myASS";

	int numMultiSamples = 1;


	try
	{

		oglVersion =
			ConfigCaster::cast<Vector2Di> (
					URE_INSTANCE->getConfig().root().
					get("UI_Settings",0).
					get("OpenGL_Settings",0).
					get("contextVersion",0)
			);

		enforceCoreProfile =
			ConfigCaster::cast<bool> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("OpenGL_Settings",0).
				get("enforceCoreProfile",0)
		);

		winPos =
			ConfigCaster::cast<Vector2Di> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("windowSettings",0).
				get("position",0)
		);

		winRes =
			ConfigCaster::cast<Vector2Di> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("windowSettings",0).
				get("resolution",0)
		);

		fullscreen =
			ConfigCaster::cast<bool> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("windowSettings",0).
				get("fullScreen",0)
		);


		grabMouse =
			ConfigCaster::cast<bool> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("inputSettings",0).
				get("grabMouse",0)
		);


		windowTitle	=
			ConfigCaster::cast<String> (
				URE_INSTANCE->getConfig().root().
				get("UI_Settings",0).
				get("windowSettings",0).
				get("windowTitle",0)
		);

		numMultiSamples =
				ConfigCaster::cast<int> (
					URE_INSTANCE->getConfig().root().
					get("UI_Settings",0).
					get("windowSettings",0).
					get("numMultiSamples",0)
			);

	}
	catch(ConfigCastException e)
	{
		LOG<<ERROR_LOG_LEVEL<<"bad cast in config parsing!\n";
		assert(0);
	}


//	Vector2Di oglVersion = URE_INSTANCE->getConfig().root()["OpenGL_Settings"][0]["contextVersion"][0];
//
	glfwOpenWindowHint(
			GLFW_OPENGL_VERSION_MAJOR,
			oglVersion.x
	);

	glfwOpenWindowHint(
			GLFW_OPENGL_VERSION_MINOR,
			oglVersion.y
	);

	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
			enforceCoreProfile ? GL_TRUE : GL_FALSE);

	//antialiasing stuff; test if it works after first triangle rendering
	assert(HelperFunctions::isPowerOfTwo(numMultiSamples));
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,numMultiSamples <= 1 ? 0 : numMultiSamples);

	createWindow(fullscreen,winPos,winRes);

	setMouseGrab(grabMouse);

	setWindowTitle(windowTitle);

	glfwSetWindowSizeCallback(windowChangeCallback);


	setWindowPosition(winPos);

	printInfo();

	//call this now already to assure a valid initial counter state, even if it distorts the first FPS values
	mFPSCounter->newFrameStarted();
}

void GLFWWindowManager::cleanup()
{
	delete mFPSCounter;

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
	LOG<<WARNING_LOG_LEVEL<<"Fullscreen toogle not possible with GLFW!\n";
}

void GLFWWindowManager::setMouseGrab(bool value)
{
	if(value)
		glfwDisable(GLFW_MOUSE_CURSOR);
	else
		glfwEnable(GLFW_MOUSE_CURSOR);
}


void GLFWWindowManager::setWindowTitle(String title)
{
	glfwSetWindowTitle(title.c_str());
}


double GLFWWindowManager::getLastFrameDuration()
{
	return mFPSCounter->getLastFrameDuration();
}

double GLFWWindowManager::getFPS(bool averaged )
{
	return mFPSCounter->getFPS(averaged);
}

void GLFWWindowManager::createWindow(bool fullScreen, const Vector2Di& position, const Vector2Di& resolution)
{
	glfwOpenWindow(resolution.x,resolution.y,8,8,8,8,32,8,
			fullScreen? GLFW_FULLSCREEN : GLFW_WINDOW );
}

void GLFWWindowManager::setWindowPosition(Vector2Di newPos)
{
	glfwSetWindowPos(newPos.x,newPos.y);
}


void GLFWWindowManager::windowChangeCallback(int newResX, int newResY)
{
	LOG<<INFO_LOG_LEVEL<<"Resolution is now ("<<newResX<<","<<newResY<<");\n";
}

Vector2Dui GLFWWindowManager::getWindowResolution()
{
	int actualResX, actualResY;
	glfwGetWindowSize(&actualResX,&actualResY);
	return Vector2Dui(actualResX,actualResY);
}


void GLFWWindowManager::printInfo()
{
	LOG<< INFO_LOG_LEVEL << "Supported OpenGL and installed driver Version: " <<  glGetString(GL_VERSION)<<"\n";

	GLint  majorVersion,minorVersion;
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	LOG<< INFO_LOG_LEVEL << "Used OpenGL Version: " << majorVersion<<"."<<minorVersion<<"\n";


	LOG<< INFO_LOG_LEVEL << "Used GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)<<"\n";


	LOG<< INFO_LOG_LEVEL << "Hardware vendor: "<<glGetString(GL_VENDOR)<<"\n";
	LOG<< INFO_LOG_LEVEL << "Rendering device: "<<glGetString(GL_RENDERER)<<"\n";


//	GLint maxVertexTextures, maxFragmentTextures,MajorVersion,MinorVersion,numext,pointSize;
//	glGetIntegerv(GL_POINT_SIZE, &pointSize);
//    Logger::Instance().message << pointSize;
//    Logger::Instance().log("MESSAGE","Point Size");
//

//	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,&maxTex1);
//	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&maxTex2);
//    Logger::Instance().message << maxTex1 << " " << maxTex2;
//    Logger::Instance().log("MESSAGE","MaxTex");
//
//	glGetIntegerv(GL_NUM_EXTENSIONS, &numext);
//    Logger::Instance().message << "Found " << numext << " GL_EXTENSIONS";
//    Logger::Instance().log("MESSAGE","GL_EXTENSIONS");

//	for (int i = 0; i < numext; i++) {
//		Logger::Instance().message << glGetStringi(GL_EXTENSIONS,i);
//		Logger::Instance().log("DEBUG","GL_EXTENSIONS");
//	}
}


}

#endif //FLEWNIT_USE_GLFW
