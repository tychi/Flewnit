/*
 * Log.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/Singleton.h"


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
public:
	Log();
	virtual ~Log();

	Log& operator<<(LogLevel loglevel);
	Log& operator<<(String logEntry);
	//Log& operator<<( char* logEntry);
	Log& operator<<( int logEntry);
	Log& operator<<( uint logEntry);
	Log& operator<<( Scalar logEntry);


	inline void enableLogLevel(LogLevel which)
	{
		mConsoleOutputLevelFlags |=FLEWNIT_FLAGIFY(which);
	}

	inline void disableLogLevel(LogLevel which)
	{
		mConsoleOutputLevelFlags &=!FLEWNIT_FLAGIFY(which);
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



}

