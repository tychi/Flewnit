/*
 * URE.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#include "URE.h"

#include "Util/Log/Log.h"

#include <iostream>

namespace Flewnit {

URE::URE()
: BASIC_OBJECT_CONSTRUCTOR("URE", "URESingletonInstance", "do the unified Rendering")
{
	// TODO Auto-generated constructor stub


}

URE::~URE()
{
	// TODO Auto-generated destructor stub
}



void URE::init( boost::filesystem::path pathToConfigFile)
{
	std::cout<<"here is the URE lib!!1\n";

	std::cout<<"The Config Path is"<< pathToConfigFile.string() <<"\n";

}

}
