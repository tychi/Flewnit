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

namespace Flewnit
{

class Log: public Singleton<Log>
{
public:
	Log();
	virtual ~Log();

private:
	std::fstream mFileStream;
};

}

