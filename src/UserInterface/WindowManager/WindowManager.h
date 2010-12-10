/*
 * WindowManager.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 *
 * Interface class for window management
 */

#pragma once

#include "Common/BasicObject.h"
#include "Common/Math.h"

namespace Flewnit
{

class Config;

class WindowManager: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	explicit WindowManager(const Config& config);

	virtual ~WindowManager(){}


	void createGLContext();

	void setWindowTitle(String title);
	    void initScreen();

	    void createWindow();
	    void initFrameBuffer();
	    void createColorMap();
	    void createBlankCursor();
};

}

