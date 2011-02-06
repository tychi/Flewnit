/*
 * Camera.cpp
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#include "Camera.h"

#include "UserInterface/WindowManager/WindowManager.h"

#include "Common/CL_GL_Common.h"


namespace Flewnit {

Camera::Camera(String name, const AmendedTransform& localTransform, float fieldOfView_Angles)
: SceneNode(name, CAMERA_NODE, localTransform)
{
	perspective(
			fieldOfView_Angles,
			static_cast<float>(WindowManager::getInstance().getWindowResolution().x) /
			static_cast<float>(WindowManager::getInstance().getWindowResolution().y)
	);

}

Camera::~Camera()
{
	// TODO Auto-generated destructor stub
}


const Matrix4x4& Camera::perspective(	float fieldOfView_Angles,
								float aspectRatioXtoY,
								float nearClipPlane,
								float farClipPlane	)
{
	mProjectionMatrix =  glm::gtc::matrix_projection::perspective(
			fieldOfView_Angles, aspectRatioXtoY, nearClipPlane, farClipPlane );

	return mProjectionMatrix;
}

//needed for stuff like static mesh voxelization per rasterization;
//then, the params should be (0,voxelsPerDimension,0,voxelsPerDimension,0,voxelsPerDimension)
//so that every integral pixel coord corresponds to one voxel;
const Matrix4x4& Camera::ortho	(float left,
						float right,
						float bottom,
						float top,
						float nearClipPlane,
						float farClipPlane
						)
{
	mProjectionMatrix = glm::gtc::matrix_projection::ortho(left,right,bottom,top,nearClipPlane,farClipPlane);
	return mProjectionMatrix;
}


const Matrix4x4& Camera::getProjectionMatrix()const
{
	return mProjectionMatrix;
}

void Camera::setGLViewPort(const Vector2Di& lowerLeftPos, const Vector2Di& extends )
{
	glViewport(lowerLeftPos.x,lowerLeftPos.y,extends.x,extends.y);
}


}

