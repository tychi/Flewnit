/*
 * Log.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"

namespace Flewnit
{

class Log: public Singleton<Log>
{
public:
	Log();
	virtual ~Log();
};

}

