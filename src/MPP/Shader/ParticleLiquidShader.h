/*
 * ParticleLiquidShader.h
 *
 * Simple Shader to show a texture on the screen via a full screen quad.
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */


#pragma once


#include "Shader.h"

namespace Flewnit
{

enum ParticleLiquidShaderType
{
	PARTLIQU_DEPTH_ACCEL_GENERATION_TYPE,
	PARTLIQU_THICKNESS_NOISE_GENERATION_TYPE,
	PARTLIQU_CURVATURE_FLOW_TYPE,
	PARTLIQU_DIRECT_RENDERING_TYPE,
	PARTLIQU_SOPHISTICATED_RENDERING_TYPE,

	__NUM_PARTICLE_LIQU_SHADER_TYPES__
};

const String particleLiquidShaderTypeStrings[] =
{
	"depthAndAccelGeneration",
	"thicknessAndNoiseGeneration",
	"curvatureFlowComputation",
	"directRendering",
	"sophisticatedRendering"
};


class ParticleLiquidShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	virtual ~ParticleLiquidShader();


	virtual void use(SubObject* so)throw(SimulatorException);


	//friend class ShaderManager;
	ParticleLiquidShader(ParticleLiquidShaderType type);

	virtual void build();

protected:

	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);

	ParticleLiquidShaderType mParticleLiquidShaderType;
};


}
