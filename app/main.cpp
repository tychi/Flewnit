/**
 *
 *
 */


#include <iostream>

#include "URE.h"

#include "Util/Log/Log.h"

#include "DemoInputInterpreter.h"

//class Foo : public Flewnit::BasicObject
//{
//public:
//	Foo(int i, int j) {};
//};

int main(int argc, char *argv[])
{
	//run bootstrap before acually instancing the render library; This is because the profiler must be instance at first;
	bool disableMemoryTrackLogging = false;
	Flewnit::URE::bootstrap(disableMemoryTrackLogging);
	//now instantiate the Engine singleton
	FLEWNIT_INSTANTIATE(new Flewnit::URE());



	//initialize it
	URE_INSTANCE->init();

	URE_INSTANCE->setInputInterpreter( new Flewnit::DemoInputInterpreter());


	URE_INSTANCE->enterMainLoop();


	DESTROY_SINGLETON(Flewnit::URE);
	//clean stuff up, like the profiler etc.
	Flewnit::URE::cleanup();
}
