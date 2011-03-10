/*
 * SkyDome.h
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#pragma once


#include "PureVisualObject.h"


namespace Flewnit
{

class SkyDome
: public PureVisualObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SkyDome( Path cubeMapFilePath );//Path cubeMapDirectory, String cubeMapFilename, String fileEnding);
	virtual ~SkyDome();

	Texture* getCubeMap();
};

}
