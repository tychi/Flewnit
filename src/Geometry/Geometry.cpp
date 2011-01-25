/*
 * Geometry.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "Geometry.h"

namespace Flewnit
{

Geometry::Geometry( String name, SimulationDomain sd, GeometryRepresentation geoRep, SubObject* owningSO)
:
		SimulationObject( name, sd),
		mGeometryRepresentation(geoRep),
		mOwningSubObject(owningSO)
{
	for(SimulationDomain runner = SimulationDomain(0); runner < __NUM_SIM_DOMAINS__; runner++)
	{
		mOwningSubObjects[runner]=0;
	}

}

Geometry::~Geometry()
{
	// TODO Auto-generated destructor stub
}

}
