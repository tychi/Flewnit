/*
 * LiquidShader.h
 *
 *  Created on: Feb 6, 2011
 *      Author: tychi
 */

#pragma once


#include "Shader.h"


namespace Flewnit {

class LiquidShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	virtual ~LiquidShader();

	virtual void use(SubObject* so)throw(SimulatorException);

protected:
	friend class ShaderManager;
	LiquidShader(Path codeDirectory, const ShaderFeaturesLocal& localShaderFeatures);

	//virtual bool operator==(const Shader& rhs)const;

	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};

}

