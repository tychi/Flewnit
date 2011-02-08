/*
 * GenericLightingUberShader.h
 *
 *  Created on: Feb 7, 2011
 *      Author: tychi
 */

#pragma once

#include "Shader.h"



namespace Flewnit {

class GenericLightingUberShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	//public constructor for deferred lighting (which does not involve material classes and hence
	//cannot trigger automatic generation by ShaderManager)
	GenericLightingUberShader(Path codeDirectory, const ShaderFeaturesLocal& localShaderFeatures);

	virtual ~GenericLightingUberShader();

	//virtual bool operator==(const Shader& rhs)const;

	virtual void use(SubObject* so)throw(SimulatorException);

protected:

	//virtual void generateCustomDefines();
	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};

}
