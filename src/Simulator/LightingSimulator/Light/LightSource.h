/*
 * LightSource.h
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#pragma once

#include "WorldObject/WorldObject.h"

namespace Flewnit
{

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/Shader/ShaderSources/GenericUberShader/04_Fragment_dataTypes.glsl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

class LightSource
:public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	LightSource();
	virtual ~LightSource();

private:
	LightSourceShaderStruct mLightSourceShaderStruct;
};

}
