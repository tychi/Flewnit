/*
 * Profiler.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

//include the shared definitions in order to be able to determine if anything her should be compiled^^
#include "Common/FlewnitSharedDefinitions.h"


//---------------------------------------------------------------------------------------------------------
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)


#include "Profiler.h"
#include "Common/BasicObject.h"
#include "Util/Log/Log.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

Profiler::Profiler()
	: mMaxAssignedID(0),
	  mTotalRegisteredObjects(0), mTotalObjectMemoryFootprint(0),
	  mCLGLSharedAllocatedBufferMemory(0),mNumCLGLSharedAllocatedBuffers(0)
{
	mPrivateAllocatedBufferMemories[HOST_CONTEXT_TYPE]=0;
	mPrivateAllocatedBufferMemories[OPEN_CL_CONTEXT_TYPE]=0;
	mPrivateAllocatedBufferMemories[OPEN_GL_CONTEXT_TYPE]=0;

	mNumPrivateAllocatedBuffers[HOST_CONTEXT_TYPE]=0;
	mNumPrivateAllocatedBuffers[OPEN_CL_CONTEXT_TYPE]=0;
	mNumPrivateAllocatedBuffers[OPEN_GL_CONTEXT_TYPE]=0;

	Log::getInstance()<< DEBUG_LOG_LEVEL<<"Profiler instanced\n";

}

Profiler::~Profiler()
{
	Log::getInstance()<< MEMORY_TRACK_LOG_LEVEL<<"Profiler destructor called;\n";
	Log::getInstance()<< MEMORY_TRACK_LOG_LEVEL<<"Showing memory Status; If you've done a good house keeping, all values should be zero:\n";
	printMemoryStatus();

	printRegisteredObjects();

	checkError();
}


void Profiler::printObjectStatus(BasicObject* bo)
{
	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL
			<<"Object class name: \""<< bo->getClassName() <<"\";\n"
			<<"\t\tMemory footprint:"<< bo->getMemoryFootprint() <<" Byte;\n"
			;//<<"\t\tObject purpose: :\""<< bo->getPurposeDescription() <<"\";\n" ;
}

void Profiler::printMemoryStatus()
{
	assert("Not all registered objects have their memory footprint initialized and tracked; "
			&& "Call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n"
			&& mRegisteredButUntrackedObjects.size()==0);

	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL
			<<  mTotalRegisteredObjects << " objects registered;\n"
			<<  mTotalObjectMemoryFootprint << " bytes are consumed by those objects;\n"
			;
	printBufferOnlyMemoryStatus();
}

void Profiler::printRegisteredObjects()
{
	Pair<ID,BasicObject*> boPair;
	BOOST_FOREACH(boPair, mRegisteredBasicObjects)
		{
			printObjectStatus(boPair.second);
		}
}

void Profiler::printBufferOnlyMemoryStatus()
{
	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL
				<< mPrivateAllocatedBufferMemories[HOST_CONTEXT_TYPE]  << " bytes are consumed by "
						<<mNumPrivateAllocatedBuffers[HOST_CONTEXT_TYPE]<<" CPU buffers;\n"
				<< mPrivateAllocatedBufferMemories[OPEN_CL_CONTEXT_TYPE] << " bytes are consumed by "
				<<mNumPrivateAllocatedBuffers[OPEN_CL_CONTEXT_TYPE] <<" privately used OpenCL buffers;\n"
				<< mPrivateAllocatedBufferMemories[OPEN_GL_CONTEXT_TYPE]  << " bytes are consumed by "
				<<mNumPrivateAllocatedBuffers[OPEN_GL_CONTEXT_TYPE] <<" privately used OpenGL buffers;\n"
				<< mCLGLSharedAllocatedBufferMemory << " bytes are consumed by "
						<<mNumCLGLSharedAllocatedBuffers<<" shared OpenCL/OpenGL buffers;\n"
				;
}



void Profiler::performBasicChecks()
{
	Log::getInstance()<< INFO_LOG_LEVEL
			<< (int) sizeof(int) << " bytes consumed by int;\n"
			<< (int) sizeof(uint) << " bytes consumed by uint;\n"
			<< (int) sizeof(Scalar) << " bytes consumed by Scalar (typedef for float or double);\n"
			<< (int) sizeof(float) << " bytes consumed by float;\n"
			<< (int) sizeof(double) << " bytes consumed by double;\n"
			<< (int) sizeof(bool) << " bytes consumed by bool;\n"
			<< (int) sizeof(int*) << " bytes consumed by pointer types;\n"
			//<< (int) sizeof(int&) << " bytes consumed by reference types;\n"
			//<< (int) sizeof(int64) << " bytes consumed by explicit 64 bit integer type __int64 ;\n"
			<< (int) sizeof(long) << " bytes consumed by long;\n"
			<< (int) sizeof(long int) << " bytes consumed by long int;\n"
			<< (int) sizeof(long unsigned int) << " bytes consumed by long unsigned int;\n"
			<< (int) sizeof(long long) << " bytes consumed by long long;\n"
			<< (int) sizeof(String) << " bytes consumed by String;\n"
			;

	//TODO maybe some cl/gl related tests..
}



//--------------------------------------------------------------------------------------------
///\brief registering stuff
///\{
ID Profiler::registerBasicObject(BasicObject* bo)
{
	checkError();

	//assure that the object was NOT yet registered or has a valid ID; otherwise, the application logic would have failed;
	assert( "BasicObject to be registered neither has a valid ID nor its ID is already registered;"
			&& ( bo->getUniqueID() == FLEWNIT_INVALID_ID )
			&& (mRegisteredBasicObjects.count(bo->getUniqueID()) == 0)
	);



	if(mIDsFromFreedObjects.size() == 0)
	{
		mRegisteredBasicObjects[mMaxAssignedID]=bo;
		bo->mUniqueID = mMaxAssignedID;
		mMaxAssignedID++;
	}
	else
	{
		mRegisteredBasicObjects[mIDsFromFreedObjects.top()]=bo;
		bo->mUniqueID= mIDsFromFreedObjects.top();
		mIDsFromFreedObjects.pop();
	}

	mTotalRegisteredObjects++;

	mRegisteredButUntrackedObjects.push_back(bo);

	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL<<"BasicObject with ID \""<< bo->getUniqueID()  <<"\" registered at Profiler; Now, "<<mTotalRegisteredObjects<<"  are registered in total;\n";

	//set the guard, so that an error is thrown if the memory footprint isn't delivered right after initialisation:
	return bo->getUniqueID();

}

void Profiler::unregisterBasicObject(BasicObject* bo)
{
	checkError();

	//assure that the object was registered; otherwise, the application logic would have failed;
	assert( mRegisteredBasicObjects.erase(bo->getUniqueID()) > 0);

	mIDsFromFreedObjects.push(bo->getUniqueID());

	mTotalRegisteredObjects--;
	mTotalObjectMemoryFootprint -= bo->getMemoryFootprint();

	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL<<"BasicObject UNregistered from Profiler; Now, "<<mTotalRegisteredObjects<<"  are registered in total;\n";
	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL<<"printing now Object Information:\n";
	printObjectStatus(bo);

	printMemoryStatus();
}

void Profiler::updateMemoryTrackingInfo()
{
	LOG<<MEMORY_TRACK_LOG_LEVEL<<"now updating memory Tracking Info:\n";
	BOOST_FOREACH(BasicObject* bo, mRegisteredButUntrackedObjects)
		{
			registerObjectMemoryFootPrint(bo);
		}

	mRegisteredButUntrackedObjects.clear();

	printMemoryStatus();
}



void Profiler::registerObjectMemoryFootPrint(BasicObject* bo)
{
	bo->initBasicObject();
	mTotalObjectMemoryFootprint += bo->getMemoryFootprint();

	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL<<"BasicObject memory footprint ("<< bo->getMemoryFootprint() <<" Bytes) of \" "<< bo->getClassName() <<" \" is now tracked by Profiler;\n";
	Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL<<"printing now Object Information:\n";
	printObjectStatus(bo);
}

void Profiler::registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte)
{
	modBufferAllocation_internal(contextTypeFlags,static_cast<int>(sizeInByte),1);
}

void Profiler::unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte)
{
	modBufferAllocation_internal(contextTypeFlags, (-1) * static_cast<int>(sizeInByte),-1);
}

void Profiler::modBufferAllocation_internal(ContextTypeFlags contextTypeFlags, int sizeIncrement, int buffIncrement)
{
	bool validFlagsGuard =false;
	assert(	(buffIncrement == 1 && sizeIncrement > 0) ||
			(buffIncrement ==-1 && sizeIncrement < 0)   );

	//host stuff
	if( (contextTypeFlags & HOST_CONTEXT_TYPE_FLAG ) )
	{
		//bullshit assertion: thought of it because of "host only" mem; kind of "mirrored/mapped"
		//GPU buffer to host space is not tracked by this profiler; it's not THAT interesting, though,
		//as the memory is not shared, i.e resides tweice in even different memories
		//(main RAM and GPU RAM)of the whole computing system;
		//assert("no other context flags allowed" && contextTypeFlags == HOST_CONTEXT_TYPE_FLAG);

		mNumPrivateAllocatedBuffers[HOST_CONTEXT_TYPE] += buffIncrement ;
		mPrivateAllocatedBufferMemories[HOST_CONTEXT_TYPE] += sizeIncrement;

		validFlagsGuard=true;
	}

	//graphics stuff
	if(		 (contextTypeFlags & OPEN_CL_CONTEXT_TYPE_FLAG )
		&& ! (contextTypeFlags & OPEN_GL_CONTEXT_TYPE_FLAG ) )
	{
		mNumPrivateAllocatedBuffers[OPEN_CL_CONTEXT_TYPE] += buffIncrement ;
		mPrivateAllocatedBufferMemories[OPEN_CL_CONTEXT_TYPE] += sizeIncrement;

		validFlagsGuard=true;
	}

	//compute stuff
	if(		 (contextTypeFlags & OPEN_GL_CONTEXT_TYPE_FLAG )
		&& ! (contextTypeFlags & OPEN_CL_CONTEXT_TYPE_FLAG ) )
	{
		mNumPrivateAllocatedBuffers[OPEN_GL_CONTEXT_TYPE] += buffIncrement ;
		mPrivateAllocatedBufferMemories[OPEN_GL_CONTEXT_TYPE] += sizeIncrement;

		validFlagsGuard=true;
	}

	//interop stuff
	if(		 (contextTypeFlags & OPEN_GL_CONTEXT_TYPE_FLAG )
		&& 	 (contextTypeFlags & OPEN_CL_CONTEXT_TYPE_FLAG ) )
	{
		mNumCLGLSharedAllocatedBuffers += buffIncrement ;
		mCLGLSharedAllocatedBufferMemory += sizeIncrement;

		validFlagsGuard=true;
	}

	assert("flags valid" && validFlagsGuard);

	printBufferOnlyMemoryStatus();
	checkError();
}

void Profiler::checkError()
{
//following assertion is bad du to nested instanciations; redesigning again :(
//	assert("at least one BasicObject isn't tracked correctly; Use the FLEWNIT_INSTANCIATE macro for every class derived from BasicObject!"
//			&&  mIDOfLastRegisteredButNotMemoryTrackedObject == FLEWNIT_INVALID_ID);
}



}
///\}

#endif
//---------------------------------------------------------------------------------------------------------

