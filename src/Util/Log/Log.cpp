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

	(*mFileStream)<< gLogLevelString[loglevel] <<" :";

	handleConsoleOutput(gLogLevelString[loglevel]);

	return (*this);
}

Log&  Log::operator<<(String logEntry)
{
	(*mFileStream) << logEntry;

	handleConsoleOutput(logEntry);

	return (*this);
}




#if FLEWNIT_DO_CONSOLE_DEBUG_OUTPUT
void Log::handleConsoleOutput(String logEntry)
{
	 if( mConsoleOutputLevelFlags & FLEWNIT_FLAGIFY(mCurrentLogLevel))
	 {
		 std::cout<< logEntry;
	 }
}
#endif




}
