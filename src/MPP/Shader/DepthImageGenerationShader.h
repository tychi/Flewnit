/*
 * DepthImageGenerationShader.h
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#pragma once


#include "Shader.h"

namespace Flewnit {



class DepthImageGenerationShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	DepthImageGenerationShader(Path codeDirectory,
			RenderingTechnique renderingTechnique,
			VisualMaterialType visMatType,
			TextureType renderTargetTextureType,
			bool forTessellation,
			bool instancedRendering);

	virtual ~DepthImageGenerationShader();

	//virtual bool operator==(const Shader& rhs)const;

	virtual void use(SubObject* so)throw(SimulatorException);

protected:

	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};
}

