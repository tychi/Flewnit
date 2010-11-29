/**
 *
 *
 */


#include <iostream>

#include "URE.h"

#include "Util/Log/Log.h"


class Foo: public Flewnit::BasicObject
{
public:
	Foo(int i, int j) {};
};

int main(int argc, char *argv[])
{
	//run bootstrap before acually instancing the render library; This is because the profiler must be instance at first;
	Flewnit::URE::bootstrap();
	//now instanciate the Renderer singleton
	//INSTANCIATE_SINGLETON(Flewnit::URE)

	FLEWNIT_INSTANTIATE(
			new Flewnit::URE(),
			Flewnit::URE,
			"do the unified Rendering");

	Flewnit::Log::getInstance()<<Flewnit::INFO_LOG_LEVEL<<"hello liquid world; running now app!!1 \n";

	//initialize it
	URE_INSTANCE->init();




	DESTROY_SINGLETON(Flewnit::URE);
	//clean stuff up, like the profiler etc.
	Flewnit::URE::cleanup();
}
