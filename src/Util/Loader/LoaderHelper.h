/*
 * LoaderHelper.h
 *
 *  Created on: Dec 9, 2010
 *      Author: tychi
 *
 * private Header with helper functions
 */

#pragma once


#include "Common/FlewnitSharedDefinitions.h"

#include "Common/Math.h"

#include "Config.h"
#include "Util/Log/Log.h"

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <tinyxml.h>

namespace Flewnit
{


class ConfigCaster
{
public:
	template<typename T>
	static const T& cast(ConfigStructNode* csn)
	{
		ConfigValueNode<T>* cvn = dynamic_cast< ConfigValueNode<T>* > (csn);

		if(cvn)
		{
			return cvn->value();
		}
		else
		{
			assert("Bad cast of ConfigStructNode!" && 0);
			//maybe should reaqlize type-prettyprinting... but later...
		}
	}

};

//class LoaderHelper
//{
//public:
//
//	template<typename T>
//	static T getAttribute(TiXmlElement *xmlElement, const String &attributeName)
//	{
//		if (xmlElement->Attribute(attributeName.c_str()))
//		{
//
//			if (typeid(T) == typeid(bool))
//			{
//				return ( String( xmlElement->Attribute(attributeName.c_str()) ) == "true");
//			}
//
//			if (typeid(T) == typeid(String))
//			{
//				return String(xmlElement->Attribute(attributeName.c_str()));
//			}
//
//			return boost::lexical_cast<T>(xmlElement->Attribute(attributeName.c_str()));
//
//
//		}
//		else
//		{
//			return getDefaultValue<T>();
//		}
//
//
//
//
//
//	}
//
//	template<typename T>
//	static T getDefaultValue()
//	{
//		if (typeid(T) == typeid(String))
//					return "";
//
//		if (typeid(T) == typeid(bool))
//			return false;
//		if (typeid(T) == typeid(unsigned char))
//			return 0;
//		if (typeid(T) == typeid(short))
//			return 0;
//		if (typeid(T) == typeid(unsigned short))
//			return 0;
//		if (typeid(T) == typeid(int))
//			return 0;
//		if (typeid(T) == typeid(unsigned int))
//			return 0;
//		if (typeid(T) == typeid(long))
//			return 0;
//		if (typeid(T) == typeid(long))
//			return 0;
//		if (typeid(T) == typeid(unsigned long))
//			return 0;
//		if (typeid(T) == typeid(float))
//			return 0.0f;
//		if (typeid(T) == typeid(double))
//			return 0.0;
//		if (typeid(T) == typeid(Vector2D))
//			return Vector2D(0.0f,0.0f);
//
//		if (typeid(T) == typeid(Vector3D))
//			return Vector3D(0.0f,0.0f,0.0f);
//		if (typeid(T) == typeid(Vector4D))
//			return Vector4D(0.0f,0.0f,0.0f,0.0f);
//
////		if (typeid(mValue) == typeid(Vector2Di))
////			return TYPE_VEC2I32;
////		if (typeid(mValue) == typeid(Vector3Di))
////			return TYPE_VEC3I32;
////		if (typeid(mValue) == typeid(Vector4Di))
////			return TYPE_VEC4I32;
////
////		if (typeid(mValue) == typeid(Vector2Dui))
////			return TYPE_VEC2UI32;
////		if (typeid(mValue) == typeid(Vector3Dui))
////			return TYPE_VEC3UI32;
////		if (typeid(mValue) == typeid(Vector4Dui))
////			return TYPE_VEC4UI32;
////
////		if (typeid(mValue) == typeid(Matrix3x3))
////			return TYPE_MATRIX33F;
////		if (typeid(mValue) == typeid(Matrix4x4))
////			return TYPE_MATRIX44F;
////		if (typeid(mValue) == typeid(Quaternion))
////			return TYPE_QUAT4F;
//
//		//default
//		return 0;
//	}
//};

}
