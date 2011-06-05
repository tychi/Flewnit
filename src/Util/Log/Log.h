/*
 * Log.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/Singleton.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include "Common/Math.h"


#define LOG Flewnit::Log::getInstance()


//forward to omit fstream includion in often used header:
namespace std
{
	typedef basic_fstream<char> fstream;
}

namespace Flewnit
{

//-----------------------------------------------------------------------
///\brief logging stuff
enum LogLevel
{
	ERROR_LOG_LEVEL 	=0,
	WARNING_LOG_LEVEL	=1,
	INFO_LOG_LEVEL		=2,
	MEMORY_TRACK_LOG_LEVEL=3,
	DEBUG_LOG_LEVEL		=4
};

//-----------------------------------------------------------------------

const String gLogLevelString[]
=
{
		"ERROR       :",
		"WARNING     :",
		"INFO        :",
		"MEMORY_TRACK:",
		"DEBUG       :"
};



class Log: public Singleton<Log>
{
private:
	friend class URE;
	Log();
public:
	virtual ~Log();

	Log& operator<<(LogLevel loglevel);
	Log& operator<<(String logEntry);
	Log& operator<<(const unsigned char* logEntry);

	inline Log& operator<<(int logEntry){return handleGenericValues<int>(logEntry);}
	inline Log& operator<<(long int logEntry){return handleGenericValues<long int>(logEntry);}
	inline Log& operator<<(uint logEntry){return handleGenericValues<uint>(logEntry);}
	inline Log& operator<<(Scalar logEntry){return handleGenericValues<Scalar>(logEntry);}
	inline Log& operator<<(double logEntry){return handleGenericValues<double>(logEntry);}
	inline Log& operator<<(size_t logEntry){return handleGenericValues<size_t>(logEntry);}

	inline Log& operator<<(const Vector3Dui& logEntry){
		return (*this)<<"Vector3Dui("
				<<logEntry.x<<","
				<<logEntry.y<<","
				<<logEntry.z<<")";
	}
	inline Log& operator<<(const Vector4D& logEntry){
		return (*this)<<"Vector4D("
				<<logEntry.x<<","
				<<logEntry.y<<","
				<<logEntry.z<<","
				<<logEntry.w<<")";
	}
	inline Log& operator<<(const Matrix4x4& logEntry){
		return (*this)<<"Matrix4x4("
				<<logEntry[0]<<";"
				<<logEntry[1]<<";"
				<<logEntry[2]<<";"
				<<logEntry[3]<<")";
	}

	//inline Log& operator<<(size_t logEntry){return handleGenericValues<size_t>(logEntry);}

	inline void enableLogLevel(LogLevel which)
	{
		mConsoleOutputLevelFlags |=FLEWNIT_FLAGIFY(which);
	}

	inline void disableLogLevel(LogLevel which)
	{
		mConsoleOutputLevelFlags &= (~FLEWNIT_FLAGIFY(which));
	}

	inline bool logLevelIsEnabled(LogLevel which)
	{
		return ( ( mConsoleOutputLevelFlags & FLEWNIT_FLAGIFY(which) ) != 0 ) ;
	}

private:
#if FLEWNIT_DO_CONSOLE_DEBUG_OUTPUT
	void handleConsoleOutput(String logEntry);
#else
	//nothing;
#	define handleConsoleOutput(stringstuff)
#endif

	template <typename T> Log& handleGenericValues(T logEntry);

	std::fstream* mFileStream;

	LogLevel mCurrentLogLevel;


	//delete or add flags to suppress annoying and slow console output
	uint mConsoleOutputLevelFlags;
};

template <typename T> Log& Log::handleGenericValues(T logEntry)
{
	if(!logLevelIsEnabled(mCurrentLogLevel))
	{
		//do nothing for disabled output levels
		return *this;
	}

	//static std::stringstream pseudoToStringConverter;
	std::stringstream pseudoToStringConverter;
	pseudoToStringConverter<<logEntry;
	(*mFileStream) << pseudoToStringConverter.str();
	handleConsoleOutput(pseudoToStringConverter.str());
	//this SHOULD reset th stringstream,. but it doesn't :(
	//thats why the local variable cannot be static :((
	//pseudoToStringConverter.rdbuf()->str()="";

	if(mCurrentLogLevel== ERROR_LOG_LEVEL)
	{
		//maybe assert(0); lets see if this is necessary;
	}

	return (*this);
}


}

