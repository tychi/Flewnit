/**
 *
 *
 */


#include <iostream>

#include "URE.h"

int main(int argc, char *argv[])
{
	std::cout<<"hello liquid world; running now app!!1 \n";


	//run bootstrap before acually instancing the render library; This is because the profiler must be instance at first;
	Flewnit::URE::bootstrap();

	//now instanciate the Renderer singleton
	Flewnit::URE::startUp();

	//initialize it
	URE_INSTANCE->init();

	Flewnit::URE::shutdown();
}
