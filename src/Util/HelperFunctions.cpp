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

int HelperFunctions::countBits(int num)
{
	int bitCounter=0;

	for(int runner=0 ; runner < 32; runner++ )
	{
		//bit at position i set*?
		//if( (num >> runner) & 1 )
		//int myAss = runner;
		//if( (num & ( 1 << myAss)) != 0 )
		if( (num & ( 1 << runner)) != 0 )
		//if(false)
		{
			bitCounter++;
		}
	}

	return bitCounter;

}

unsigned int HelperFunctions::ceilToNextMultiple(unsigned int valToCeil, unsigned int multipleToBeCeiledTo)
{
	unsigned int rest = valToCeil % multipleToBeCeiledTo;
	if(rest > 0)
	{
		return  valToCeil - rest + multipleToBeCeiledTo;
	}else{
		return valToCeil;
	}
}

unsigned int HelperFunctions::ceilToNextPowerOfTwo(unsigned int valToCeil)
{
	assert( valToCeil <= (unsigned int)(1<<31));

	if(valToCeil == 0)
	{return valToCeil;}

	unsigned int result = 1;

	while(result < valToCeil)
	{
		result <<= 1;
	}

	return result;
}

unsigned int HelperFunctions::floorToNextPowerOfTwo(unsigned int valToFloor)
{

	unsigned int result = (1<<31) ;

	while(result > valToFloor)
	{
		result >>= 1;
	}

	return result;
}


}
