/*
 * GLFWWindowManager.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "WindowManager.h"

#ifdef FLEWNIT_USE_GLFW

namespace Flewnit
{

class FPSCounter;

class GLFWWindowManager
: public WindowManager
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:
	GLFWWindowManager();
	virtual ~GLFWWindowManager();

	virtual void init() ;
	virtual void cleanup();

	virtual void swapBuffers();

	virtual void toggleFullScreen();
	virtual void setMouseGrab(bool value);

	virtual void setWindowTitle(String title);

	virtual double getLastFrameDuration() ;
	virtual double getFPS(bool averaged );


	virtual Vector2Dui getWindowResolution();
	virtual bool windowIsOpened();

	virtual void setWindowPosition(Vector2Di newPos);
	virtual void printInfo();

	static void windowChangeCallback(int newResX, int newResY);

	virtual bool openGLContextIsCreated();
	virtual Vector2Di getAvailableOpenGLVersion();

	virtual void drawFullScreenQuad();

protected:


	virtual void createWindow(bool fullScreen, const Vector2Di& position, const Vector2Di& resolution) ;

	FPSCounter* mFPSCounter;
	bool mGLContextCreatedGuard;

	UnitQuad* mFullScreenQuadGeom;

};

}

#endif //FLEWNIT_USE_GLFW

