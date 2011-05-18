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
#include "Simulator/ParallelComputeManager.h"
#include "Util/Time/FPSCounter.h"
#include "Simulator/SimulationResourceManager.h"
#include "MPP/Shader/ShaderManager.h"
#include "../include/GL3/gl3.h"
#else

#endif



#include "../src/Simulator/MechanicsSimulator/ParticleMechanicsStages/ParticleMechanicsStage.h"




namespace Flewnit
{

DemoInputInterpreter::DemoInputInterpreter(float cameraLookMouseSensivity, float movementSpeed)
: mCameraLookMouseSensivity(cameraLookMouseSensivity),mMovementSpeed(movementSpeed),
  mHideMouse(false),mLeftMousePressed(false)
{
	// TODO make cameraLookMouseSensivity  config file- configurable;

	mMovementState[MOVEMENT_DIRECTION_FRONT_BACK] = MOVEMENT_STATE_STILL;
	mMovementState[MOVEMENT_DIRECTION_RIGHT_LEFT] = MOVEMENT_STATE_STILL;
	mMovementState[MOVEMENT_DIRECTION_UP_DOWN] = MOVEMENT_STATE_STILL;

	mUserForceControlPoint =
			dynamic_cast<ParticleMechanicsStage*>( URE_INSTANCE->getSimulator(MECHANICAL_SIM_DOMAIN)->getStage("ParticleMechanicsStage") )
				->addUserForceControlPoint( Vector4D(1,1,1,1),9,0.3f);
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

	mUserForceControlPoint->setForceOriginWorldPos(
			Vector4D( URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getPosition(), 1.0f )
			+ 10.0f * Vector4D( URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getDirection(), 0.0f)
	);

	mUserForceControlPoint->setIntensity(mLeftMousePressed ? 7.5f: 0.0f);



}

void DemoInputInterpreter::interpretInput(Keyboard* keyboard)
{
//	Camera* mainCamera =
//		URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

	static float polygonOffSetFactor =2.5f;
	static float polyGonOffsetUnit = 5.8f;
	const float increment = 0.1f;
	if(keyboard->getRecentStatus() == GLFW_PRESS)
	{
		if( (keyboard->getRecentKey()) == GLFW_KEY_UP)
		{
			polygonOffSetFactor += increment;
		}
		if( (keyboard->getRecentKey()) == GLFW_KEY_DOWN)
		{
			polygonOffSetFactor -= increment;
		}
		if( (keyboard->getRecentKey()) == GLFW_KEY_LEFT)
		{
			polyGonOffsetUnit += increment;
		}
		if( (keyboard->getRecentKey()) == GLFW_KEY_RIGHT)
		{
			polyGonOffsetUnit -= increment;
		}

//		LOG<<DEBUG_LOG_LEVEL<<"polygonOffSetFactor: "<<polygonOffSetFactor
//				<<"; polyGonOffsetUnit: "<<polyGonOffsetUnit<<";\n";
		GUARD(glPolygonOffset(polygonOffSetFactor,polyGonOffsetUnit));
	}




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

		GUARD(glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT));
		GUARD(glEnable(GL_PROGRAM_POINT_SIZE));

	}
	if(keyboard->getRecentKey() == GLFW_KEY_F3)
	{
		GUARD(glDisable(GL_PROGRAM_POINT_SIZE));

		GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	if(keyboard->getRecentKey() == GLFW_KEY_F4)
	{
		GUARD(glDisable(GL_PROGRAM_POINT_SIZE));

		GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}


	if( (keyboard->getRecentKey() == GLFW_KEY_TAB) && (keyboard->getRecentStatus() == GLFW_PRESS) )
	{
		URE_INSTANCE->toggleDoDebugDraw();
	}


	if((keyboard->getRecentKey() == GLFW_KEY_SPACE) && (keyboard->getRecentStatus() == GLFW_PRESS))
	{
		mHideMouse = !mHideMouse;
	}


	//---------------------------------------------------------------------------
	//static ShadingFeatures currentlyEnabledFeatures = ShaderManager::getInstance().getEnabledShadingFeatures();

	if(keyboard->getRecentStatus() == GLFW_PRESS)
	{
		ShadingFeatures sfToChange = SHADING_FEATURE_NONE;
		switch (keyboard->getRecentKey())
		{
			case 'N':
				sfToChange = SHADING_FEATURE_DIFFUSE_TEXTURING;
				break;
			case 'M':
				sfToChange = SHADING_FEATURE_DIRECT_LIGHTING;
				break;
			case ',':
				sfToChange = SHADING_FEATURE_NORMAL_MAPPING;
				break;
			case '.':
				sfToChange = SHADING_FEATURE_CUBE_MAPPING;
				break;
			case '-':
				sfToChange = SHADING_FEATURE_TESSELATION;
				break;
			default:
				break;
		}

		if( sfToChange != SHADING_FEATURE_NONE)
		{
			ShaderManager::getInstance().setEnableShadingFeatures(
				sfToChange,
				//toggle current activations state by querying, negating and setting it
				! ShaderManager::getInstance().shadingFeaturesAreEnabled(sfToChange)
			);

		}
	}

//	static bool enableTess =ShaderManager::getInstance().tesselationIsEnabled();
//	if((keyboard->getRecentKey() == GLFW_KEY_TAB) && (keyboard->getRecentStatus() == GLFW_PRESS))
//	{
//		enableTess = !enableTess;
//		ShaderManager::getInstance().setEnableTesselation(enableTess);
//	}




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
		if( mouse->getRecentButton() == GLFW_MOUSE_BUTTON_LEFT )
		{

			mLeftMousePressed = mouse->getRecentButtonStatus() == GLFW_PRESS;
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
