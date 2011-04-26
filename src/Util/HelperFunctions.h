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

	static String toString(int number);

	static int countBits(int num);

	static unsigned int ceilToNextMultiple(unsigned int valToCeil, unsigned int multipleToBeCeiledTo);
};

}

