/*
 * ReorderParticleAttributesProgram.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once


#include "BasicCLProgram.h"


namespace Flewnit
{

class ReorderParticleAttributesProgram
	:public BasicCLProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	//ParticleSceneRepresentation* param needed for creation of the kernel arguments list,
	//not for template context, setup, hence
	ReorderParticleAttributesProgram(ParticleSceneRepresentation* partScene);

	virtual ~ReorderParticleAttributesProgram();

protected:

	//the most simple templat context: an empty one ;)
	virtual void setupTemplateContext(TemplateContextMap& contextMap){}

	//issue the createKernel() call for reorderAttributes.cl with initial argument list
	virtual void createKernels();

	ParticleSceneRepresentation* mParticleSceneRepresentation;

};



}
