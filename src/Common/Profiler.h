/*
 * Profiler.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/Singleton.h"

namespace Flewnit
{

class Profiler : public Singleton<Profiler>
{
public:
	Profiler();
	virtual ~Profiler();


};

}

