
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

	//iterates over the scenegraph and issues material activations and geometry draw calls on materials
	//compatible to the specific rendering technique;
	//uses VisualMaterialFlags::areCompatibleTo(materialFlagMask) to decide which SubObject in the SceneGraph
	//to draw and which not
	//void drawAllCompliantGeometry(const VisualMaterialFlags& materialFlagMask);

	//NON-pure virtual here, because many lighting stages don't need any special implementation;
	//usually, a scene node is tested if it is a world object an if true, its "visual domain" subobjects are
	//checked for compatibility with the current material type mask, and if the check passed,
	//their material is activated and their geometry drawn;
	virtual void visitSceneNode(SceneNode* node);

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual void initStage()throw(SimulatorException) = 0;
	virtual void validateStage()throw(SimulatorException) = 0;

	inline RenderTarget* getUsedRenderTarget()const{return mUsedRenderTarget;}
	inline const VisualMaterialFlags& getMaterialFlagMask()const{return mMaterialFlagMask;}

protected:
	//init to 0
	RenderTarget* mUsedRenderTarget;

private:

	//NULL if rendering to screen;
	RenderingTechnique mRenderingTechnique;
	VisualMaterialFlags mMaterialFlagMask;

};



}

