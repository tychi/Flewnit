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


#include "Simulator/ShaderFeatureRelevantEnums.h"




//  class MyClass : public QObject
//  {
//    Q_OBJECT
//    Q_ENUMS(PersonName)
//    Q_PROPERTY(PersonName personName READ personName)
//  public:
//
//    enum PersonName
//    {
//      Mike,
//      Natalie,
//      Oliver,
//      Patrica = 9,
//      Quentin
//    };
//
//    MyClass(QObject *parent = 0 ){}
//    virtual ~MyClass(){}
//
//    PersonName personName() const { return Quentin; }
//
//  };
//
//  Q_DECLARE_METATYPE(MyClass*)
//
//
//
//
//
//class GrantleeShaderFeaturesContext
//: public QObject
//{
//	Q_OBJECT
//
//	Q_ENUMS(ShaderFeatureEnumQTClass::RenderingTechnique)
//	Q_ENUMS(ShaderFeatureEnumQTClass::TextureType)
//	Q_ENUMS(ShaderFeatureEnumQTClass::VisualMaterialType)
//	Q_ENUMS(ShaderFeatureEnumQTClass::ShadingFeatures)
//	Q_ENUMS(ShaderFeatureEnumQTClass::LightSourcesLightingFeature)
//	Q_ENUMS(ShaderFeatureEnumQTClass::LightSourcesShadowFeature)
//	Q_ENUMS(ShaderFeatureEnumQTClass::ShadowTechnique)
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::RenderingTechnique renderingTechnique READ renderingTechnique)
//	Q_PROPERTY(bool renderingTechnique READ renderingTechnique)
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::TextureType renderTargetTextureType READ renderTargetTextureType)
//
//	//what to do about this bitflag??
//	Q_PROPERTY(ShaderFeatureEnumQTClass::ShadingFeatures shadingFeatures READ shadingFeatures)
//
//
//
//	Q_PROPERTY(bool instancedRendering READ instancedRendering)
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::LightSourcesLightingFeature lightSourcesLightingFeature READ lightSourcesLightingFeature)
//
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::LightSourcesShadowFeature lightSourcesShadowFeature READ lightSourcesShadowFeature)
//
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::ShadowTechnique shadowTechnique READ shadowTechnique)
//
//
//	Q_PROPERTY(int numMaxLightSources READ numMaxLightSources )
//	Q_PROPERTY(float invNumMaxLightSources READ invNumMaxLightSources )
//
//	Q_PROPERTY(int numMaxLightSources READ numMaxLightSources )
//	Q_PROPERTY(float invNumMaxLightSources READ invNumMaxLightSources )
//
//	Q_PROPERTY(int numMaxShadowCasters READ numMaxShadowCasters )
//	Q_PROPERTY(float invNumMaxShadowCasters READ invNumMaxShadowCasters )
//
//	Q_PROPERTY(int numMaxInstancesRenderable READ numMaxInstancesRenderable )
//	Q_PROPERTY(float invNumMaxInstancesRenderable READ invNumMaxInstancesRenderable )
//
//	Q_PROPERTY(ShaderFeatureEnumQTClass::TextureType GBufferType READ GBufferType )
//
//	Q_PROPERTY(int numMultiSamples READ numMultiSamples )
//	Q_PROPERTY(float invNumMultiSamples READ invNumMultiSamples )
//
//	//Q_PROPERTY(ShaderFeaturesLocal mShaderFeaturesLocal READ mShaderFeaturesLocal )
//	//Q_PROPERTY(ShaderFeaturesGlobal mShaderFeaturesGlobal READ mShaderFeaturesGlobal )
//
//	Flewnit::ShaderFeaturesLocal mShaderFeaturesLocal;
//	Flewnit::ShaderFeaturesGlobal mShaderFeaturesGlobal;
//
//public:
//
//
//
//	GrantleeShaderFeaturesContext(const Flewnit::ShaderFeaturesLocal& sfl, const Flewnit::ShaderFeaturesGlobal& sfg)
//	: mShaderFeaturesLocal(sfl), mShaderFeaturesGlobal(sfg)
//	{}
//	virtual ~GrantleeShaderFeaturesContext(){}
//
//	ShaderFeatureEnumQTClass::RenderingTechnique renderingTechnique(){return ShaderFeatureEnumQTClass::RenderingTechnique(mShaderFeaturesLocal.renderingTechnique);
//	}
//	ShaderFeatureEnumQTClass::TextureType renderTargetTextureType()const{return ShaderFeatureEnumQTClass::TextureType(mShaderFeaturesLocal.renderTargetTextureType);}
//	ShaderFeatureEnumQTClass::VisualMaterialType visualMaterialType()const{return ShaderFeatureEnumQTClass::VisualMaterialType( mShaderFeaturesLocal.visualMaterialType);}
//	ShaderFeatureEnumQTClass::ShadingFeatures shadingFeatures()const{return ShaderFeatureEnumQTClass::ShadingFeatures (mShaderFeaturesLocal.shadingFeatures);}
//	bool instancedRendering()const{return mShaderFeaturesLocal.instancedRendering;}
//
//	ShaderFeatureEnumQTClass::LightSourcesLightingFeature lightSourcesLightingFeature()const{return  ShaderFeatureEnumQTClass::LightSourcesLightingFeature(mShaderFeaturesGlobal.lightSourcesLightingFeature);}
//	ShaderFeatureEnumQTClass::LightSourcesShadowFeature lightSourcesShadowFeature()const{return ShaderFeatureEnumQTClass::LightSourcesShadowFeature (mShaderFeaturesGlobal.lightSourcesShadowFeature);}
//	ShaderFeatureEnumQTClass::ShadowTechnique shadowTechnique()const{return ShaderFeatureEnumQTClass::ShadowTechnique (mShaderFeaturesGlobal.shadowTechnique);}
//	int numMaxLightSources()const{return mShaderFeaturesGlobal.numMaxLightSources;}
//	int numMaxShadowCasters()const{return mShaderFeaturesGlobal.numMaxShadowCasters;}
//	int numMaxInstancesRenderable()const{return mShaderFeaturesGlobal.numMaxInstancesRenderable;}
//	ShaderFeatureEnumQTClass::TextureType GBufferType()const{return ShaderFeatureEnumQTClass::TextureType(mShaderFeaturesGlobal.GBufferType);}
//	int numMultiSamples()const{return mShaderFeaturesGlobal.numMultiSamples;}
//
//	float invNumMaxLightSources()const{return 1.0f/mShaderFeaturesGlobal.numMaxLightSources;}
//	float invNumMaxShadowCasters()const{return 1.0f/mShaderFeaturesGlobal.numMaxShadowCasters;}
//	float invNumMaxInstancesRenderable()const{return 1.0f/mShaderFeaturesGlobal.numMaxInstancesRenderable;}
//	float invNumMultiSamples()const{return 1.0f/mShaderFeaturesGlobal.numMultiSamples;}
//};


//Q_DECLARE_METATYPE(GrantleeShaderFeaturesContext*)
