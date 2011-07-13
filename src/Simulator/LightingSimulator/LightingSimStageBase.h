
/*
 * LightingSimStageBase.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"
#include "Simulator/SimulatorMetaInfo.h"

#include "Material/VisualMaterial.h"

#include <typeinfo>


namespace Flewnit {


class LightingSimStageBase
: public SimulationPipelineStage
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	LightingSimStageBase(
			String name,
			RenderingTechnique renderingTechnique,
			const VisualMaterialFlags& materialFlagMask,
			ConfigStructNode* simConfigNode);
	virtual ~LightingSimStageBase();

	inline RenderingTechnique getRenderingTechnique()const {return mRenderingTechnique;}

	//override to pass stuff from mUsedRenderTarget instead of mRenderingResults
	//WARNING: just returns owned color textures; if you want to expose the depth texture
	//uese as depth buffer or non-owned color textures, you have to override again
	virtual BufferInterface* getRenderingResult(BufferSemantics what);

	//iterates over the scenegraph and issues material activations and geometry draw calls on materials
	//compatible to the specific rendering technique;
	//uses VisualMaterialFlags::areCompatibleTo(materialFlagMask) to decide which SubObject in the SceneGraph
	//to draw and which not
	//void drawAllCompliantGeometry(const VisualMaterialFlags& materialFlagMask);

	//usually, a scene node is tested if it is a world object an if true, its "visual domain" subobjects are
	//checked for compatibility with the current material type mask, and if the check passed,
	//their material is activated and their geometry drawn;
	virtual void visitSceneNode(SceneNode* node);
	//determine if a given material is campatible to the current Liging Simulation Stage
	virtual bool checkCompatibility(VisualMaterial* visMat)=0;


	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual bool initStage()throw(SimulatorException) = 0;
	virtual bool validateStage()throw(SimulatorException) = 0;

	//NULL if rendering to screen;
	inline RenderTarget* getUsedRenderTarget()const{return mUsedRenderTarget;}
	inline const VisualMaterialFlags& getMaterialFlagMask()const{return mMaterialFlagMask;}
	inline void setRenderToScreen(bool val){mRenderToScreen=val;}

protected:
	//init to 0
	RenderTarget* mUsedRenderTarget;

	bool mRenderToScreen;

private:

	//NULL if rendering to screen;
	RenderingTechnique mRenderingTechnique;
	VisualMaterialFlags mMaterialFlagMask;

};



}

