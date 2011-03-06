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
#include "Simulator/OpenCL_Manager.h"
#include "Util/Time/FPSCounter.h"
#else

#endif

namespace Flewnit
{

DemoInputInterpreter::DemoInputInterpreter(float cameraLookMouseSensivity, float movementSpeed)
: mCameraLookMouseSensivity(cameraLookMouseSensivity),mMovementSpeed(movementSpeed),
  mHideMouse(false)
{
	// TODO make cameraLookMouseSensivity  config file- configurable;

	mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_STILL;
	mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_STILL;
	mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_STILL;
}

DemoInputInterpreter::~DemoInputInterpreter()
{
	// TODO Auto-generated destructor stub
}

//if no input is done, the interpretInput methods aren't called; to do stuff like continous
//update of certain variables, this method is called after every frame
void DemoInputInterpreter::perFrameCallback()
{
	//---------------------------------------------------------

//	URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera()

	URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().moveRelativeToDirection(
			(static_cast<float>(mMovementState[MOVEMENT_DIRECTION_FRONT_BACK]) 	- 1.0f)
			* -1.0f *mMovementSpeed * URE_INSTANCE->getFPSCounter()->getLastFrameDuration(),
			(static_cast<float>(mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT]) 	- 1.0f)
			* -1.0f * mMovementSpeed * URE_INSTANCE->getFPSCounter()->getLastFrameDuration(),
			(static_cast<float>(mMovementState[MOVEMENT_DIRECTION_UP_DOWN])  	- 1.0f)
			*mMovementSpeed * URE_INSTANCE->getFPSCounter()->getLastFrameDuration()
	);
	//---------------------------------------------------------
}

void DemoInputInterpreter::interpretInput(Keyboard* keyboard)
{
//	Camera* mainCamera =
//		URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

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


	if( (keyboard->getRecentKey()) == 'W')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_FORWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_STILL;
		}
	}
	if( (keyboard->getRecentKey()) == 'S')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_BACKWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_STILL;
		}
	}

	if( (keyboard->getRecentKey()) == 'D')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_FORWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_STILL;
		}
	}
	if( (keyboard->getRecentKey()) == 'A')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_BACKWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_STILL;
		}
	}

	if( (keyboard->getRecentKey()) == 'Q')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_BACKWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_STILL;
		}
	}
	if( (keyboard->getRecentKey()) == 'E')
	{
		if(keyboard->getRecentStatus() == GLFW_PRESS){
			mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_FORWARD;
		}
		else{
			mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_STILL;
		}
	}

	//----------------------------------------------------------------------------
	if(keyboard->getRecentKey() == GLFW_KEY_F2)
	{
		GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
	}
	if(keyboard->getRecentKey() == GLFW_KEY_F3)
	{
		GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	if(keyboard->getRecentKey() == GLFW_KEY_F4)
	{
		GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}

	if((keyboard->getRecentKey() == GLFW_KEY_SPACE) && (keyboard->getRecentStatus() == GLFW_PRESS))
	{
		mHideMouse = !mHideMouse;
	}


	//--------------------------------------------------------------------


	if(keyboard->getRecentKey() == GLFW_KEY_ESC)
	{
		LOG<<DEBUG_LOG_LEVEL<<"escape pressed, shutting down;(;\n";
		InputManager::getInstance().setMouseHidden(false);
		URE_INSTANCE->requestMainLoopQuit();
	}

}

void DemoInputInterpreter::interpretInput(Mouse* mouse)
{

	//static bool hideMouse= false;
	if(mouse->isHidden() != mHideMouse)
	{
		mouse->setHidden(mHideMouse);
	}

	if(mouse->getRecentEvent() == Mouse::MOUSE_EVENT_BUTTON_CHANGED)
	{
		if(mouse->getRecentButton() == GLFW_MOUSE_BUTTON_LEFT )
		{
//			if(mouse->getRecentButtonStatus() == GLFW_PRESS)
//			{
//				LOG<<DEBUG_LOG_LEVEL<<"left mouse button pressed;\n";
//			}
//			else
//			{
//				LOG<<DEBUG_LOG_LEVEL<<"left mouse button released;\n";
//			}
		}

//		if(mouse->getRecentButton() == GLFW_MOUSE_BUTTON_RIGHT )
//		{
//			if(mouse->getRecentButtonStatus() == GLFW_PRESS)
//			{
//				//LOG<<DEBUG_LOG_LEVEL<<"right mouse button pressed; toggling mouse hide status;\n";
//				hideMouse = !hideMouse;
//				mouse->setHidden(hideMouse);
//			}
//			else
//			{
//
//			}
//		}

	}
	else
	{
		if( (mouse->getRecentEvent() == Mouse::MOUSE_EVENT_POSITION_CHANGED)
			&& (mouse->isHidden())   )
		{
//			LOG<<DEBUG_LOG_LEVEL<<"mouse moved from ("
//					<<mouse->getLastPosition().x
//					<<","
//					<<mouse->getLastPosition().y
//					<<") to ("
//					<<mouse->getRecentPosition().x
//					<<","
//					<<mouse->getRecentPosition().y
//					<<");\n"
//					;

			Camera* mainCamera = //SimulationResourceManager::getInstance().getMainCamera();
					URE_INSTANCE->getCurrentlyActiveCamera(); // getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

			//Matrix4x4 lookAtMatrix = mainCamera->getViewMatrix();
			float differenceHorizontal =  static_cast<float>(mouse->getRecentPosition().x - mouse->getLastPosition().x);
			float differenceVertical   =  static_cast<float>(mouse->getRecentPosition().y - mouse->getLastPosition().y);

			//self made initial sensivity rule of thumb: moving the mouse about half the screen
			//shall rotate the view about 90 degrees (pi/2); we assume a screen size of 1600*1600
			//pixels for this rule of thumb: a compromize between full hd and notebook displays ("HD ready ;( )
			//4* half thumbrule screen size= 4* 1600 / 2:
			const float pixelsCausingFullRotation = 3200.0f;
			float degreesToYaw = (-360.0f / pixelsCausingFullRotation) * differenceHorizontal * mCameraLookMouseSensivity;
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
