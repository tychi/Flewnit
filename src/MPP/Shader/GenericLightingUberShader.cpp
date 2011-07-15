/*
 * GenericLightingUberShader.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: tychi
 */

#include "GenericLightingUberShader.h"
#include "Simulator/ParallelComputeManager.h"

#include "URE.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"
#include "Simulator/SimulatorInterface.h"
#include "Simulator/LightingSimulator/LightingSimulator.h"
#include "WorldObject/WorldObject.h"
#include "WorldObject/SubObject.h"
#include "Scene/SceneNode.h"
#include "Simulator/LightingSimulator/Light/LightSource.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Util/Log/Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Material/VisualMaterial.h"
#include "Buffer/BufferInterface.h"
#include "Buffer/Texture.h"
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit
{


//public constructor for deferred lighting (which does not involve material classes and hence
//cannot trigger automatic generation by ShaderManager)
GenericLightingUberShader::GenericLightingUberShader(Path codeDirectory, const ShaderFeaturesLocal& localShaderFeatures)
:
	Shader(codeDirectory, Path("GenericLightingUberShader"), localShaderFeatures,
				String("GenericLightingUberShader"))
{
	build();

	assert( "only lighting stuff allowed in this shader type" &&
		(
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFAULT_LIGHTING) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL) ||
			( mLocalShaderFeatures.renderingTechnique == RENDERING_TECHNIQUE_DEFERRED_LIGHTING)
		)
	);

//	assert(	"not other material but default lighting allowed; use special shaders for special stuff ;)." &&
//			(mLocalShaderFeatures.visualMaterialType == VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING)
//	 );

}

GenericLightingUberShader::~GenericLightingUberShader()
{
	//do nothing
}




//virtual bool operator==(const Shader& rhs)const;
void GenericLightingUberShader::use(SubObject *so) throw (SimulatorException)
{
   GUARD(glUseProgram(mGLProgramHandle));


   //---------------------------------------------------------
   setupTransformationUniforms(so);
   //----------------------------------------------------------
   if( (mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_DIRECT_LIGHTING) !=0)
   {
	   setupLightSourceUniforms();
   }
   //----------------------------------------------------------
   VisualMaterial * visMat =  dynamic_cast<VisualMaterial*>(so->getMaterial());
   assert(visMat);
   setupMaterialUniforms(visMat);

   setupShadowMapUniform(visMat);

//---------- uncategorized uniforms to come ------------------------------------------------
	bindVector3D("eyePositionW",URE_INSTANCE->getCurrentlyActiveCamera()->getGlobalTransform().getPosition());

}

//virtual void generateCustomDefines();
void GenericLightingUberShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
}


}
