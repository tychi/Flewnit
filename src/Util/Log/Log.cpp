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
{
	mFileStream.open(FLEWNIT_DEFAULT_LOG_FILEPATH, std::ios::out);
	mFileStream << "Log for the Flewnit Engine ,located at "<<FLEWNIT_DEFAULT_LOG_FILEPATH << ";\n";
}

Log::~Log()
{

	mFileStream.close();
}

}
