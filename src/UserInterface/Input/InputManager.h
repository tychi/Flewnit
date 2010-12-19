/*
 * InputManager.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"

namespace Flewnit
{

class InputDevice;
class Keyboard;
class Mouse;
class WiiMote;

class InputInterpreter;

class InputManager
: public Singleton<InputManager>, public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	//create input devices depending on window manager and config;
	void init();

public:
	InputManager();
	virtual ~InputManager();

	static void keyPressedCallback(int key, int status);
	static void mouseMovedCallback(int newXpos, int newYpos);
	static void mouseButtonChangedCallback(int button, int status);
	static void mouseWheelCallback(int wheelValue);
	//Wiimote will have to pull its own status actively, i.e. no callback function needed;


	//let die InputInterpreter do his work: set camera, delegate GUI
	bool processInput();

	void setInputInterpreter(InputInterpreter* inputInterpreter);
	InputInterpreter* getInputInterpreter()const {return mInputInterpreter;}


protected:
	//List <InputDevice*> mInputDevices;

	Keyboard* mKeyboard;
	Mouse* mMouse;
	WiiMote* mWiiMote;

	InputInterpreter* mInputInterpreter;
};

}

