/*
 * Loader.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Loader.h"

#include "LoaderHelper.h"



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

	TiXmlDocument XMLDoc ("flewnitGlobalConfig");
	TiXmlElement* xmlRootNode = 0;

	try {
		if(! XMLDoc.LoadFile(pathToGlobalConfigFile.string())
			|| XMLDoc.Error())
		{
			throw std::exception();
		}

		xmlRootNode = XMLDoc.RootElement();//->FirstChildElement("flewnitGlobalConfig");

		loadUISettings(xmlRootNode, config);

		//rest to come TODO


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
	const char *tmp =0;

	TiXmlElement* winSettings= xmlRootNode->FirstChildElement("UI_Settings")->FirstChildElement("windowSettings");



	config.root().get("UI_Settings")= new ConfigStructNode();

	config.root().get("UI_Settings")->get("windowSettings")= new ConfigStructNode();

//	config.root().get("UI_Settings")->get("windowSettings")->get("mediaLayer")=
//			new ConfigValueNode<String>(
//					//String(winSettings->FirstChildElement("mediaLayer")->Attribute("value"))
//					LoaderHelper::getAttribute<String>(winSettings,"value")
//					);
//
//
//	config.root().get("UI_Settings")->get("WindowSettings")->get("fullScreen")=
//				new ConfigValueNode<bool>(
//						LoaderHelper::getAttribute<bool>(winSettings->FirstChildElement("fullScreen"),"value")
//						);



}

//LOG<<DEBUG_LOG_LEVEL<<"mediaLayer value: "<<
//		reinterpret_cast<ConfigValueNode<String>* > (
//				config.root().get("UI_Settings")->get("windowSettings")->get("mediaLayer")
//		)->value()
//		   <<":\n";
//
//LOG<<DEBUG_LOG_LEVEL<<"mediaLayer type enum value : "<<
//	config.root().get("UI_Settings")->get("windowSettings")->get("mediaLayer")->getType()
//		   <<":\n";
//





}



