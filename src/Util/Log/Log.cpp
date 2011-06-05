/*
 * Log.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#include "Log.h"


namespace Flewnit
{

Log::Log()
: mCurrentLogLevel(INFO_LOG_LEVEL),
  mConsoleOutputLevelFlags
  	  	(
  			FLEWNIT_FLAGIFY(ERROR_LOG_LEVEL)	|
  			FLEWNIT_FLAGIFY(WARNING_LOG_LEVEL)	|
  			FLEWNIT_FLAGIFY(INFO_LOG_LEVEL) 	|
  			FLEWNIT_FLAGIFY(MEMORY_TRACK_LOG_LEVEL)  |
  			FLEWNIT_FLAGIFY(DEBUG_LOG_LEVEL)
  		)

{
	mFileStream = new std::fstream();
	mFileStream->open(FLEWNIT_DEFAULT_LOG_FILEPATH, std::ios::out);
	(*mFileStream) << "Log for the Flewnit Engine ,located at "<<FLEWNIT_DEFAULT_LOG_FILEPATH << ";\n";
}

Log::~Log()
{
	mFileStream->close();
	delete mFileStream;
}


Log&  Log::operator<<(LogLevel loglevel)
{
	mCurrentLogLevel = loglevel;

	if(logLevelIsEnabled(mCurrentLogLevel))
	{
		(*mFileStream)<< gLogLevelString[loglevel] <<" :";
		handleConsoleOutput(gLogLevelString[loglevel]);
	}



	return (*this);
}

Log&  Log::operator<<(String logEntry)
{
	if(! logLevelIsEnabled(mCurrentLogLevel))
	{
		return *this;
	}

	(*mFileStream) << logEntry;

	handleConsoleOutput(logEntry);

	return (*this);
}

Log&  Log::operator<<(const unsigned char* logEntry)
{
	String stringEntry( reinterpret_cast<const char*>( logEntry) );
	return *(this)<<stringEntry;
//	if(!charPtr)
//	{
//		assert(0 && "Log: Bad cast from const unsigned char* to const char* ");
//	}
//	(*mFileStream) << String( charPtr );
//
//	handleConsoleOutput( String( charPtr ) );
//
//	return (*this);
}

#if FLEWNIT_DO_CONSOLE_DEBUG_OUTPUT
void Log::handleConsoleOutput(String logEntry)
{
	assert(logLevelIsEnabled(mCurrentLogLevel));

	std::cout<< logEntry;

}
#endif




}
