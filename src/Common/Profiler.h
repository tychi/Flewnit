/*
 * Profiler.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 *	\brief The profiler for memory Tracking
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"
#include "Buffer/BufferSharedDefinitions.h"

//---------------------------------------------------------------------------------------------------------
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)

#include "Common/Singleton.h"


namespace Flewnit
{

//forwards to omit cyclic includes;
class BasicObject;

class Profiler : public Singleton<Profiler>
{
public:
	Profiler();
	virtual ~Profiler();

	void performBasicChecks();

	ID registerBasicObject(BasicObject*);

	void registerBufferAllocation(ContextType contextType, size_t sizeInByte);

private:
	Map<ID,BasicObject*> mRegisteredBasicObjects;

};

}


#endif // (FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING)
//---------------------------------------------------------------------------------------------------------
