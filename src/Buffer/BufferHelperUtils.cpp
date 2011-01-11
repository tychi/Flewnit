/*
 * BufferHelperUtils.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#include "BufferHelperUtils.h"

namespace Flewnit
{

bool BufferHelper::isPowerOfTwo(int value)
{
	if(value <0 ) return false;

	int cnt=0;
	for (unsigned int i=0;i< sizeof(int)*8 ; i++)
	{
		if( ( value & (1<<i) ) !=0) cnt++;
	}
	return (cnt <= 1);
}

}
