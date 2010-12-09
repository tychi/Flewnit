
/*
 * Loader.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once


#include "../../Common/BasicObject.h"



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


};

}

