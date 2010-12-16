/*
 * Config.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include "Common/Math.h"



namespace Flewnit
{

enum Type
{
	TYPE_UNDEF,

	TYPE_STRUCTURE,

	TYPE_BOOL,

	//stuff maybe needed for TextureBuffers
	TYPE_BOOL8,
	TYPE_BOOL16,
	TYPE_BOOL32,
	TYPE_BOOL64,

	TYPE_CHAR, //unsigned char, 1 Byte
	TYPE_INT8, //signed char, 1 Byte
	TYPE_UINT8,

	TYPE_INT16,
	TYPE_UINT16,

	TYPE_INT32,
	TYPE_UINT32,

	TYPE_INT64,
	TYPE_UINT64,

	TYPE_FLOAT,
	TYPE_DOUBLE,

	TYPE_STRING,

	//-----------------
	//all permutations of integer Vectors to come
	TYPE_VEC2I8,
	TYPE_VEC2UI8,

	TYPE_VEC2I16,
	TYPE_VEC2UI16,

	TYPE_VEC2I32,
	TYPE_VEC2UI32,

	TYPE_VEC2I64,
	TYPE_VEC2UI64,

	TYPE_VEC3I8,
	TYPE_VEC3UI8,

	TYPE_VEC3I16,
	TYPE_VEC3UI16,

	TYPE_VEC3I32,
	TYPE_VEC3UI32,

	TYPE_VEC3I64,
	TYPE_VEC3UI64,

	TYPE_VEC4I8,
	TYPE_VEC4UI8,

	TYPE_VEC4I16,
	TYPE_VEC4UI16,

	TYPE_VEC4I32,
	TYPE_VEC4UI32,

	TYPE_VEC4I64,
	TYPE_VEC4UI64,
	//-----------------

	TYPE_VEC2F,
	TYPE_VEC2D,

	TYPE_VEC3F,
	TYPE_VEC3D,

	TYPE_VEC4F,
	TYPE_VEC4D,

	TYPE_QUAT4F, // 4 floats encoding a quaternion {qx,qy,qz,qs}
	TYPE_QUAT4D, // 4 doubles encoding a quaternion {qx,qy,qz,qs}

	TYPE_MATRIX33F,
	TYPE_MATRIX33D,

	TYPE_MATRIX44F,
	TYPE_MATRIX44D,

	//semantics and encoding are treated otherwise ;)
//	TYPE_COLOR32, // 32 bits color. Order is RGBA if API is OpenGL or Direct3D10, and inversed if API is Direct3D9 (can be modified by defining 'colorOrder=...', see doc)
//	TYPE_COLOR3F, // 3 floats color. Order is RGB.
//	TYPE_COLOR4F, // 4 floats color. Order is RGBA.
//
//	TYPE_DIRECTION3F, // direction vector represented by 3 floats
//	TYPE_DIRECTION3D // direction vector represented by 3 doubles


};



enum Access
{
	ACCESS_NONE,
	ACCESS_READ,
	ACCESS_READWRITE
};

class GUIParams
{
	//this class is so tiny, there will be no pointer-f***up, so we need no memory tracking of this class
public:
	GUIParams():mGUIVisibility(ACCESS_NONE), mGUIPropertyString(""){}
	GUIParams(Access GUIVisibility, String GUIPropertyString):mGUIVisibility(GUIVisibility), mGUIPropertyString(GUIPropertyString){}

	GUIParams(const GUIParams& other):mGUIVisibility(other.getGUIVisibility()),mGUIPropertyString(other.getGUIPropertyString()){}
	const GUIParams& operator=(const GUIParams& other){mGUIVisibility=other.getGUIVisibility();mGUIPropertyString=other.getGUIPropertyString(); return *this;}

    Access getGUIVisibility() const{return mGUIVisibility;}
    void setGUIVisibility(Access mGUIVisibility){this->mGUIVisibility = mGUIVisibility;}

    void setGUIPropertyString(String str){mGUIPropertyString = str;}
    const String& getGUIPropertyString()const{return mGUIPropertyString;}


private:
	Access mGUIVisibility;
	String mGUIPropertyString;

};



class ConfigStructNode: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	Map<String, List<ConfigStructNode*> > mChildren;

	//String mName;

protected:

	Type mValueType;

public:

	ConfigStructNode(): mValueType(TYPE_STRUCTURE){}
	virtual ~ConfigStructNode();


	bool isLeafNode()const{return (mChildren.size()==0);}

	List<ConfigStructNode*>& operator[](String name);

	List<ConfigStructNode*>& get(String name);
	ConfigStructNode& get(String name, int index);

	//ConfigStructNode*& get(String name){return mChildren[name];}

	Type getType()const{return mValueType;}

};

template <typename T>
class ConfigValueNode: public ConfigStructNode
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	T mValue;

	GUIParams mGUIParams;

public:

	ConfigValueNode( T value, GUIParams guiParams = GUIParams() ):ConfigStructNode(), mValue(value), mGUIParams(guiParams)
	{
		mValueType = initType();
	}

	virtual ~ConfigValueNode()
	{
		// LOG<<DEBUG_LOG_LEVEL<<"destroying ConfigValueNode; hope to kill all children of this:P ;\n";
	}

	ConfigValueNode( const ConfigValueNode& other)
	: mValue(other.value()), mGUIParams(other.getGUIParams())
	{}



	const T& value(){return mValue;}
	const GUIParams& getGUIParams(){return mGUIParams;}

private:

	virtual Type initType()const
	{
		//not all types define in teh Type-enum are checked, but only the most common;

		if(typeid(mValue) == typeid(bool))
			return TYPE_BOOL;
		if(typeid(mValue) == typeid(unsigned char))
			return TYPE_CHAR;
		if(typeid(mValue) == typeid(short))
			return TYPE_INT16;
		if(typeid(mValue) == typeid(unsigned short))
			return TYPE_UINT16;
		if(typeid(mValue) == typeid(int))
			return TYPE_INT32;
		if(typeid(mValue) == typeid(unsigned int))
			return TYPE_UINT32;
		if(typeid(mValue) == typeid(long))
			return TYPE_INT64;
		if(typeid(mValue) == typeid(long))
			return TYPE_INT64;
		if(typeid(mValue) == typeid(unsigned long))
			return TYPE_UINT64;
		if(typeid(mValue) == typeid(float))
			return TYPE_FLOAT;
		if(typeid(mValue) == typeid(double))
			return TYPE_DOUBLE;
		if(typeid(mValue) == typeid(Vector2D))
			return TYPE_VEC2F;
		if(typeid(mValue) == typeid(Vector3D))
			return TYPE_VEC3F;
		if(typeid(mValue) == typeid(Vector4D))
			return TYPE_VEC4F;

		if(typeid(mValue) == typeid(Vector2Di))
			return TYPE_VEC2I32;
		if(typeid(mValue) == typeid(Vector3Di))
			return TYPE_VEC3I32;
		if(typeid(mValue) == typeid(Vector4Di))
			return TYPE_VEC4I32;

		if(typeid(mValue) == typeid(Vector2Dui))
			return TYPE_VEC2UI32;
		if(typeid(mValue) == typeid(Vector3Dui))
			return TYPE_VEC3UI32;
		if(typeid(mValue) == typeid(Vector4Dui))
			return TYPE_VEC4UI32;

		if(typeid(mValue) == typeid(Matrix3x3))
			return TYPE_MATRIX33F;
		if(typeid(mValue) == typeid(Matrix4x4))
			return TYPE_MATRIX44F;
		if(typeid(mValue) == typeid(Quaternion))
			return TYPE_QUAT4F;

		if(typeid(mValue) == typeid(String))
			return TYPE_STRING;



		//default
		return  TYPE_UNDEF;
	}

};




class Config : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	ConfigStructNode* mRootNode;

	//necessary for initialization: <-- NOT ;(
	//friend class Loader;
	//ConfigStructNode* & rootPtr(){return mRootNode;}



public:
	Config(): mRootNode(0)
	{
		mRootNode=FLEWNIT_INSTANTIATE(new ConfigStructNode());
	}

	virtual ~Config(){delete mRootNode;}

	ConfigStructNode & root(){return *mRootNode;}

};


}

