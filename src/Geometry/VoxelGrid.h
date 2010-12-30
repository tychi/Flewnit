/*
 * VoxelGrid.h
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 *
 * Class to represent grids for smoke simulation or uniform grids, being used as acceleration structure;
 *
 * EDIT: Stub class; may not be needed or will be heavily modded;
 */

#pragma once

#include "Geometry.h"

#include "Common/Math.h"


namespace Flewnit
{

class VoxelGrid
:public Geometry
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	Vector3D mMinPos;
	Vector3D mMaxPos;
	int mVoxelsPerDimension;

	//Map<String, Buffer*> mVoxelBuffers;

public:
	VoxelGrid(String name, const Vector3D& minPos, const Vector3D& maxPos, int voxelsPerDimension);
	virtual ~VoxelGrid();


	virtual void render();

};

}
