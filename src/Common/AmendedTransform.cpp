/*
 * AmendedTransform.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: tychi
 */

#include "AmendedTransform.h"

#include "Scene/SceneNode.h"
#include "Util/Log/Log.h"

namespace Flewnit {

AmendedTransform::	AmendedTransform(
		const Vector3D& position,
		//(0,0,-1) is assumed as initial orientation, extress any deviation in euler angles(radians)
		const Vector3D& direction,
		const Vector3D& upVector,
		float scale)
:	mOwningSceneNode(0), mIsGlobalTransform(true),
 	mPosition(position), mDirection(direction), mUpVector(upVector), mScale(scale)
{
	setup();
}

AmendedTransform::AmendedTransform(const AmendedTransform& rhs)
:mOwningSceneNode(0),mPosition(rhs.mPosition), mDirection(rhs.mDirection), mUpVector(rhs.mUpVector), mScale(rhs.mScale)
{
	setup();
}

void AmendedTransform::setup()
{
	assert(glm::length(mDirection) > 0.01f);
	assert(glm::length(mUpVector) > 0.01f);
	assert(mScale > 0.0f);

	mDirection = glm::normalize(mDirection);
	mUpVector = glm::normalize(mUpVector);

	//LOG<<DEBUG_LOG_LEVEL<<"mDirection: "<<Vector4D(mDirection,0.0f)<<";\n";

	assert("direction and upVector not parallel"
		&& ( std::fabs(glm::dot(mDirection, mUpVector)) < 0.99 ));

	//Haxx: construct rotate/translate matrix, via the inverse ViewMatrix:
	Matrix4x4 lookAtMatrix = glm::gtx::transform2::lookAt(
			mPosition,mPosition+mDirection,mUpVector);
	//LOG<<DEBUG_LOG_LEVEL<<"lookAtMatrix: "<<lookAtMatrix<<";\n";


	//Matrix4x4 translationRotationMatrix = lookAtMatrix._inverse();
	//LOG<<DEBUG_LOG_LEVEL<<"lookAtMatrix._inverse(): "<<translationRotationMatrix<<";\n";
	Matrix4x4 translationRotationMatrix = glm::inverse(lookAtMatrix);
	//LOG<<DEBUG_LOG_LEVEL<<"glm::inverse(lookAtMatrix); "<<translationRotationMatrix<<";\n";

	mAccumTranslationRotationScaleMatrix =
			translationRotationMatrix *
			//just the rotational component shall be scaled, not the translational
			getScaleMatrix();

	if(mOwningSceneNode) mOwningSceneNode->transformChanged(mIsGlobalTransform);
}


AmendedTransform::AmendedTransform(const Matrix4x4& transform)
: mOwningSceneNode(0)
{
	mPosition = Vector3D(transform[3]);
	assert("lowerRightComponent is one" && (std::fabs(transform[3][3]-1.0f) <0.001f ));

	assert("lowerleftComponents are zero"
			&& (std::fabs(transform[0][3]) <0.001f )
			&& (std::fabs(transform[1][3]) <0.001f )
			&& (std::fabs(transform[2][3]) <0.001f ));

	Vector3D rotMatLengths= Vector3D(
			glm::length(Vector3D(transform[0])),
			glm::length(Vector3D(transform[1])),
			glm::length(Vector3D(transform[2]))
	);
	assert("all lengths of the rotMatrix vectors are equal, i.e. scale is homogeneous"
			&&  ( std::fabs(rotMatLengths.x - rotMatLengths.y) 		< 0.001f )
			&& 	( std::fabs(rotMatLengths.x - rotMatLengths.z) 		< 0.001f )
	);
	mScale = rotMatLengths.x;

	assert("3x3 component is orthogonal rotation matrix"
			&&  ( std::fabs( glm::dot(transform[0],transform[1]) ) < 0.01f  )
			&&  ( std::fabs( glm::dot(transform[0],transform[2]) ) < 0.01f  )
			&&  ( std::fabs( glm::dot(transform[1],transform[2]) ) < 0.01f  )
	);

	//pick negative third row for direction (negative z-axis of rotation matrix)
	mDirection = glm::normalize( Vector3D(transform[2]) * (-1.0f) );

	//pick second row for upVector (y-axis of rotation matrix)
	mUpVector = glm::normalize( Vector3D(transform[1]) );

	setup();

	assert(matricesAreEqual(mAccumTranslationRotationScaleMatrix, transform));
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
	//BUG in glm _inverse!
	//return mAccumTranslationRotationScaleMatrix._inverse() * getInverseScaleMatrix();

	//mistake of myself:
	//return glm::inverse(mAccumTranslationRotationScaleMatrix) * getInverseScaleMatrix();

	//mathematical explanation:
	//mAccumTranslationRotationScaleMatrix == glm::inverse(lookAtMatrix)* getScaleMatrix()
	//mAccumTranslationRotationScaleMatrix * getInverseScaleMatrix() == glm::inverse(lookAtMatrix)
	//glm::inverse(mAccumTranslationRotationScaleMatrix * getInverseScaleMatrix()) == lookAtMatrix
	return glm::inverse(
			mAccumTranslationRotationScaleMatrix * getInverseScaleMatrix() );
}

////normal matrix: rotation matrix, scaled by 1/scale
//Matrix3x3 AmendedTransform::getNormalMatrix()const
//{
//	return getRotationMatrix()*  (1.0f/mScale) ;
//}
//accum: translationMatrix* Mat4(normalMatrix)* scaleMatrix;
Matrix4x4 AmendedTransform::getTotalTransform()const
{
	return mAccumTranslationRotationScaleMatrix;
}

AmendedTransform AmendedTransform::getInverse()const
{
	return AmendedTransform(
			//(TRS)^-1 == S^-1 * R^-1 * T^-1;
			//this gives a matrix where the fourth column is (inverse rotated negative original value scaled by 1/originalScale)
			glm::inverse(getRotationMatrix())*	(mPosition * (-1.0f/ mScale )),

			//inverse rotate dir and up: do the rotation twice, as we don't wanna return the
			//direction/up to neg.z/pos.y, but rotate it also in the other direction!
			glm::inverse(getRotationMatrix()) * glm::inverse(getRotationMatrix()) * mDirection,
			glm::inverse(getRotationMatrix()) * glm::inverse(getRotationMatrix()) * mUpVector,
			//glm::inverse(getRotationMatrix()) * mDirection,
			//glm::inverse(getRotationMatrix()) * mUpVector,

			1.0f / mScale
	);
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

bool AmendedTransform::matricesAreEqual(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	for(int col=0;col<4;col++)
	{
		for(int row=0; row<4;row++)
		{
			if(std::fabs(lhs[col][row]-rhs[col][row]) > 0.01 )
			{
				return false;
			}
		}
	}
	return true;
}

//----------------------------------------------------------

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

	if(mOwningSceneNode) mOwningSceneNode->transformChanged(mIsGlobalTransform);

	return *this;
}

const AmendedTransform& AmendedTransform::operator=(const AmendedTransform& rhs)
{
	mPosition=rhs.mPosition;
	mDirection=rhs.mDirection;
	mUpVector=rhs.mUpVector;
	mScale=rhs.mScale;

	//instead of copying, lets construct new and compare the outcome to see if there are some bugs;
	setup();
	assert(
		matricesAreEqual(mAccumTranslationRotationScaleMatrix, rhs.mAccumTranslationRotationScaleMatrix)
	);

	return *this;
}

void AmendedTransform::moveRelativeToDirection(float forwardBackward, float rightLeft, float upDown)
{
	mPosition +=
		(forwardBackward * 		glm::normalize( mDirection 					  	) ) +
		(rightLeft * 			glm::normalize( glm::cross(mDirection, mUpVector) ) +
		(upDown* 				glm::normalize( mUpVector					    ) )
	 );
	setup();
	//TODO check if works
}

//change direction by rotating it angleDegrees degrees around cross(direction,upVector)
void AmendedTransform::pitchRelativeToDirection(float angleDegrees)
{

		Vector3D newDir  = 	Vector3D (
			glm::rotate(
				angleDegrees,
				glm::normalize(glm::cross(mDirection, mUpVector))
			)
			* Vector4D(mDirection,0.0f)
		);

	//omit overturning and singularities at the peaks
	if( std::fabs(glm::dot(newDir, mUpVector)) < 0.99 )
	{
		mDirection = newDir;
		setup();
		//TODO check if works
	}
}

//change direction by rotating it angleDegrees degrees around the upVector;
void AmendedTransform::yawRelativeToUpVector(float angleDegrees)
{
	mDirection = 	Vector3D (
			glm::rotate(
				angleDegrees,
				glm::normalize(mUpVector)
			)
			* Vector4D(mDirection,0.0f)
	);

	setup();
	//TODO check if works
}


}


