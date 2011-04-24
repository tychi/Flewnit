/*
 * SimulationPipelineStage.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "SimulationObject.h"
#include "Scene/SceneNode.h"

#include  "SimulatorMetaInfo.h"

namespace Flewnit
{

class SimulationPipelineStage
: public SimulationObject,
  public SceneNodeVisitor
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	SimulationPipelineStage( String name, SimulationDomain sd, ConfigStructNode* simConfigNode);
	virtual ~SimulationPipelineStage();

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual bool initStage()throw(SimulatorException) = 0;
	virtual bool validateStage()throw(SimulatorException) = 0;

	virtual void visitSceneNode(SceneNode* node){}


	//acessor to the different buffers, if subsequent stages need them
	//returns NULL if Buffer with specified name does not exist
	//none-pure virtual for special treatment in some stages
	virtual BufferInterface* getRenderingResult(BufferSemantics what);

	//non-pure virtual, as not every simulation stage needs this functionality
//	virtual bool validateSimulationStepResults(){return true;}
//	virtual bool profileAndOptimizeSettings(){return true;}
//	virtual bool profilePerformance(){return true;}

protected:
	ConfigStructNode* mSimConfigNode;

	std::map<BufferSemantics,BufferInterface*> mRenderingResults;

};

}

