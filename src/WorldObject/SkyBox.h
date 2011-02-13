/*
 * SkyBox.h
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#pragma once


#include "PureVisualObject.h"


namespace Flewnit
{

class SkyBox
: public PureVisualObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SkyBox(Path cubeMapDirectory, String cubeMapFilename, String fileEnding,const AmendedTransform& localtransform = AmendedTransform());
	virtual ~SkyBox();
};

}
