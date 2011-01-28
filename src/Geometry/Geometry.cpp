/*
 * Geometry.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "Geometry.h"

namespace Flewnit
{

Geometry::Geometry( String name, GeometryRepresentation geoRep)
:
		SimulationObject( name, GENERIC_SIM_DOMAIN),
		mGeometryRepresentation(geoRep)
{
//	for(int runner = 0; runner < __NUM_SIM_DOMAINS__; runner++)
//	{
//		mOwningSubObjects[runner]=0;
//	}

}

Geometry::~Geometry()
{
	// TODO Auto-generated destructor stub
}

}
