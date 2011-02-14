/*
 * Material.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "Material.h"
#include "Simulator/SimulationResourceManager.h"

namespace Flewnit
{

Material::Material(String name, SimulationDomain sd)
:SimulationObject(name, sd)
{
	// TODO Auto-generated constructor stub
	SimulationResourceManager::getInstance().registerMaterial(this);

}

Material::~Material()
{
	// TODO Auto-generated destructor stub
}

}
