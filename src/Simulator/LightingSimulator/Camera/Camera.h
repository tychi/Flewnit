
/*
 * Camera.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#include "Scene/SceneNode.h"

#pragma once

namespace Flewnit {

class Camera
: public SceneNode
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//constructor sets up the camera to a look at the -z axis from (0,0,0),
	//at 45° FOV, 1/1 aspect ratio perspective;
	//grabs the viewport from the WindowManager Singleton window size;
	//setup the relevtant matrices per hand if you want different params;
	Camera(String name, const AmendedTransform& localTransform = AmendedTransform());

	virtual ~Camera();



	const Matrix4x4& perspective(	float fieldOfView_Angles,
									float aspectRatioXtoY,
									float nearClipPlane = 0.1f,
									float farClipPlane = 100.0f	);
	//glm::gtc::matrix_projection::perspective()

	//needed for stuff like static mesh voxelization per rasterization;
	//then, the params should be (0,voxelsPerDimension,0,voxelsPerDimension,0,voxelsPerDimension)
	//so that every integral pixel coord corresponds to one voxel;
	const Matrix4x4& ortho	(float left,
							float right,
							float bottom,
							float top,
							float nearClipPlane,
							float farClipPlane
							);
	//glm::gtc::matrix_projection::ortho()

	const Matrix4x4& getProjectionMatrix()const;

	void setGLViewPort(const Vector2Di& lowerLeftPos, const Vector2Di& extends );


private:

	Matrix4x4 mProjectionMatrix;
};

}

