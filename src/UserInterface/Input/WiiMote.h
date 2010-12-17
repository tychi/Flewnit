/*
 * WiiMote.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "InputDevice.h"

#include "Common/Math.h"

namespace Flewnit
{

class WiiMote
:public InputDevice
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	WiiMote();
	virtual ~WiiMote();

	//to be overridden by devices which don't have a callback functionality to get informed about their status changes, e.g. a Wiimote
	void pullStatus(); //TODO use WiiYouself or whatever to realize stuff; this is only a stub, Wiimote isn't supported yet;

	//partial visitor pattern : call "visit" part actively, i.e. without explicit accept-part; Visitor is InputInterpreter, and it is acquired from the InputManager
	virtual void notifyInterpreter();

};

}

