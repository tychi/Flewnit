/**
 *
 *
 */


#include <iostream>

#include "URE.h"

#include "Util/Log/Log.h"


class Foo : public Flewnit::BasicObject
{
public:
	Foo(int i, int j) {};
};

int main(int argc, char *argv[])
{
	//run bootstrap before acually instancing the render library; This is because the profiler must be instance at first;
	Flewnit::URE::bootstrap();
	//now instantiate the Engine singleton
	FLEWNIT_INSTANTIATE(Flewnit::URE,(),"do the unified Rendering");


	//initialize it
	URE_INSTANCE->init();


	DESTROY_SINGLETON(Flewnit::URE);
	//clean stuff up, like the profiler etc.
	Flewnit::URE::cleanup();
}
