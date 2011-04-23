/*
 * DebugDrawVisualMaterial.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#include "DebugDrawVisualMaterial.h"
#include "Simulator/ParallelComputeManager.h"
#include "MPP/Shader/Shader.h"

namespace Flewnit {

//DebugDrawVisualMaterial::DebugDrawVisualMaterial() {
//	// TODO Auto-generated constructor stub
//
//}
//
//DebugDrawVisualMaterial::~DebugDrawVisualMaterial() {
//	// TODO Auto-generated destructor stub
//}




DebugDrawVisualMaterial::DebugDrawVisualMaterial( String name, const Vector4D& debugDrawColor,
		bool isInstanced,
		//must be VERTEX_BASED_POINT_CLOUD or VERTEX_BASED_LINES or VERTEX_BASED_TRIANGLES
		GeometryRepresentation geomRepToDraw  )
: VisualMaterial(name, isInstanced, debugDrawColor)
{
	switch (geomRepToDraw) {
		case VERTEX_BASED_POINT_CLOUD:
			mGLPolygonMode = GL_POINT;
			break;
		case VERTEX_BASED_LINES:
			mGLPolygonMode = GL_LINE;
			break;
		case VERTEX_BASED_TRIANGLES:
			mGLPolygonMode = GL_FILL;
			break;
		default:
			assert(0 && "DebugDrawVisualMaterial: invalid polygon draw type");
			break;
	}
}

DebugDrawVisualMaterial::~DebugDrawVisualMaterial()
{}

bool DebugDrawVisualMaterial::operator==(const Material& rhs) const
{
	const DebugDrawVisualMaterial* ddvm = dynamic_cast<const DebugDrawVisualMaterial*>(&rhs);

	return (
		ddvm
		&&
		( glm::length( getColor() - ddvm->getColor() ) < 0.001f )
		&&
		mGLPolygonMode == ddvm->mGLPolygonMode
	);
}

void DebugDrawVisualMaterial::activate(
		SimulationPipelineStage* currentStage,
		SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//draw debug stuff in desired polygon mode, save old mode!
	GUARD( glGetIntegerv(GL_POLYGON_MODE, mOldGlPolygonModes) );
	GUARD( glPolygonMode(GL_FRONT_AND_BACK, mGLPolygonMode));
	//GUARD(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	GUARD( mCullingIsEnabled = glIsEnabled(GL_CULL_FACE) );
	if(mCullingIsEnabled)
	{
		glDisable(GL_CULL_FACE);
	}

	getCurrentlyUsedShader()->use(currentUsingSuboject);

}

void DebugDrawVisualMaterial::deactivate(SimulationPipelineStage* currentStage,
		SubObject* currentUsingSuboject) throw(SimulatorException)
{
	//retore old polygoin mode
	GUARD(glPolygonMode(GL_FRONT_AND_BACK, mOldGlPolygonModes[0]));

	if(mCullingIsEnabled)
	{
		glEnable(GL_CULL_FACE);
	}

	//GUARD(glPolygonMode(GL_FRONT, mOldGlPolygonModes[0]));
	//GUARD(glPolygonMode(GL_BACK, mOldGlPolygonModes[1]));
}

}
