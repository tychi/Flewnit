/*
 * GenericLightingUberShader.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: tychi
 */

#include "GenericLightingUberShader.h"

namespace Flewnit {


//public constructor for deferred lighting (which does not involve material classes and hence
//cannot trigger automatic generation by ShaderManager)
GenericLightingUberShader::GenericLightingUberShader(Path codeDirectory, const ShaderFeaturesLocal& localShaderFeatures)
:
		Shader(codeDirectory, Path("GenericLightingUberShader"), localShaderFeatures)
{
	assert( "only lighting stuff allowed in this shader type" &&
		(
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFAULT_LIGHTING) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		)
	);

	assert(	"not other material but default lighting allowed; use special shaders for special stuff ;)." &&
			(mLocalShaderFeatures.visualMaterialType == VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING)
	 );

}

GenericLightingUberShader::~GenericLightingUberShader()
{
	//do nothing
}

//virtual bool operator==(const Shader& rhs)const;

void GenericLightingUberShader::use(SubObject* so)throw(SimulatorException)
{
	//TODO implement
}

//virtual void generateCustomDefines();
void GenericLightingUberShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
}


}
