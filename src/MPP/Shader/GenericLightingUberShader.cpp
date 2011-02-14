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

void GenericLightingUberShader::use(SubObject* so)throw(SimulatorException)
{
	//IMPORTANT: THIS USE FUNKTION IS A QUICK HACK TO TEST THE FIRST RENDERING;
	//FOR FULL COMPLIANCE TO THE TEMPLATE, THERE MUST MUCH MORE LOGIC BE PUT INTO THIS ROUTINE,
	//AND SOME STUFF LIKE INSTANCEMANAGER AND LIGHTSOURCE;AMANAGER MUST IMPLEMENT THERI BUFFFER FILLINGS
	// TODO TODO TODO



	glBindAttribLocation(mGLProgramHandle,0,"inVPosition");
	glBindAttribLocation(mGLProgramHandle,1,"inVNormal");
	//glBindAttribLocation(mGLProgramHandle,2,"inVPosition");
	glLinkProgram(mGLProgramHandle);

	Camera* mainCam = URE_INSTANCE->getSimulator(VISUAL_SIM_DOMAIN)->toLightingSimulator()->getMainCamera();
	Matrix4x4 viewProjMatrix = mainCam->getProjectionMatrix() * mainCam->getGlobalTransform().getLookAtMatrix();

	LOG<<DEBUG_LOG_LEVEL<<"Camera Transformation Matrix:"<< mainCam->getGlobalTransform().getTotalTransform()<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"Camera Lookat Matrix:"<< mainCam->getGlobalTransform().getLookAtMatrix()<<";\n";

	LOG<<DEBUG_LOG_LEVEL<<"Camera Projection Matrix:"<<mainCam->getProjectionMatrix()<<";\n";
	LOG<<DEBUG_LOG_LEVEL<<"Camera View Projection Matrix:"<<mainCam->getProjectionMatrix()<<";\n";

//	Matrix4x4 hardcodeViewMat = glm::lookAt(Vector3D(0.0f,0.0f,30.0f),Vector3D(0.0f,0.0f,0.0f),Vector3D(0.0f,1.0f,0.0f));
//	Matrix4x4 hardcodeProjMat = glm::gtc::matrix_projection::perspective(45.0f,1.0f,0.1f,100.0f);
//	Matrix4x4 hardcodeViewProjMat = hardcodeProjMat * hardcodeViewMat;
//
//	Matrix4x4 hardCodeModelMat = glm::gtc::matrix_transform::translate(Matrix4x4(), Vector3D(0.0f,0.0f,-10.0f));
//	Matrix4x4 hardCodeModelViewMat =  hardcodeViewMat * hardCodeModelMat;
//	Matrix4x4 hardCodeModelViewProjMat = hardcodeViewProjMat * hardCodeModelMat;
//
//	float ownmat[16];
//	fillOwnMatrixStructure(ownmat,hardcodeViewMat);

	GUARD(glUseProgram(mGLProgramHandle));

//	GLint uniformLocView = glGetUniformLocation(mGLProgramHandle,"viewMatrix");
//	GLint uniformLocmViewProj = glGetUniformLocation(mGLProgramHandle,"modelViewProjectionMatrix");
//
//	LOG<<DEBUG_LOG_LEVEL<<"shader program handle: "<<mGLProgramHandle<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"modelViewProjectionMatrix uniform loc: "<<uniformLocmViewProj<<";\n";

	GUARD(
			glUniformMatrix4fv(
			glGetUniformLocation(mGLProgramHandle,"viewMatrix"),
			1,
			GL_FALSE,
			&(mainCam->getGlobalTransform().getLookAtMatrix()[0][0])
			//&(hardcodeViewMat[0][0] )
			//ownmat
		)
	);

	//fillOwnMatrixStructure(ownmat,hardcodeViewProjMat);
	GUARD(
			glUniformMatrix4fv(
			glGetUniformLocation(mGLProgramHandle,"viewProjectionMatrix"),
			1,
			GL_FALSE,
			&(viewProjMatrix[0][0])
			//&( hardcodeViewProjMat[0][0] )
			//ownmat
		)
	);

	//----------------------------------------------
	const Matrix4x4& modelMatrix = so->getOwningWorldObject()->getGlobalTransform().getTotalTransform();
	Matrix4x4 modelViewMatrix = mainCam->getGlobalTransform().getLookAtMatrix() * modelMatrix;
	Matrix4x4 modelViewProjMatrix = mainCam->getProjectionMatrix() * modelViewMatrix;


	//fillOwnMatrixStructure(ownmat,hardCodeModelMat);
	GUARD(
			glUniformMatrix4fv(
			glGetUniformLocation(mGLProgramHandle,"modelMatrix"),
			1,
			GL_FALSE,
			&(modelMatrix[0][0])
			//ownmat
		)
	);
	//fillOwnMatrixStructure(ownmat,hardCodeModelViewMat);
	GUARD(
			glUniformMatrix4fv(
			glGetUniformLocation(mGLProgramHandle,"modelViewMatrix"),
			1,
			GL_FALSE,
			&(modelViewMatrix[0][0])
			//ownmat
		)
	);
	//fillOwnMatrixStructure(ownmat,hardCodeModelViewProjMat);
	GUARD(
			glUniformMatrix4fv(
			glGetUniformLocation(mGLProgramHandle,"modelViewProjectionMatrix"),
			1,
			GL_FALSE,
			&(modelViewProjMatrix[0][0])
			//ownmat
		)
	);

	//-----------------------------------------------------------------------
	const LightSourceShaderStruct& lsStruct = LightSourceManager::getInstance().getLightSource(0)->getdata();

	GUARD(
		glUniform3fv(
			glGetUniformLocation(mGLProgramHandle,"lightSource.position"),
			1,
			&(lsStruct.position[0])
		)
	);
	GUARD(
		glUniform3fv(
			glGetUniformLocation(mGLProgramHandle,"lightSource.diffuseColor"),
			1,
			&(lsStruct.diffuseColor[0])
		)
	);
	GUARD(
			glUniform3fv(
				glGetUniformLocation(mGLProgramHandle,"lightSource.specularColor"),
				1,
				&(lsStruct.specularColor[0])
			)
	);
	GUARD(
			glUniform3fv(
				glGetUniformLocation(mGLProgramHandle,"lightSource.direction"),
				1,
				&(lsStruct.direction[0])
			)
	);
	GUARD(	glUniform1f(	glGetUniformLocation(mGLProgramHandle,"lightSource.innerSpotCutOff_Radians"),
							lsStruct.innerSpotCutOff_Radians	)											);
	GUARD(	glUniform1f(	glGetUniformLocation(mGLProgramHandle,"lightSource.outerSpotCutOff_Radians"),
							lsStruct.outerSpotCutOff_Radians	)											);
	GUARD(	glUniform1f(	glGetUniformLocation(mGLProgramHandle,"lightSource.spotExponent"),
							lsStruct.spotExponent	)											);
	GUARD(	glUniform1f(	glGetUniformLocation(mGLProgramHandle,"lightSource.shadowMapLayer"),
							lsStruct.shadowMapLayer	)											);

	GUARD(glUseProgram(mGLProgramHandle));

	//TODO implement
}

//virtual void generateCustomDefines();
void GenericLightingUberShader::bindFragDataLocations(RenderTarget* rt) throw(BufferException)
{
	//TODO implement
}


}
