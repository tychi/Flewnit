/*
 * GrantleeShaderFeaturesContext.h
 *
 *  Created on: Feb 9, 2011
 *      Author: tychi
 */

#pragma once

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

#include "Simulator/SimulatorMetaInfo.h"



  class MyClass : public QObject
  {
    Q_OBJECT
    Q_ENUMS(PersonName)
    Q_PROPERTY(PersonName personName READ personName)
  public:

    enum PersonName
    {
      Mike,
      Natalie,
      Oliver,
      Patrica = 9,
      Quentin
    };

    MyClass(QObject *parent = 0 ){}
    virtual ~MyClass(){}

    PersonName personName() const { return Quentin; }

  };

  Q_DECLARE_METATYPE(MyClass*)





class GrantleeShaderFeaturesContext
: public QObject
{
	Q_OBJECT

	Q_ENUMS(RenderingTechnique)
	Q_ENUMS(TextureType)
	Q_ENUMS(VisualMaterialType)
	Q_ENUMS(ShadingFeatures)
	Q_ENUMS(LightSourcesLightingFeature)
	Q_ENUMS(LightSourcesShadowFeature)
	Q_ENUMS(ShadowTechnique)

	Q_PROPERTY(RenderingTechnique renderingTechnique READ renderingTechnique)
	Q_PROPERTY(TextureType renderTargetTextureType READ renderTargetTextureType)
	Q_PROPERTY(RenderingTechnique renderingTechnique READ renderingTechnique)

	//what to do about this bitflag??
	Q_PROPERTY(ShadingFeatures shadingFeatures READ shadingFeatures)

	Q_PROPERTY(bool instancedRendering READ instancedRendering)
	Q_PROPERTY(LightSourcesLightingFeature lightSourcesLightingFeature READ lightSourcesLightingFeature)
	Q_PROPERTY(LightSourcesShadowFeature lightSourcesShadowFeature READ lightSourcesShadowFeature)
	Q_PROPERTY(ShadowTechnique shadowTechnique READ shadowTechnique)
	Q_PROPERTY(int numMaxLightSources READ numMaxLightSources )
	Q_PROPERTY(float invNumMaxLightSources READ invNumMaxLightSources )

	Q_PROPERTY(int numMaxLightSources READ numMaxLightSources )
	Q_PROPERTY(float invNumMaxLightSources READ invNumMaxLightSources )

	Q_PROPERTY(int numMaxShadowCasters READ numMaxShadowCasters )
	Q_PROPERTY(float invNumMaxShadowCasters READ invNumMaxShadowCasters )

	Q_PROPERTY(int numMaxInstancesRenderable READ numMaxInstancesRenderable )
	Q_PROPERTY(float invNumMaxInstancesRenderable READ invNumMaxInstancesRenderable )

	Q_PROPERTY(TextureType GBufferType READ GBufferType )

	Q_PROPERTY(int numMultiSamples READ numMultiSamples )
	Q_PROPERTY(float invNumMultiSamples READ invNumMultiSamples )

	//Q_PROPERTY(ShaderFeaturesLocal mShaderFeaturesLocal READ mShaderFeaturesLocal )
	//Q_PROPERTY(ShaderFeaturesGlobal mShaderFeaturesGlobal READ mShaderFeaturesGlobal )

	Flewnit::ShaderFeaturesLocal mShaderFeaturesLocal;
	Flewnit::ShaderFeaturesGlobal mShaderFeaturesGlobal;

public:

	enum RenderingTechnique
	{
		RENDERING_TECHNIQUE_SHADOWMAP_GENERATION		=0,
		RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION 	=1,
		RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION		=2,
		RENDERING_TECHNIQUE_DEFAULT_LIGHTING			=3,
		RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING	=4,
		RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL		=5,
		RENDERING_TECHNIQUE_DEFERRED_LIGHTING			=6,
		RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION	=7,
		RENDERING_TECHNIQUE_CUSTOM						=8
	};
	enum LightSourcesLightingFeature
	{
		LIGHT_SOURCES_LIGHTING_FEATURE_NONE						=0,
		LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT			=1,
		LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT			=2,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS			=3,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS			=4,
		LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS	=5
	};
	enum LightSourcesShadowFeature
	{
		LIGHT_SOURCES_SHADOW_FEATURE_NONE			=0,
		LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT	=1,
		LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT	=2,
		LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	=3
	};
	enum ShadowTechnique
	{
		SHADOW_TECHNIQUE_NONE		=0,
		SHADOW_TECHNIQUE_DEFAULT	=1,
		SHADOW_TECHNIQUE_PCFSS		=2
	};
	enum ShadingFeatures
	{
		SHADING_FEATURE_NONE				=0,
		SHADING_FEATURE_DIRECT_LIGHTING		=1<<0,
		//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented
		SHADING_FEATURE_GLOBAL_LIGHTING		=1<<1,
		SHADING_FEATURE_DECAL_TEXTURING		=1<<2,
		SHADING_FEATURE_DETAIL_TEXTURING	=1<<3,
		SHADING_FEATURE_NORMAL_MAPPING		=1<<4,
		SHADING_FEATURE_CUBE_MAPPING		=1<<5,
		SHADING_FEATURE_AMBIENT_OCCLUSION	=1<<6,
		SHADING_FEATURE_TESSELATION			=1<<7,
	};
	enum VisualMaterialType
	{
		VISUAL_MATERIAL_TYPE_NONE					=0,
		VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING  		=1,
		VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING		=2,
		VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY		=3,	//just set a color value or something

		VISUAL_MATERIAL_TYPE_GAS_RENDERING			=4,
		VISUAL_MATERIAL_TYPE_LIQUID_RENDERING		=5
	};
	enum TextureType
	{
		TEXTURE_TYPE_1D						=0,
		TEXTURE_TYPE_1D_ARRAY				=1,

		TEXTURE_TYPE_2D						=2,
		TEXTURE_TYPE_2D_RECT				=3,
		TEXTURE_TYPE_2D_CUBE				=4,
		TEXTURE_TYPE_2D_ARRAY				=5,
		TEXTURE_TYPE_2D_MULTISAMPLE			=6,
		TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE 	=7,

		TEXTURE_TYPE_2D_DEPTH				=8,
		TEXTURE_TYPE_2D_RECT_DEPTH			=9,
		TEXTURE_TYPE_2D_CUBE_DEPTH			=10,
		TEXTURE_TYPE_2D_ARRAY_DEPTH			=11,
		TEXTURE_TYPE_2D_MULTISAMPLE_DEPTH 		=12, //not supported yet
		TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE_DEPTH 	=13, //not supported yet

		TEXTURE_TYPE_3D						=14
	};



	GrantleeShaderFeaturesContext(const Flewnit::ShaderFeaturesLocal& sfl, const Flewnit::ShaderFeaturesGlobal& sfg)
	: mShaderFeaturesLocal(sfl), mShaderFeaturesGlobal(sfg)
	{}
	virtual ~GrantleeShaderFeaturesContext(){}

	RenderingTechnique renderingTechnique(){
		return RENDERING_TECHNIQUE_SHADOWMAP_GENERATION;
		//return RenderingTechnique(mShaderFeaturesLocal.renderingTechnique);
	}
	TextureType renderTargetTextureType()const{return TextureType(mShaderFeaturesLocal.renderTargetTextureType);}
	VisualMaterialType visualMaterialType()const{return VisualMaterialType( mShaderFeaturesLocal.visualMaterialType);}
	ShadingFeatures shadingFeatures()const{return ShadingFeatures (mShaderFeaturesLocal.shadingFeatures);}
	bool instancedRendering()const{return mShaderFeaturesLocal.instancedRendering;}

	LightSourcesLightingFeature lightSourcesLightingFeature()const{return  LightSourcesLightingFeature(mShaderFeaturesGlobal.lightSourcesLightingFeature);}
	LightSourcesShadowFeature lightSourcesShadowFeature()const{return LightSourcesShadowFeature (mShaderFeaturesGlobal.lightSourcesShadowFeature);}
	ShadowTechnique shadowTechnique()const{return ShadowTechnique (mShaderFeaturesGlobal.shadowTechnique);}
	int numMaxLightSources()const{return mShaderFeaturesGlobal.numMaxLightSources;}
	int numMaxShadowCasters()const{return mShaderFeaturesGlobal.numMaxShadowCasters;}
	int numMaxInstancesRenderable()const{return mShaderFeaturesGlobal.numMaxInstancesRenderable;}
	TextureType GBufferType()const{return TextureType(mShaderFeaturesGlobal.GBufferType);}
	int numMultiSamples()const{return mShaderFeaturesGlobal.numMultiSamples;}

	float invNumMaxLightSources()const{return 1.0f/mShaderFeaturesGlobal.numMaxLightSources;}
	float invNumMaxShadowCasters()const{return 1.0f/mShaderFeaturesGlobal.numMaxShadowCasters;}
	float invNumMaxInstancesRenderable()const{return 1.0f/mShaderFeaturesGlobal.numMaxInstancesRenderable;}
	float invNumMultiSamples()const{return 1.0f/mShaderFeaturesGlobal.numMultiSamples;}
};


Q_DECLARE_METATYPE(GrantleeShaderFeaturesContext*)
