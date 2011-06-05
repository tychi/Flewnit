/**
 *
 *
 */


#include <iostream>

#include "URE.h"

#include "Util/Log/Log.h"

#include "DemoInputInterpreter.h"



int main(int argc, char *argv[])
{

	new Flewnit::URE();

	LOG.disableLogLevel(Flewnit::DEBUG_LOG_LEVEL);
	LOG.disableLogLevel(Flewnit::MEMORY_TRACK_LOG_LEVEL);

	//initialize it
	URE_INSTANCE->init();

	URE_INSTANCE->setInputInterpreter( new Flewnit::DemoInputInterpreter(0.5,50));


	URE_INSTANCE->enterMainLoop();


	DESTROY_SINGLETON(Flewnit::URE);


}
