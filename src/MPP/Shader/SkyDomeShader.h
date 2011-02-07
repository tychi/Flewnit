/*
 * SkyDomeShader.h
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#pragma once


#include "Shader.h"

namespace Flewnit {

class SkyDomeShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	virtual ~SkyDomeShader();

	virtual void use(SubObject* so)throw(SimulatorException);

protected:
	friend class ShaderManager;
	SkyDomeShader(Path codeDirectory,
			TextureType renderTargetTextureType);

	virtual void generateCustomDefines();
	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};


}

