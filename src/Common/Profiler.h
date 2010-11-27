/*
 * Profiler.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 *
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

//---------------------------------------------------------------------------------------------------------
#if ! (FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING)
//there is no profiling opted, so we do some empty defines here

//---------------------------------------------------------------------------------------------------------
#else

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


#endif // FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING
//---------------------------------------------------------------------------------------------------------
