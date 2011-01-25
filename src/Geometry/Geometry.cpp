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
	// TODO Auto-generated constructor stub

}

Geometry::~Geometry()
{
	// TODO Auto-generated destructor stub
}

}
