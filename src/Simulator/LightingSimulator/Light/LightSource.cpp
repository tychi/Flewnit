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


//LightSource::LightSource(String name, LightSourceType type, bool castsShadows,
//			const LightSourceShaderStruct& data)
//
//{
//	//TODO
//}



LightSource::~LightSource()
{
	LightSourceManager::getInstance().unregisterLightSource(this);
}



void LightSource::validateData() throw(SimulatorException)
{

}



//private constructor so that only the LS manager can create sources :);
//like this, compatibility to shader, shadowmap buffers etc. is much easier to
//enforce;
//PointLight::PointLight(String name, bool castsShadows,
//			const LightSourceShaderStruct& data)
//{
//	//TODO
//}


PointLight::~PointLight()
{

}


//mPointLightShadowMapNonTranslationalViewMatrices[whichFace] * translate(globalPosition);
Matrix4x4 PointLight::getViewMatrix(int whichFace)const
{

}



Matrix4x4 PointLight::getViewProjectionMatrix(int whichFace, float nearClipPlane, float farClipPlane)const
{

}






//private constructor so that only the LS manager can create sources :);
//like this, compatibility to shader, shadowmap buffers etc. is much easier to
//enforce;
//SpotLight::SpotLight(String name, bool castsShadows,
//			const LightSourceShaderStruct& data)
//{
//	//TODO
//}



SpotLight::~SpotLight()
{

}



	//shortcut to SceneNode::mGlobalAmendedTransform::getViewMatrix();
Matrix4x4 SpotLight::getViewMatrix()
{

}



//Projective part is constructed like this;
// 	opening angle from outerSpotCutOff_Radians,
//	aspect ratio = 1/1
//a squared septh texture is recommended;
Matrix4x4 SpotLight::getViewProjectionMatrix(float nearClipPlane, float farClipPlane)
{

}

	//matrix for shadowmap lookup; scale is configuarable in case one wants to use rectangle texture for whatever reason...
Matrix4x4 SpotLight::getBiasedViewProjectionMatrix(float nearClipPlane, float farClipPlane,
			float scaleBias, float translationBias)
{

}


}
