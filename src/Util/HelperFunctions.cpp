/*
 * HelperFunctions.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: tychi
 */

#include "HelperFunctions.h"

#include <sstream>

namespace Flewnit
{

HelperFunctions::HelperFunctions()
{}

HelperFunctions::~HelperFunctions()
{}

String HelperFunctions::toString(int number)
{
	std::stringstream s;
	s<<number;
	return s.str();
}

}
