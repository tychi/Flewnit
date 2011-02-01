
/*
 * AmendedTransform.h
 *
 *  Created on: Jan 31, 2011
 *      Author: tychi
 *
 *  Class storing restrictive transformation features; This shall help to omit situations like screwed up
 *  matrices, especially when deriving lookAt()-matrices from arbitrary scene node transformations;
 *  In this frame work, only homogeneous scaling (scalar * 3x3-identity matrix), rotation and translation
 *  is allowed for scene node transformations; no skew or inhomogeneous skaling allowed;
 */

#pragma once

#include "Common/Math.h"

namespace Flewnit {

class AmendedTransform
{

public:
	AmendedTransform(
			const Vector3D& position = Vector3D(0.0f,0.0f,0.0f),
			//(0,0,-1) is assumed as initial orientation, extress any deviation in euler angles(radians)
			const Vector3D& direction = Vector3D(0.0f,.0f,-1.0f),
			const Vector3D& upVector = Vector3D(0.0f,1.0f,0.0f),
			float scale = 1.0f);

	AmendedTransform(const AmendedTransform& rhs);


	virtual ~AmendedTransform();


	void setPosition(const Vector3D& pos);
	void setDirection(const Vector3D& dir);
	void setUpVector(const Vector3D& up);
	void setScale(float amount);


	//unscaled, i.e. orthonormal rotation matrix:
	Matrix3x3 getRotationMatrix()const;
	//inverse of translationMat*RotationMat;
	Matrix4x4 getLookAtMatrix()const;
	//normal matrix: rotation matrix, scaled by 1/scale
	Matrix3x3 getNormalMatrix()const;
	//accum: translationMatrix* Mat4(normalMatrix)* scaleMatrix;
	Matrix4x4 getTotalTransform()const;

	AmendedTransform getInverse()const;

	AmendedTransform operator*(const AmendedTransform& rhs)const;
	const AmendedTransform& operator*=(const AmendedTransform& rhs);
	const AmendedTransform& operator=(const AmendedTransform& rhs);

	inline const Vector3D& getPosition()const{return mPosition;}
	inline const Vector3D& getDirection()const{return mDirection;}
	inline const Vector3D& getUpVector()const{return mUpVector;}
	inline float getScale()const{return mScale;}

protected:
	Matrix4x4 mAccumTranslationRotationScaleMatrix;

	Vector3D mPosition;
	Vector3D mDirection;
	Vector3D mUpVector;
	float mScale;

	void setup();

	//convenience functions:
	Matrix4x4 getScaleMatrix()const;
	Matrix4x4 getInverseScaleMatrix()const;

	//AmendedTransform constructor only for the scene loader;
	//"per hand" scene nodes shall be restricted to pos/dir/scale transforms,
	//so that the user cant f*** up the transformation matrix TOO much ;);
	//this constructor will vaklidate the passed transformation matrix nevertheless;
	friend class Loader;
	//parent is set by addScenenode-function automatically
	AmendedTransform(const Matrix4x4& transform);

};

//AmendedTransform operator*(const AmendedTransform& lhs,const AmendedTransform& rhs);

}


