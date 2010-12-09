/*
 * Loader.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Loader.h"

#include "Config.h"
#include "Util/Log/Log.h"

#include <boost/filesystem/path.hpp>

#include <tinyxml.h>


namespace Flewnit
{

Loader::Loader()
{
	// TODO Auto-generated constructor stub

}

Loader::~Loader()
{
	// TODO Auto-generated destructor stub
}

void Loader::loadGlobalConfig(Config& config)
{
	Path dummy(FLEWNIT_DEFAULT_CONFIG_PATH);
	loadGlobalConfig(config, dummy);
}

void Loader::loadGlobalConfig(Config& config, const Path & pathToGlobalConfigFile)
{
	LOG<<INFO_LOG_LEVEL<< "Loading global Config;\n";

	TiXmlDocument XMLDoc ("FlewnitGlobalConfig");
	TiXmlElement* rootXMLNode = 0;

	try {
		if(! XMLDoc.LoadFile(pathToGlobalConfigFile.string())
			|| XMLDoc.Error())
		{
			throw std::exception();
		}

		const char *tmp =0;
		rootXMLNode = XMLDoc.RootElement();

		loadUISettings(rootXMLNode, config);

		//rest to come


	} catch (...) {
		// We'll just log, and continue on gracefully
		LOG<<ERROR_LOG_LEVEL<< "[Loader::loadGlobalConfig] Error loading file \""<< pathToGlobalConfigFile.string() <<"\";"
				<< XMLDoc.ErrorDesc()
				<<"; No default config initialization yet exists;\n";
		assert(0);
	}
}

void Loader::loadUISettings(TiXmlElement* xmlRootNode, Config& config)
{

}



}



