/*
 * LightSource.h
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#pragma once

#include "WorldObject/WorldObject.h"

namespace Flewnit
{

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/Shader/ShaderSources/GenericLightingUberShader/04_Fragment_appCodeIncludable_dataTypes.glsl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE


enum LightSourceType
{
	LIGHT_SOURCE_TYPE_POINT_LIGHT,
	LIGHT_SOURCE_TYPE_SPOT_LIGHT
	//maybe for later ;)
	//LIGHT_SOURCE_TYPE_AREA_LIGHT,
};

class LightSource
:public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

protected:
	//quasi-abstract class: protected constructor so that only derived classes can instantiate;
	//like this, compatibility to shader, shadowmap buffers etc. is much easier to
	//enforce;
	LightSource(String name, LightSourceType type, bool castsShadows, bool isEnabled,
			const LightSourceShaderStruct& data);
public:
	virtual ~LightSource();

	void setEnable(bool val);

	inline LightSourceType getType()const{return mType;}
	inline bool castsShadows()const{return mCastsShadows;}
	inline bool isEnabled()const{return mIsEnabled;}
	inline LightSourceShaderStruct& getdata(){return mLightSourceShaderStruct;}


	//void destroyedCallBack();

private:

	//check for valid ranges etc..; called by constructor
	void validateData() throw(SimulatorException);
	//indicate the LS manager that the lightsource has been destroyed

	LightSourceType mType;
	bool mIsEnabled;
	bool mCastsShadows;
	LightSourceShaderStruct mLightSourceShaderStruct;

};

class PointLight
:public LightSource
 {
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	friend class LightSourceManager;
	//private constructor so that only the LS manager can create sources :);
	//like this, compatibility to shader, shadowmap buffers etc. is much easier to
	//enforce;
	PointLight(String name, bool castsShadows, bool isEnabled,
			const LightSourceShaderStruct& data);

public:
	virtual ~PointLight();

	//mPointLightShadowMapNonTranslationalViewMatrices[whichFace] * translate(globalPosition);
	Matrix4x4 getViewMatrix(int whichFace);
	Matrix4x4 getProjectionMatrix(int whichFace);
	//perspective(45,1,globalLsNearClipPlane,globalLsFarClipPlane) * getViewMatrix(which)
	Matrix4x4 getViewProjectionMatrix(int whichFace);

private:

	//rotational part of the view matrices
	//refer to the omni-directional Shadows- paper, slide 14:
	static Matrix4x4 mPointLightShadowMapNonTranslationalViewMatrices[6];

	//45°, 1/1 ratio
	//Matrix4x4 mPointLightShadowMapProjectionMatrix;

 };



class SpotLight
:public LightSource
 {
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	friend class LightSourceManager;
	//private constructor so that only the LS manager can create sources :);
	//like this, compatibility to shader, shadowmap buffers etc. is much easier to
	//enforce;
	SpotLight(String name, bool castsShadows,  bool isEnabled,
			const LightSourceShaderStruct& data);

public:
	virtual ~SpotLight();

	//following functions cannot be tagged "const" as the getter for the amendedtransform is
	//not const (for direct manipulation); but the functions won't change anything; in the lightsource state;

	//shortcut to SceneNode::mGlobalAmendedTransform::getViewMatrix();
	Matrix4x4 getViewMatrix();
	//Projective part is constructed like this;
	// 	opening angle from outerSpotCutOff_Radians,
	//	aspect ratio = 1/1
	//a squared depth texture is recommended;
	Matrix4x4 getProjectionMatrix();
	Matrix4x4 getViewProjectionMatrix();
	//matrix for shadowmap lookup in WORLD space: bias*perspLight*viewLight;
	//scale is configurable in case one wants to use rectangle texture for whatever reason...
	Matrix4x4 getBiasedViewProjectionMatrix(
			float scale= 0.5f, float translation= 0.5f);

	//matrix for shadowmap lookup in VIEW space: bias*perspLight*viewLight * (camView)⁻1 ;
	//returns getBiasedViewProjectionMatrix(scale,translation) * spectatorCam->getViewMatrix().inverse();
	//scale is configurable in case one wants to use rectangle texture for whatever reason...
	Matrix4x4 getViewSpaceShadowMapLookupMatrix( Camera* spectatorCam,
			float scale= 0.5f, float translation= 0.5f);

private:

	//no matrix members, derived from scene node state or generated on the fly on demand;

 };

}

