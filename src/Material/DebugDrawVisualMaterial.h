/*
 * DebugDrawVisualMaterial.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once

#include "Material/VisualMaterial.h"

#include "Geometry/Geometry.h"

namespace Flewnit
{

class DebugDrawVisualMaterial
	: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	DebugDrawVisualMaterial( String name, const Vector4D& debugDrawColor,
			bool isInstanced,
			//must be VERTEX_BASED_POINT_CLOUD or VERTEX_BASED_LINES or VERTEX_BASED_TRIANGLES
			GeometryRepresentation geomRepToDraw = VERTEX_BASED_LINES  );

	virtual ~DebugDrawVisualMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);


protected:

	GLint  mOldGlPolygonModes[2];
	GLboolean mCullingIsEnabled;
	GLuint mGLPolygonMode;

};

}
