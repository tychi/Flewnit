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

class Timer;

class GLFWWindowManager: public WindowManager
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:
	GLFWWindowManager();
	virtual ~GLFWWindowManager();

	virtual void init() ;
	virtual void cleanup();

	virtual void swapBuffers();

	virtual void toggleFullScreen();
	virtual void toggleMouseGrab();

	virtual void setWindowTitle(String title);

	virtual float getLastFrameDuration() ;
	virtual float getFPS(bool averaged );


protected:


	virtual void createWindow(bool fullScreen, const Vector2Di& position, const Vector2Di& resolution) = 0;

	Timer* mTimer;


};

}

#endif //FLEWNIT_USE_GLFW

