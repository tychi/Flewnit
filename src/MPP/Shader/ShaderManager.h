
/*
 * ShaderManager.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once


#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"

#include "Material/VisualMaterial.h"

#include "Simulator/LightingSimulator/LightingSimStageBase.h"

#include <boost/unordered_map.hpp>


namespace Flewnit
{




class ShaderManager
:public Singleton<ShaderManager>,
 public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:

	ShaderManager(const ShaderFeaturesGlobal& globalShaderFeatures,
			ShadingFeatures supportedShadingFeatures,
			Path shaderCodeDirectory = FLEWNIT_DEFAULT_SHADER_SOURCES_PATH);
	virtual ~ShaderManager();

	inline const ShaderFeaturesGlobal& getGlobalShaderFeatures()const{return mGlobalShaderFeatures;}
	inline Path getShaderCodeDirectory()const{return mShaderCodeDirectory;}



	//iterates all visual materials and assigns them shaders fitting the current scenario;
	//the shaders might need to be generated first (done automatically);
	//the attachment status of the rendertaget is validated to fit the material's needs and the shader's
	//binding status.
	//throws exception if there is something incompatible;
	//void setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt)throw(SimulatorException);
	void setRenderingScenario(LightingSimStageBase* lightingStage)throw(SimulatorException);


	//needed for creation of/access to specialshaders like the primitive id-renderer:
	Shader* getShader(const ShaderFeaturesLocal& sfl);

	/*
	 * 	Shall return a shader where offsets of members in
	 *	lightsource buffer and shadowmap matrix buffer,
	 *	if the global shader features indicate their possible usage;
	 *
	 *	note: an instance transform uniform block is not necessary, as every instance manager
	 *	has its "own" shader to query from; only the other two uniform blocks have a "global" character
	 */
	Shader* getUniformBufferOffsetQueryShader(bool forShadowMapGeneration);

	bool currentRenderingScenarioPerformsLayeredRendering()const;
	//returns true if doing layered rendering or other stuff involving multiple view/projection cameras
	//which would currupt view space transformed data;
	//note: in this case pure viewspace-dependent stuff like AO calculatins can
	//only be done without costly per-pixel back-transformations if some conditions are satified
	bool shaderNeedsWorldSpaceTransform()const;

	bool currentRenderingScenarioNeedsGeometryShader()const;
	bool currentRenderingScenarioNeedsFragmentShader()const;

	//silently ignores returns if tess is not enabled by config or for technical reasons;


	void setEnableShadingFeatures(ShadingFeatures sfs, bool val);
	//void setEnableTesselation(bool val);


	bool shadingFeaturesAreEnabled(ShadingFeatures sfs)const;
	inline ShadingFeatures getEnabledShadingFeatures()const{return mEnabledShadingFeatures;}
	bool tesselationIsEnabled()const;


private:


	void assignShader(VisualMaterial* mat);
	Shader* generateShader(const ShaderFeaturesLocal& sfl);

	Path mShaderCodeDirectory;

	std::vector<VisualMaterial*> mRegisteredVisualMaterials;

	ShaderFeaturesGlobal mGlobalShaderFeatures;

	RenderingTechnique mCurrentRenderingTechnique;
	//renderTargetTextureType delegates creation and/or configuration of a geometry shader,
	//defining layers for cubemap rendering or general layered rendering
	TextureType mCurrentRenderTargetTextureType;

	//bool mTesselationIsEnabled;
	ShadingFeatures mEnabledShadingFeatures;

	//ShaderFeaturesLocal mCurrenLocalShaderFeatures;
	//bool mIsInitializedGuard;

	boost::unordered_map<ShaderFeaturesLocal, Shader*> mShaderMap;



	friend VisualMaterial::VisualMaterial(String,VisualMaterialType,ShadingFeatures,const std::map<BufferSemantics, Texture*>&,
			const VisualMaterialFlags&, float, float); //registerToShaderManager();
	friend VisualMaterial::~VisualMaterial();//unregisterFromShaderManager();
	void registerVisualMaterial(VisualMaterial* mat);
	void unregisterVisualMaterial(VisualMaterial* mat);
};

//TODO rewrite this documentation ;)
//get shader with help of a subset of the ShaderFeatures-structure;
//The missoing params are derived from the global state of the engine
//(LightSourceManager),  max number ofinstances and multisample from
//ResourceManager or global render target
//Not all combinations are valid, (e.g. G-buffer fill and deferred
//lighting on an cube map render target resp. texture); in this case,
//an exception will be thrown
//Usage:
// 		For LightingSimulationStages not responsible for actual shading
//		(shadow/depth/position image generation) :
//			if any instanced-rendering geometry exists, every stage
//			has to grab at least two shaders:
//			-	one for the non-instanced geometry and
//			-	one for the instanced;
//			These two shaders can handle all the Rendering of that stage,
//			as long as they check for material flags like shadow casting etc.
//
//		For shading lighting stages (G-Buffer fill including, because fragment
//		colors are calculated, even though not lighted):
//		A Pipeline should, before any rendering, determine its rendering scenarios
//		for the default geometry;
//		Then, every shading-material requests the generation of a shader with the
//		material-specific shading features for every rendering szenario..
//		(deferred shading is another (easier, at least in terms of material-shader-association)
//		story, as no different geometry and hence material is involved)
//
//			- default direct rendering:
//				one classical vert/frag shader sufficient for every used shadingFeature
//				permutation (i.e. normal/cube/decal mapping etc. in arbitrary combinations)
//			- G-Buffer fill:
//
//			- dynamic envmap - generation:


}

