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
class Wiimote;

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
	static void mouseMovedCallback(int newXposm, int newYpos);
	static void mouseButtonPressedCallback(int button, in status);
	//Wiimote will have to pull its own status actively, i.e. no callback function needed;


	//let die InputInterpreter do his work: set camera, delegate GUI
	void processInput();

	void setInputInterpreter(InputInterpreter* inputInterpreter);



protected:
	//List <InputDevice*> mInputDevices;

	Keyboard* mKeyboard;
	Mouse* mMouse;
	Wiimote* mWiiMote;

	InputInterpreter* mInputInterpreter;
};

}

