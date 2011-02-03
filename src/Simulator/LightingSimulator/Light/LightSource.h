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


//#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
//
////make some mods so that the c++-code can use this definition, too
//#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
//#	define NAMESPACE_PREFIX glm::
//#else
//#	define NAMESPACE_PREFIX
//#endif
//
//struct LightSourceShaderStruct
//{
//	NAMESPACE_PREFIX vec3 position;
//	NAMESPACE_PREFIX vec3 diffuseColor;
//	//a dedicated specular color to produce unrealistic but nice effects;
//	NAMESPACE_PREFIX vec3 specularColor;
//
//	//----------------------------------------------------------------------
//	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
//	//to indicate a point light, those valus are all zero
//	NAMESPACE_PREFIX vec3 direction;
//	//value beyond with will be no lighting, to produce a nice light circle and to
//	//hide the rectangular shape of the shadowmap ;)
//	float innerSpotCutOff_Radians;	//serves also as indicator if the source shall be treated as spot or not (zero= pointlight ;) )
//	float outerSpotCutOff_Radians;
//	float spotExponent;
//	//alignment is everything :P
//
//	float shadowMapLayer;
//
//	//aligned to 64 bytes;
//
//
//};

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
	LightSource(String name, LightSourceType type, bool castsShadows,
			const LightSourceShaderStruct& data);
public:


	virtual ~LightSource();

	inline LightSourceType getType()const{return mType;}
	inline bool castsShadows()const{return mCastsShadows;}
	inline const LightSourceShaderStruct& getdata()const{return mLightSourceShaderStruct;}

private:

	//check for valid ranges etc..; called by constructor
	void validateData() throw(SimulatorException);

	LightSourceType mType;
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
	PointLight(String name, bool castsShadows,
			const LightSourceShaderStruct& data);

public:
	//mPointLightShadowMapNonTranslationalViewMatrices[whichFace] * translate(globalPosition);
	Matrix4x4 getViewMatrix(int whichFace)const;
	Matrix4x4 getViewProjectionMatrix(int whichFace, float nearClipPlane, float farClipPlane)const;

private:

	//projection matrices
	//refer to the omni-directional Shadows- paper, slide 14:
	Matrix4x4 mPointLightShadowMapNonTranslationalViewMatrices[6];
	//45°, 1/1 ratio
	Matrix4x4 mPointLightShadowMapProjectionMatrix;

 };


class SpotLight
:public LightSource
 {
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	friend class LightSourceManager;
	//private constructor so that only the LS manager can create sources :);
	//like this, compatibility to shader, shadowmap buffers etc. is much easier to
	//enforce;
	SpotLight(String name, bool castsShadows,
			const LightSourceShaderStruct& data);

public:

	//following functions cannot be tagged "const" as the getter for the amendedtransform is
	//not const (for direct manipulation); but the functions won't change anything; in the lightsource state;

	//shortcut to SceneNode::mGlobalAmendedTransform::getViewMatrix();
	Matrix4x4 getViewMatrix();
	//Projective part is constructed like this;
	// 	opening angle from outerSpotCutOff_Radians,
	//	aspect ratio = 1/1
	//a squared septh texture is recommended;
	Matrix4x4 getViewProjectionMatrix(float nearClipPlane, float farClipPlane);
	//matrix for shadowmap lookup; scale is configuarable in case one wants to use rectangle texture for whatever reason...
	Matrix4x4 getBiasedViewProjectionMatrix(float nearClipPlane, float farClipPlane,
			float scaleBias= 0.5f, float translationBias= 0.5f);

private:

	//no matrix members, derived from scene node state or generated on the fly on demand;

 };

}

