/*
 * SceneRepresentation.h
 *
 *	Base class for all types of "Scene"; Because we want to be able to
 *	(at least theoretically, this is a concept study) simulate various
 *	phenomena, we may use several representations for (subsets of) the
 *	phenomena, set of world object, i.e. the virtual world";
 *
 *	Example: Classic OpenGL Lighting Simulation is done on a SceneGraph
 *	 representation, but fluid mechanics and (at least one taste of)
 *	 Rigid body mechanics are performed on a particle Representation, boosted
 *	 by a UniformGrid AccelerationStructure;
 *
 *
 * Note: One could design every concrete Scene representation as Singleton;
 * 		 Considering that one could have several nested simulation domains (e.g. a dungeon within
 * 		 a world), there might be need for several instances;
 *
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 *
 */


#pragma once

#include "Common/BasicObject.h"

namespace Flewnit
{


enum SceneRepresentationType
{
	SCENE_GRAPH_REPRESENTATION,	   //classical generic domain
	PARTICLE_SCENE_REPRESENTATION, //particle fluid and particleized rigid body stuff
	VOXEL_SCENE_REPRESENTATION,	   //grid based fluid and voxelized ridig body stuff
								   //not used in this thesis so far
	PRIMITIVE_SCENE_REPRESENTATION //classical rigid body stuff: compounds of
								   //sphere/box/capspule/convex hull etc..
							  	   //not used in this thesis so far
};


class SceneRepresentation
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


public:
	SceneRepresentation(SceneRepresentationType type);

	virtual ~SceneRepresentation();

	SceneRepresentationType getSceneRepresentationType()const{return mSceneRepresentationType;}

private:

	SceneRepresentationType mSceneRepresentationType;
};

}


