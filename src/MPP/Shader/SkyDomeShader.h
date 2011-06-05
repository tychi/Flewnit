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
			const ShaderFeaturesLocal& sfl
			//, TextureType renderTargetTextureType
			);

	//virtual bool operator==(const Shader& rhs)const;

	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};


}

