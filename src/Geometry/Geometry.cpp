/*
 * Geometry.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "Geometry.h"

#include "Buffer/BufferInterface.h"
#include "Simulator/SimulationResourceManager.h"

namespace Flewnit
{

Geometry::Geometry( String name, GeometryRepresentation geoRep)
:
		SimulationObject( name, GENERIC_SIM_DOMAIN),
		mGeometryRepresentation(geoRep)
{
	SimulationResourceManager::getInstance().registerGeometry(this);
}

Geometry::~Geometry()
{
	//nothing to do
}





}
