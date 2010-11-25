/*
 * URE.h
 *
 *  Created on: Nov 24, 2010
 *      Author: tychi
 *
 *
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"
#include "Common/Singleton.h"
//#include <boost/filesystem/path.hpp>

namespace Flewnit {

class URE : public Singleton<URE>
{
public:
	URE();
	virtual ~URE();

	void init();//boost::filesystem::path directoryOfConfigFile = FLEWNIT_DEFAULT_CONFIG_DIR);
};

}

