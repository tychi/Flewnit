/**
 *
 *
 */


#include <iostream>

#include "URE.h"

int main(int argc, char *argv[])
{
	std::cout<<"hello liquid world; running now app!!1 \n";

	Flewnit::URE::startUp();

	Flewnit::URE::getInstancePtr()->init();


	Flewnit::URE::shutdown();
}
