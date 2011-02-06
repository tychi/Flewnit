
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
			Path shaderCodeDirectory = FLEWNIT_DEFAULT_SHADER_SOURCES_PATH);
	virtual ~ShaderManager();

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


	//iterates all visual materials and assigns them shaders fitting the current scenario;
	//the shaders might need to be generated first (done automatically);
	//the attachment status of the rendertaget is validated to fit the material's needs and the shader's
	//binding status.
	//throws exception if there is something incompatible;
	//void setRenderingScenario(RenderingTechnique rendTech,TextureType renderTargetTextureType, RenderTarget* rt)throw(SimulatorException);
	void setRenderingScenario(LightingSimStageBase* lightingStage)throw(SimulatorException);

	//
	//Shader* getShaderForCurrentRenderingScenario(ShadingFeatures shadingFeatures,bool forInstancedRendering);


private:

	Path mShaderCodeDirectory;

	List<VisualMaterial*> mRegisteredVisualMaterials;

	ShaderFeaturesGlobal mGlobalShaderFeatures;

	RenderingTechnique mRenderingTechnique;
	//renderTargetTextureType delegates creation and/or configuration of a geometry shader,
	//defining layers for cubemap rendering or general layered rendering
	TextureType mRenderTargetTextureType;

	//ShaderFeaturesLocal mCurrenLocalShaderFeatures;
	bool mIsInitializedGuard;

	boost::unordered_map<ShaderFeaturesLocal, Shader*> mShaderMap;


//	std::un
//	Map<
//		RenderingTechnique,
//		Map<
//			TextureType,
//			Map<
//				ShadingFeature,
//				Map<
//					bool,
//					Shader*
//				>
//			>
//		>
//	> mGeneratedShaders;

	friend VisualMaterial::VisualMaterial(String,VisualMaterialType,ShadingFeatures,const Map<BufferSemantics, Texture*>&,
			bool,bool,bool,bool,bool,bool); //registerToShaderManager();
	friend VisualMaterial::~VisualMaterial();//unregisterFromShaderManager();
	void registerVisualMaterial(VisualMaterial* mat);
	void unregisterVisualMaterial(VisualMaterial* mat);
};

}

