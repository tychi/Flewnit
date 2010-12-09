
/*
 * Loader.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once


#include "../../Common/BasicObject.h"

class TiXmlElement;



namespace Flewnit
{

class Config;
class Scene;

class Loader : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:
	Loader();
	virtual ~Loader();

	void loadGlobalConfig( Config& config, const Path& pathToGlobalConfigFile	);
	void loadGlobalConfig( Config& config);

	Scene* loadScene();

private:
	void loadUISettings(TiXmlElement* xmlRootNode, Config& config);


};

}

