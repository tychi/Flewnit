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



void GenericLightingUberShader::fillOwnMatrixStructure(float* toFill, const Matrix4x4& in)
{
	for(unsigned int column=0; column<4; column++)
	{
		for(unsigned int element=0; element<4; element++)
		{
			toFill[(4*column) + element] = in[column][element];
		}
	}
}



   	//virtual bool operator==(const Shader& rhs)const;
    void GenericLightingUberShader::use(SubObject *so) throw (SimulatorException)
    {
        //IMPORTANT: THIS USE FUNKTION IS A QUICK HACK TO TEST THE FIRST RENDERING;
        //FOR FULL COMPLIANCE TO THE TEMPLATE, THERE MUST MUCH MORE LOGIC BE PUT INTO THIS ROUTINE,
        //AND SOME STUFF LIKE INSTANCEMANAGER AND LIGHTSOURCE;AMANAGER MUST IMPLEMENT THERI BUFFFER FILLINGS
        // TODO TODO TODO


        GUARD(glUseProgram(mGLProgramHandle));

        Camera* mainCam = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();

        setupMatrixUniforms(mainCam,so);

        Matrix4x4 viewProjMatrix = mainCam->getProjectionMatrix() * mainCam->getGlobalTransform().getLookAtMatrix();
        const Matrix4x4 & modelMatrix = so->getOwningWorldObject()->getGlobalTransform().getTotalTransform();
        Matrix4x4 modelViewMatrix = mainCam->getGlobalTransform().getLookAtMatrix() * modelMatrix;
        Matrix4x4 modelViewProjMatrix = mainCam->getProjectionMatrix() * modelViewMatrix;

        GUARD(
        	glUniformMatrix4fv(
        		glGetUniformLocation(mGLProgramHandle,"viewMatrix"),
        		1,
        		GL_FALSE,
        		&(mainCam->getGlobalTransform().getLookAtMatrix()[0][0])
        	)
        );
        GUARD(
        	glUniformMatrix4fv(
        		glGetUniformLocation(mGLProgramHandle,"viewProjectionMatrix"),
        		1,
        		GL_FALSE,
        		&(viewProjMatrix[0][0])
        	)
        );
        //----------------------------------------------

        GUARD(
			glUniformMatrix4fv(
				glGetUniformLocation(mGLProgramHandle,"modelMatrix"),
				1,
				GL_FALSE,
				&(modelMatrix[0][0])
				//ownmat
			)
        );
        GUARD(
			glUniformMatrix4fv(
				glGetUniformLocation(mGLProgramHandle,"modelViewMatrix"),
				1,
				GL_FALSE,
				&(modelViewMatrix[0][0])
			)
		);
        GUARD(
			glUniformMatrix4fv(
				glGetUniformLocation(mGLProgramHandle,"modelViewProjectionMatrix"),
				1,
				GL_FALSE,
				&(modelViewProjMatrix[0][0])
			)
		);

//        GUARD(
//			glUniformMatrix3fv(
//				glGetUniformLocation(mGLProgramHandle,"inverseViewRotationMatrix"),
//				1,
//				GL_FALSE,
//				&(   [0][0])
//			)
//		);




    //----------------------------------------------------------

     setupLightSourceUniforms(mainCam);

    //----------------------------------------------------------

   VisualMaterial * visMat =  dynamic_cast<VisualMaterial*>(so->getMaterial());
	if(visMat &&  ((visMat->getShadingFeatures() & SHADING_FEATURE_DECAL_TEXTURING ) !=0 ))
	{
		glActiveTexture(GL_TEXTURE0);
		visMat->getTexture(DECAL_COLOR_SEMANTICS)->bind(OPEN_GL_CONTEXT_TYPE);


		GUARD(	glUniform1i(glGetUniformLocation(mGLProgramHandle,"decalTexture"),
					0	)
		);
	}


	//---------- uncategrorize uniforms to come ------------------------------------------------

	GUARD(
		glUniform3fv(
			glGetUniformLocation(mGLProgramHandle,"eyePositionW"),
			1,
			&(mainCam->getGlobalTransform().getPosition()[0])
		)
	);

}

//virtual void generateCustomDefines();
void GenericLightingUberShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
}


}
