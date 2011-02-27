/*
 * Camera.cpp
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#include "Camera.h"

#include "UserInterface/WindowManager/WindowManager.h"

#include "Common/CL_GL_Common.h"
#include "Util/Log/Log.h"


namespace Flewnit {

Camera::Camera(String name, const AmendedTransform& localTransform,
		float verticalFOVAngle,
		float nearClipPlane,
		float farClipPlane)
:
		SceneNode(name, CAMERA_NODE, localTransform),
		mVerticalFOVAngle(verticalFOVAngle),
		mNearClipPlane(nearClipPlane), mFarClipPlane(farClipPlane)
{
	mAspectRatioXtoY =
			static_cast<float>(WindowManager::getInstance().getWindowResolution().x) /
			static_cast<float>(WindowManager::getInstance().getWindowResolution().y);

	perspective( mVerticalFOVAngle, mAspectRatioXtoY, mNearClipPlane, mFarClipPlane);

//	LOG<<DEBUG_LOG_LEVEL<<"Camera Transformation Matrix:"<<getGlobalTransform().getTotalTransform()<<";\n";
//	LOG<<DEBUG_LOG_LEVEL<<"Camera Lookat Matrix:"<<getGlobalTransform().getLookAtMatrix()<<";\n";
//
//	LOG<<DEBUG_LOG_LEVEL<<"Camera Projection Matrix:"<<getProjectionMatrix()<<";\n";

}

Camera::~Camera()
{
	// TODO Auto-generated destructor stub
}


Matrix4x4 Camera::getViewMatrix()
{
	return getGlobalTransform().getLookAtMatrix();
}

const Matrix4x4& Camera::perspective(	float verticalFOVAngle,
								float aspectRatioXtoY,
								float nearClipPlane,
								float farClipPlane	)
{
	mVerticalFOVAngle=verticalFOVAngle;
	mAspectRatioXtoY = aspectRatioXtoY;
	mNearClipPlane=nearClipPlane;
	mFarClipPlane=farClipPlane;

	mProjectionMatrix =  glm::gtc::matrix_projection::perspective(
			verticalFOVAngle, aspectRatioXtoY, nearClipPlane, farClipPlane );

	return mProjectionMatrix;
}

//needed for stuff like static mesh voxelization per rasterization;
//then, the params should be (-voxelsPerDimension/2,+voxelsPerDimension/2,-voxelsPerDimension/2,+voxelsPerDimension/2,0,voxelsPerDimension)
//so that every integral pixel coord corresponds to one voxel;
const Matrix4x4& Camera::ortho	(float left,
						float right,
						float bottom,
						float top,
						float nearClipPlane,
						float farClipPlane
						)
{
	mNearClipPlane=nearClipPlane;
	mFarClipPlane=farClipPlane;

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

