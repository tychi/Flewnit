/*
 * VoxelGridGeometry.h
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 *
 * Classes to represent grids for smoke simulation or uniform grids, being used as acceleration structure;
 *
 */

#pragma once

#include "Geometry.h"

#include "Common/Math.h"


namespace Flewnit
{


//abstract base class for all voxel stuff:
class 	VoxelGridGeometry
:public BufferBasedGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	Vector3D mCentrePosition;
	float mEdgeLenght;
	//must be power of two
	int mVoxelsPerDimension;

	//Map<String, Buffer*> mVoxelBuffers;

public:
	VoxelGridGeometry(String name, const Vector3D& centrePosition, float edgeLength, int voxelsPerDimension);
	virtual ~VoxelGridGeometry();


	virtual void draw(SimulationPipelineStage* currentStage,
				GeometryRepresentation desiredGeomRep) = 0;

};


//As In OpenCL (in contrast to CUDA), it is not possible to "reinterpret cast" a Generic Buffer
//to a Texture or Vice versa, we have to implement two seperate classes :(.
//The 3D Texture Voxel representation is great for gas Simulation AND its rendering;
class		Texture3DVoxelGridGeometry
: public VoxelGridGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	Texture3DVoxelGridGeometry(String name, const Vector3D& centrePosition, float edgeLength, int voxelsPerDimension);
	virtual ~Texture3DVoxelGridGeometry();

	virtual void draw(SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject,
				GeometryRepresentation desiredGeomRep);
private:
	//compare buffers for sizees, types, number of elements etc;
	virtual void validateBufferIntegrity()throw(BufferException);
};

/*
The Generic Buffer is needed as a storage for a Uniform Grid acceleration structure
storing
	0.: The Amount of the containing particles
	1.: index to the first of the containing Particles
	2.: The Amount of the containing static triangles
	3.: index to the first of the containing static Triangles

It COULD be realized a a 3D Integer Texture, bu I don't like this concept (yet).

Drawing this Geometry type has only sense for debug purposes
*/
class GenericBufferVoxelGridGeometry
: public VoxelGridGeometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	GenericBufferVoxelGridGeometry(String name, const Vector3D& centrePosition, float edgeLength, int voxelsPerDimension);
	virtual ~GenericBufferVoxelGridGeometry();

	//only debugdraw of certain values; vonfigurable via shader; (e.g. point rendering of certain buffer
	//values bound to a VBO, positions are implicitly derived form glPrimitiveID and so on ;)) )
	virtual void draw(SimulationPipelineStage* currentStage,
				GeometryRepresentation desiredGeomRep);
private:
	//compare buffers for sizees, types, number of elements etc;
	virtual void validateBufferIntegrity()throw(BufferException);
};
//the single purpose for this Geometry type is to realize debug drawing of the Uniform Grid
//via instanced drawing of only two vertices;
class		UniformGridImplicitVoxelGridGeometry;



}
