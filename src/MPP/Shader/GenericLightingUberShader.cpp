/*
 * GenericLightingUberShader.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: tychi
 */

#include "GenericLightingUberShader.h"
#include "Simulator/OpenCL_Manager.h"

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

//	assert(	"not other material but default lighting allowed; use special shaders for special stuff ;)." &&
//			(mLocalShaderFeatures.visualMaterialType == VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING)
//	 );

}

GenericLightingUberShader::~GenericLightingUberShader()
{
	//do nothing
}



//void GenericLightingUberShader::fillOwnMatrixStructure(float* toFill, const Matrix4x4& in)
//{
//	for(unsigned int column=0; column<4; column++)
//	{
//		for(unsigned int element=0; element<4; element++)
//		{
//			toFill[(4*column) + element] = in[column][element];
//		}
//	}
//}



//virtual bool operator==(const Shader& rhs)const;
void GenericLightingUberShader::use(SubObject *so) throw (SimulatorException)
{
    //IMPORTANT: THIS USE FUNKTION IS A QUICK HACK TO TEST THE FIRST RENDERING;
    //FOR FULL COMPLIANCE TO THE TEMPLATE, THERE MUST MUCH MORE LOGIC BE PUT INTO THIS ROUTINE,
    //AND SOME STUFF LIKE INSTANCEMANAGER AND LIGHTSOURCE;AMANAGER MUST IMPLEMENT THERI BUFFFER FILLINGS
    // TODO TODO TODO

   GUARD(glUseProgram(mGLProgramHandle));

   Camera* mainCam = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

   //---------------------------------------------------------
   setupTransformationUniforms(mainCam,so);
   //----------------------------------------------------------
   if( (mLocalShaderFeatures.shadingFeatures & SHADING_FEATURE_DIRECT_LIGHTING) !=0)
   {
	   setupLightSourceUniforms(mainCam);
   }
//   //----------------------------------------------------------
//   if( ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
//		   != LIGHT_SOURCES_SHADOW_FEATURE_NONE )
//   {
//	   setupShadowUniforms(mainCam);
//   }
   //----------------------------------------------------------
   VisualMaterial * visMat =  dynamic_cast<VisualMaterial*>(so->getMaterial());
   assert(visMat);
   setupMaterialUniforms(visMat);
//---------- uncategorized uniforms to come ------------------------------------------------
	bindVector3D("eyePositionW",mainCam->getGlobalTransform().getPosition());

}

//virtual void generateCustomDefines();
void GenericLightingUberShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
}


}
