/*
 * DemoInputInterpreter.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "DemoInputInterpreter.h"

#include "UserInterface/Input/InputManager.h"


#include "URE.h"


#include "UserInterface/Input/Keyboard.h"
#include "UserInterface/Input/Mouse.h"
#include "UserInterface/Input/WiiMote.h"

#include "UserInterface/WindowManager/WindowManager.h"
//#include "Simulator/SimulationResourceManager.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"


#include "Simulator/SimulatorInterface.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"

#include "Util/Log/Log.h"



#ifdef FLEWNIT_USE_GLFW
#	include <GL/glfw.h>
#else

#endif

namespace Flewnit
{

DemoInputInterpreter::DemoInputInterpreter(float cameraLookMouseSensivity)
: mCameraLookMouseSensivity(cameraLookMouseSensivity)
{
	// TODO make cameraLookMouseSensivity  config file- configurable;

}

DemoInputInterpreter::~DemoInputInterpreter()
{
	// TODO Auto-generated destructor stub
}

void DemoInputInterpreter::interpretInput(Keyboard* keyboard)
{
	if(keyboard->getRecentKey() == GLFW_KEY_ENTER)
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS)
		{

			LOG<<DEBUG_LOG_LEVEL<<"enter pressed;\n";
			Vector2Dui currentRes = WindowManager::getInstance().getWindowResolution();
			LOG<<INFO_LOG_LEVEL<<"Resolution is now ("<<currentRes.x<<","<<currentRes.y<<");\n";
		}
		else
		{
			LOG<<DEBUG_LOG_LEVEL<<"enter released;\n";
		}
	}

	//if((unsigned char) (keyboard->getRecentKey() & 0xffff) == (int)('W'))
	if( (keyboard->getRecentKey()) == 'W')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS)
		{

			LOG<<DEBUG_LOG_LEVEL<<"w pressed;\n";
		}
		else
		{
			LOG<<DEBUG_LOG_LEVEL<<"w released;\n";
		}
	}


	if(keyboard->getRecentKey() == GLFW_KEY_ESC)
	{
		LOG<<DEBUG_LOG_LEVEL<<"escape pressed, shutting down;(;\n";
		InputManager::getInstance().setMouseHidden(false);
		URE_INSTANCE->requestMainLoopQuit();
	}

}

void DemoInputInterpreter::interpretInput(Mouse* mouse)
{

	static bool hideMouse= false;

	if(mouse->getRecentEvent() == Mouse::MOUSE_EVENT_BUTTON_CHANGED)
	{
		if(mouse->getRecentButton() == GLFW_MOUSE_BUTTON_LEFT )
		{
			if(mouse->getRecentButtonStatus() == GLFW_PRESS)
			{
				LOG<<DEBUG_LOG_LEVEL<<"left mouse button pressed;\n";
			}
			else
			{
				LOG<<DEBUG_LOG_LEVEL<<"left mouse button released;\n";
			}
		}

		if(mouse->getRecentButton() == GLFW_MOUSE_BUTTON_RIGHT )
		{
			if(mouse->getRecentButtonStatus() == GLFW_PRESS)
			{
				LOG<<DEBUG_LOG_LEVEL<<"right mouse button pressed; toggling mouse hide status;\n";
				hideMouse = !hideMouse;
				mouse->setHidden(hideMouse);
			}
			else
			{

			}
		}

	}
	else
	{
		if(mouse->getRecentEvent() == Mouse::MOUSE_EVENT_POSITION_CHANGED)
		{
			LOG<<DEBUG_LOG_LEVEL<<"mouse moved from ("
					<<mouse->getLastPosition().x
					<<","
					<<mouse->getLastPosition().y
					<<") to ("
					<<mouse->getRecentPosition().x
					<<","
					<<mouse->getRecentPosition().y
					<<");\n"
					;

			Camera* mainCamera = //SimulationResourceManager::getInstance().getMainCamera();
					URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

			//Matrix4x4 lookAtMatrix = mainCamera->getViewMatrix();
			float differenceHorizontal =  static_cast<float>(mouse->getRecentPosition().x - mouse->getLastPosition().x);
			float differenceVertical   =  static_cast<float>(mouse->getRecentPosition().y - mouse->getLastPosition().y);

			//self made initial sensivity rule of thumb: moving the mouse about half the screen
			//shall rotate the view about 90 degrees (pi/2); we assume a screen size of 1600*1600
			//pixels for this rule of thumb: a compromize between full hd and notebook displays ("HD ready ;( )
			//4* half thumbrule screen size= 4* 1600 / 2:
			const float pixelsCausingFullRotation = 3200.0f;
			float degreesToYaw = (360.0f / pixelsCausingFullRotation) * differenceHorizontal * mCameraLookMouseSensivity;
			//negative as mouse coord go from top to bottom
			float degreesToPitch = (-360.0f / pixelsCausingFullRotation) * differenceVertical * mCameraLookMouseSensivity;


			mainCamera->getGlobalTransform().yawRelativeToUpVector(degreesToYaw);
			mainCamera->getGlobalTransform().pitchRelativeToDirection(degreesToPitch);
		}
		else
		{
			if(mouse->getRecentEvent() == Mouse::MOUSE_EVENT_WHEEL_CHANGED)
			{
				WindowManager::getInstance().setWindowPosition(
						Vector2Di(mouse->getRecentWheelValue()+100, 500));
			}
		}
	}
}

void DemoInputInterpreter::interpretInput(WiiMote* wiiMote)
{
	//nothing to do yet;
}

}
