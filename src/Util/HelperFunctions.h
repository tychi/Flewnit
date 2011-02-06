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

	static int countBits(int num)
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
};

}

