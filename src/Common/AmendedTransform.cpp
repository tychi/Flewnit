/*
 * AmendedTransform.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: tychi
 */

#include "AmendedTransform.h"



namespace Flewnit {

AmendedTransform::	AmendedTransform(
		const Vector3D& position,
		//(0,0,-1) is assumed as initial orientation, extress any deviation in euler angles(radians)
		const Vector3D& direction,
		const Vector3D& upVector,
		float scale)
:	 mPosition(position), mDirection(direction), mUpVector(upVector), mScale(scale)
{
	setup();
}

AmendedTransform::AmendedTransform(const AmendedTransform& rhs)
:mPosition(rhs.mPosition), mDirection(rhs.mDirection), mUpVector(rhs.mUpVector), mScale(rhs.mScale)
{
	setup();
}

void AmendedTransform::setup()
{
	assert(glm::length(mDirection) > 0.01f);
	assert(glm::length(mUpVector) > 0.01f);
	assert(mScale > 0.0f);

	glm::normalize(mDirection);
	glm::normalize(mUpVector);

	//Haxx: construct rotate/translate matrix, via the inverse ViewMatrix:
	Matrix4x4 lookAtMatrix = glm::gtx::transform2::lookAt(
			mPosition,mPosition+mDirection,mUpVector);
	Matrix4x4 translationRotationMatrix = lookAtMatrix._inverse();

	mAccumTranslationRotationScaleMatrix =
			translationRotationMatrix *
			//just the rotational component shall be scaled, not the translational
			getScaleMatrix();

	//Matrix3x3 rotationMatrix= Matrix3x3(lookAtMatrix)._inverse();
}


AmendedTransform::AmendedTransform(const Matrix4x4& transform)
{
	mPosition = Vector3D(transform[3]);
	assert("lowerRightComponent is one" && (std::fabs(transform[3][3]-1.0f) <0.01f ));
}



AmendedTransform::~AmendedTransform()
{
	// TODO Auto-generated destructor stub
}


Matrix3x3 AmendedTransform::getRotationMatrix()const
{
	return Matrix3x3(mAccumTranslationRotationScaleMatrix) * (1.0f/mScale);
}
//inverse of translationMat*RotationMat;
Matrix4x4 AmendedTransform::getLookAtMatrix()const
{
	return mAccumTranslationRotationScaleMatrix._inverse() * getInverseScaleMatrix();
}
//normal matrix: rotation matrix, scaled by 1/scale
Matrix3x3 AmendedTransform::getNormalMatrix()const
{
	return getRotationMatrix()*  (1.0f/mScale) ;
}
//accum: translationMatrix* Mat4(normalMatrix)* scaleMatrix;
Matrix4x4 AmendedTransform::getTotalTransform()const
{
	return mAccumTranslationRotationScaleMatrix;
}


//convenience functions:
Matrix4x4 AmendedTransform::getScaleMatrix()const
{
	return Matrix4x4(Matrix3x3(mScale));
}
Matrix4x4 AmendedTransform::getInverseScaleMatrix()const
{
	return Matrix4x4(Matrix3x3(1.0f/mScale));
}

void AmendedTransform::setPosition(const Vector3D& pos)
{
	mPosition = pos;
	setup();
}
void AmendedTransform::setDirection(const Vector3D& dir)
{
	mDirection = dir;
	setup();
}
void AmendedTransform::setUpVector(const Vector3D& up)
{
	mUpVector = up;
	setup();
}
void AmendedTransform::setScale(float amount)
{
	mScale = amount;
	setup();
}

AmendedTransform AmendedTransform::operator*(const AmendedTransform& rhs)const
{
	return AmendedTransform(
			//to get the new position,  multiplicate the rhs' old one with the product
			//of lhs * rhs;
			Vector3D( mAccumTranslationRotationScaleMatrix * Vector4D(rhs.mPosition,1.0) ),
			getRotationMatrix() * rhs.mDirection,
			getRotationMatrix() * rhs.mUpVector,
			mScale * rhs.mScale
	);
}

const AmendedTransform& AmendedTransform::operator*=(const AmendedTransform& rhs)
{
	*this = AmendedTransform(
			Vector3D( mAccumTranslationRotationScaleMatrix * Vector4D(rhs.mPosition,1.0) ),
			getRotationMatrix() * rhs.mDirection,
			getRotationMatrix() * rhs.mUpVector,
			mScale * rhs.mScale
	);
	return *this;
}

const AmendedTransform& AmendedTransform::operator=(const AmendedTransform& rhs)
{
	mPosition=rhs.mPosition;
	mDirection=rhs.mDirection;
	mUpVector=rhs.mUpVector;
	mScale=rhs.mScale;

	setup();

	return *this;
}

AmendedTransform AmendedTransform::getInverse()const
{
	return AmendedTransform(
			//(TRS)^-1 == S^-1 * R^-1 * T^-1;
			//this gives a matrix where the fourtg column is (inverse rotated negative original value scaled by 1/originalScale)
			getRotationMatrix()._inverse()*	(mPosition * (-1.0f/ mScale )),
		//invers rotate dir and up
		getRotationMatrix()._inverse() * mDirection,
		getRotationMatrix()._inverse() * mUpVector,
		1.0f / mScale
	);
}

}


