/*
 * MPP.h
 *
 *  Created on: Jan 27, 2011
 *      Author: tychi
 *
 *  The "Massively Parallel Program" class, an abstract base class for Shaders and OpenCL kernels
 */

#pragma once

#include "Simulator/SimulationObject.h"


//forwards for the grantle string template engine:
class QString;
class QVariant;
template <class Key, class T>class QHash;
typedef QHash<QString, QVariant> QVariantHash;
typedef QVariantHash TemplateContextMap;
namespace Grantlee
{
	class Engine;
}



namespace Flewnit
{

class MPP
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	MPP(String name, SimulationDomain sd);
	virtual ~MPP();
	virtual void build()=0;
protected:
	virtual void setupTemplateContext(TemplateContextMap& contextMap)=0;
	virtual void validate()throw(SimulatorException)=0;
	//for later debugging of the final code of a stage:
	void writeToDisk(String sourceCode, Path where);
};

}
