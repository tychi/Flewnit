/*
 * VoxelGrid
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 */

#include "VoxelGrid.h"

namespace Flewnit {

VoxelGrid::VoxelGrid(String name, const Vector3D& minPos, const Vector3D& maxPos, int voxelsPerDimension)
: Geometry(VOXEL_GRID, name), mMinPos(minPos),mMaxPos(maxPos), mVoxelsPerDimension(voxelsPerDimension)
{
	// TODO Auto-generated constructor stub

}

VoxelGrid::~VoxelGrid() {
	// TODO Auto-generated destructor stub
}

void VoxelGrid::render()
{

}

}
