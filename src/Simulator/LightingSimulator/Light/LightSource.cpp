/*
 * LightSource.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSource.h"

#include "LightSourceManager.h"

namespace Flewnit
{
	//TODO call callback on deletion so that the LS manager doesn't call detructor on invalid pointers;


LightSource::LightSource(String name, LightSourceType type, bool castsShadows,  bool isEnabled,
			const LightSourceShaderStruct& data)
: WorldObject(
	name,
	LIGHT_NODE,
	AmendedTransform(
			data.position,
			//set the direction:
			(type == LIGHT_SOURCE_TYPE_SPOT_LIGHT)
				? glm::normalize(data.direction)
				: Vector3D(0.0f,0.0f,-1.0f),
			//set the up vector:
			(type == LIGHT_SOURCE_TYPE_SPOT_LIGHT)
				//is direction and y-axis collinear (dot product of normalized vecs ==1)?
				? ( (	std::fabs(
							glm::dot(
								glm::normalize(data.direction),Vector3D(0.0f,1.0f,0.0f)
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
	assert("color vals are positive" && glm::all(glm::greaterThan(mLightSourceShaderStruct.diffuseColor, Vector3D(0.0f,0.0f,0.0f))));
	assert("color vals are non-negative" && glm::all(glm::greaterThanEqual(mLightSourceShaderStruct.specularColor, Vector3D(0.0f,0.0f,0.0f))));


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
//a squared septh texture is recommended;
Matrix4x4 SpotLight::getViewProjectionMatrix()
{
	return
		glm::gtc::matrix_projection::perspective(
				glm::degrees( getdata().outerSpotCutOff_Radians ),
				1.0f,
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


}
