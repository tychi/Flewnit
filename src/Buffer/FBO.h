/*
 * FBO.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief The base class of all buffers;
 */

#pragma once


#include "Common/BasicObject.h"

#include "Buffer/BufferSharedDefinitions.h"

#include "Buffer/BufferHelperUtils.h"

#include "Common/Math.h"


namespace Flewnit
{

class FBO
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	FBO(String name);

};

}

