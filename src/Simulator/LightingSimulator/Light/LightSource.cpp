/*
 * LightSource.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSource.h"

#include "LightSourceManager.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"

namespace Flewnit
{

Matrix4x4 PointLight::mPointLightShadowMapNonTranslationalViewMatrices[6] =
	{
			//rotational part of the view matrices
			//refer to the omni-directional Shadows- paper, slide 14:
			//+x
			Matrix4x4(
			0.0f,	0.0f,	-1.0f,	0.0f,
			0.0f,	-1.0f,	0.0f,	0.0f,
			-1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
			//-x
			Matrix4x4(
			0.0f,	0.0f,	1.0f,	0.0f,
			0.0f,	-1.0f,	0.0f,	0.0f,
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
			//+y
			Matrix4x4(
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	-1.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
			//-y
			Matrix4x4(
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			0.0f,	-1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
			//+z
			Matrix4x4(
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	-1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	-1.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
			//-z
			Matrix4x4(
			-1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	-1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f),
	};


LightSource::LightSource(String name, LightSourceType type, bool castsShadows,  bool isEnabled,
			const LightSourceShaderStruct& data)
: WorldObject(
	name,
	LIGHT_NODE,
	AmendedTransform(
			Vector3D(data.position),
			//set the direction:
			(type == LIGHT_SOURCE_TYPE_SPOT_LIGHT)
				? glm::normalize(Vector3D(data.direction))
				: Vector3D(0.0f,0.0f,-1.0f),
			//set the up vector:
			(type == LIGHT_SOURCE_TYPE_SPOT_LIGHT)
				//is direction and y-axis collinear (dot product of normalized vecs ==1)?
				? ( (	std::fabs(
							glm::dot(
								glm::normalize(Vector3D(data.direction)),Vector3D(0.0f,1.0f,0.0f)
							)
						) > 0.98f
					//if collinear (spotlight looks up or down), then take x-axis as upvector, else the default y-axis^^
					)  ? Vector3D(1.0f,0.0f,0.0f)  : Vector3D(0.0f,1.0f,0.0f) )
				: Vector3D(0.0f,1.0f,0.0f)
	)
  ),
  mType(type),
  mIsEnabled(isEnabled),
  mCastsShadows(castsShadows),
  mLightSourceShaderStruct(data)
{
	validateData();
}



LightSource::~LightSource()
{
	LightSourceManager::getInstance().unregisterLightSource(this);
}

void LightSource::setEnable(bool val)
{
	mIsEnabled = val;
}



void LightSource::validateData() throw(SimulatorException)
{
	assert("at least one diffuse color channel has positive value, else it would be waste of computations" && glm::any(glm::greaterThan(mLightSourceShaderStruct.diffuseColor, Vector4D(0.0f,0.0f,0.0f,0.0f))));
	assert("color vals are non-negative" && glm::all(glm::greaterThanEqual(mLightSourceShaderStruct.diffuseColor, Vector4D(0.0f,0.0f,0.0f,0.0f))));
	assert("color vals are non-negative" && glm::all(glm::greaterThanEqual(mLightSourceShaderStruct.specularColor, Vector4D(0.0f,0.0f,0.0f,0.0f))));


	switch(mType)
	{
	case LIGHT_SOURCE_TYPE_SPOT_LIGHT:
		assert( "direction is normalized" && std::fabs( glm::length(mLightSourceShaderStruct.direction)) -1.0f < 0.01 );
		assert("opening angle not too big" && mLightSourceShaderStruct.outerSpotCutOff_Radians <= glm::radians(75.0f));
		assert("inner angle not bigger than outer" && mLightSourceShaderStruct.innerSpotCutOff_Radians <= mLightSourceShaderStruct.outerSpotCutOff_Radians);
		assert("opening angles positive" && (mLightSourceShaderStruct.innerSpotCutOff_Radians >0.0f) && (mLightSourceShaderStruct.outerSpotCutOff_Radians >0.0f));
		assert("array layer negative for non-shadowcaster and non-negative for shadow caster" &&
				((mCastsShadows)==(mLightSourceShaderStruct.shadowMapLayer>=0.0f)) );
		break;
	case LIGHT_SOURCE_TYPE_POINT_LIGHT:
		assert(
				"all spotlightvalues zero but the direction and shadowmap layer" &&
				(mLightSourceShaderStruct.innerSpotCutOff_Radians == 0.0f) &&
				(mLightSourceShaderStruct.outerSpotCutOff_Radians == 0.0f)
		);
		break;
	default:
		throw(SimulatorException("unknown lightsource type"));
		break;
	};
}



//private constructor so that only the LS manager can create sources :);
//like this, compatibility to shader, shadowmap buffers etc. is much easier to
//enforce;
PointLight::PointLight(String name, bool castsShadows, bool isEnabled,
			const LightSourceShaderStruct& data)
: LightSource(name, LIGHT_SOURCE_TYPE_POINT_LIGHT,castsShadows,isEnabled,data)
{
	assert(	"rotationMatrix tightly packed" &&
			(sizeof(mPointLightShadowMapNonTranslationalViewMatrices) == 6* sizeof(Matrix4x4))
	 &&		(sizeof(Matrix4x4) == 16* sizeof(float) )
	);
}


PointLight::~PointLight()
{
	//nothing to do
}


//mPointLightShadowMapNonTranslationalViewMatrices[whichFace] * translate(globalPosition);
Matrix4x4 PointLight::getViewMatrix(int whichFace)
{
	assert(whichFace<6);

	return mPointLightShadowMapNonTranslationalViewMatrices[whichFace]
	       *
	       glm::translate(getGlobalTransform().getPosition());
}



Matrix4x4 PointLight::getViewProjectionMatrix(int whichFace)
{
	assert(whichFace<6);

	return
		glm::gtc::matrix_projection::perspective(
				45.0f,
				1.0f,
				LightSourceManager::getInstance().getLightSourceProjectionMatrixNearClipPlane(),
				LightSourceManager::getInstance().getLightSourceProjectionMatrixFarClipPlane()
		)
		*
		getViewMatrix(whichFace);
}






//private constructor so that only the LS manager can create sources :);
//like this, compatibility to shader, shadowmap buffers etc. is much easier to
//enforce;
SpotLight::SpotLight(String name, bool castsShadows,  bool isEnabled,
		const LightSourceShaderStruct& data)
: LightSource(name, LIGHT_SOURCE_TYPE_SPOT_LIGHT,castsShadows,isEnabled,data)
{
}



SpotLight::~SpotLight()
{
	//nothing to do
}



	//shortcut to SceneNode::mGlobalAmendedTransform::getViewMatrix();
Matrix4x4 SpotLight::getViewMatrix()
{
	return getGlobalTransform().getLookAtMatrix();
}



//Projective part is constructed like this;
// 	opening angle from outerSpotCutOff_Radians,
//	aspect ratio = 1/1
//a squared septh texture is strongly recommended;
Matrix4x4 SpotLight::getViewProjectionMatrix()
{
	return
		glm::gtc::matrix_projection::perspective(
				glm::degrees( getdata().outerSpotCutOff_Radians ),
				1.0f, //assum square shadow map
				LightSourceManager::getInstance().getLightSourceProjectionMatrixNearClipPlane(),
				LightSourceManager::getInstance().getLightSourceProjectionMatrixFarClipPlane()
		)
		*
		getViewMatrix();
}

	//matrix for shadowmap lookup; scale is configuarable in case one wants to use rectangle texture for whatever reason...
Matrix4x4 SpotLight::getBiasedViewProjectionMatrix(float scale, float translation)
{
	return
			glm::scaleBias(scale,translation)
			*
			getViewProjectionMatrix();
}


//matrix for shadowmap lookup in VIEW space: bias*perspLight*viewLight * (camView)⁻1 ;
//returns getBiasedViewProjectionMatrix(scale,translation) * spectatorCam->getViewMatrix().inverse();
//scale is configurable in case one wants to use rectangle texture for whatever reason...
Matrix4x4 SpotLight::getViewSpaceShadowMapLookupMatrix( Camera* spectatorCam,
		float scale, float translation)
{
	//this could be greatly optimized, the matrixes can be stored instead of recalculated every time;
	//plus, all those inverser for sure break down to a simple wolrdm amitrx without any inverses ;)
	//TODO in faar future ;)
	return getBiasedViewProjectionMatrix(scale,translation) * glm::inverse(spectatorCam->getViewMatrix());
}




}
