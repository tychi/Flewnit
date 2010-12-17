/*
 * WiiMote.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "WiiMote.h"
#include "Util/Log/Log.h"

#include "URE.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "UserInterface/Input/InputInterpreter.h"

namespace Flewnit
{

WiiMote::WiiMote()
{
	// TODO Auto-generated constructor stub

}

WiiMote::~WiiMote()
{
	// TODO Auto-generated destructor stub
}


//to be overridden by devices which don't have a callback functionality to get informed about their status changes, e.g. a Wiimote
void WiiMote::pullStatus() //TODO use WiiYouself or whatever to realize stuff; this is only a stub, Wiimote isn't supported yet;
{
	LOG<<WARNING_LOG_LEVEL<< "  WiiMote::pullStatus(): no functionality implemented yet ;( ";
	notifyInterpreter();
}


void WiiMote::notifyInterpreter()
{
	URE_INSTANCE->getInputInterpreter()->interpretInput(this);
}


}
