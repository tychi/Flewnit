/*
 * MPP.h
 *
 *  Created on: Jan 27, 2011
 *      Author: tychi
 *
 *  The "Massively Parallel Program" class, an abstract base class for Shaders and OpenCL kernels
 */

#pragma once

#include "Common/BasicObject.h"


namespace Flewnit
{



class MPP
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	MPP();
	virtual ~MPP();
};

}
