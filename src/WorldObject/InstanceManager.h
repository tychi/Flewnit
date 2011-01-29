
/*
 * InstanceManager.h
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#pragma once


#include "Simulator/SimulationObject.h"

#include "Simulator/SimulatorForwards.h"

#include "Common/CL_GL_Common.h"

namespace Flewnit
{

class InstanceManager
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	InstanceManager(String name, GLuint numMaxInstances,
			Material* associatedMaterial, Geometry* nonInstancedGeometryToDraw );

	virtual ~InstanceManager();


	void drawRegisteredInstances();

	//SubObject must own an InstacedGeometry;
	void registerInstanceForNextDrawing(SubObject* so);

private:
	String mName;

	Material* mAssociatedMaterial;

	//Uniform buffer containing the world transforms of the currently registered geometry instances
	Buffer* mModelMatricesUniformBuffer;
	//optional: if different material parameters are desired, so that every geo. instance
	//needs unique and reproducable identification in a shader, we ne a "map-buffer" holding the
	//"application logic-instance ID"; The dynamic registry of instances and the runtime-variable
	//length of the "instance buffer" for the sake  of stuff like dynamic instance adding/removal
	//and culling makes this necessary;
	//contains one GLuint per
	//Buffer* mInstanceIDUniformBuffer;

	GLuint  mMaxManagedInstances;

	GLuint mNumCurrentlyRegisteredInstancesForNextDrawing;

	//the "real" geometry which will be renderer
	Geometry* mNonInstancedGeometryToDraw;

};

}


