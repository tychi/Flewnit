/*
 * HelperFunctions.h
 *
 *  Created on: Dec 15, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

namespace Flewnit
{

class HelperFunctions
{
public:
	HelperFunctions();
	virtual ~HelperFunctions();

	static bool isPowerOfTwo(int num)
	{
		return (countBits(num) <= 1);
	}

	static unsigned int log2ui(unsigned int val);

	static String toString(int number);

	static int countBits(int num);

	static String getBitString(unsigned int value, unsigned int numStringifiedBits = 32 );



	static unsigned int ceilToNextMultiple(unsigned int valToCeil, unsigned int multipleToBeCeiledTo);

	//if valToCeil is power of two, return valToCeil, else return the next higher value being a power of two
	static unsigned int ceilToNextPowerOfTwo(unsigned int valToCeil);
	//if valToCeil is power of two, return valToCeil, else return the next lower value being a power of two
	static unsigned int floorToNextPowerOfTwo(unsigned int valToFloor);

};

}

