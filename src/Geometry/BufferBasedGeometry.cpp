/*
 * BufferBasedGeometry.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "BufferBasedGeometry.h"

#include "Buffer/BufferInterface.h"
#include "Simulator/SimulationResourceManager.h"


namespace Flewnit
{

BufferBasedGeometry::BufferBasedGeometry(String name, GeometryRepresentation geoRep)
: Geometry(name, geoRep)
{
	for(int runner = 0; runner < __NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__; runner++)
	{
		mAttributeBuffers[runner]=0;
	}
}

BufferBasedGeometry::~BufferBasedGeometry()
{
	//nothing to delete, everything managed by sim resource manager;
}


void BufferBasedGeometry::setAttributeBuffer(BufferInterface* buffi, BufferSemantics bs) throw(BufferException)
{
	if(buffi)
	{
		mAttributeBuffers[buffi->getBufferInfo().bufferSemantics] = buffi;
	}
	else
	{
		mAttributeBuffers[bs] = 0;
	}
	validateBufferIntegrity();
}

//can return NULL pointer if buffer is not registered for the given semantics
BufferInterface* BufferBasedGeometry::getAttributeBuffer(BufferSemantics bs)
{
	return mAttributeBuffers[bs];
}


}
