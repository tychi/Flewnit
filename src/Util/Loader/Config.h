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

class ConfigStructNode;
typedef Map<String, List<ConfigStructNode*> > ConfigMap;

class ConfigStructNode: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS


	ConfigMap mChildren;

	String mName;

protected:

	Type mValueType;

public:

	ConfigStructNode(String name): mName(name), mValueType(TYPE_STRUCTURE){}
	virtual ~ConfigStructNode();


	String getName()const{return mName;}

	bool isLeafNode()const{return (mChildren.size()==0);}

	List<ConfigStructNode*>& operator[](String name);

	List<ConfigStructNode*>& get(String name);
	ConfigStructNode& get(String name, int index);
	bool childExists(String name, int index);
	ConfigMap& getChildren(){return mChildren;}

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

	ConfigValueNode(String name, T value, GUIParams guiParams = GUIParams() ):ConfigStructNode(name), mValue(value), mGUIParams(guiParams)
	{
		mValueType = initType();
	}

	virtual ~ConfigValueNode()
	{
		// LOG<<DEBUG_LOG_LEVEL<<"destroying ConfigValueNode; hope to kill all children of this:P ;\n";
	}

	ConfigValueNode( const ConfigValueNode& other)
	: ConfigStructNode(other.getName()), mValue(other.value()), mGUIParams(other.getGUIParams())
	{}



	T value(){return mValue;}
	const GUIParams& getGUIParams(){return mGUIParams;}

private:

	virtual Type initType()const
	{
		//not all types define in teh Type-enum are checked, but only the most common;

		if(typeid(mValue) == typeid(bool))
			return TYPE_BOOL;

		if(typeid(mValue) == typeid(signed char))
			return TYPE_INT8;
		//to be sure without "signed" tag:
		if(typeid(mValue) == typeid(char))
			return TYPE_INT8;

		if(typeid(mValue) == typeid(unsigned char))
			return TYPE_UINT8;

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

		//-----------------------
		//smaller types, for textures etc; experimental!
		///\{
		if(typeid(mValue) == typeid(Vector2D8i))
			return TYPE_VEC2I8;
		if(typeid(mValue) == typeid(Vector2D8ui))
			return TYPE_VEC2UI8;

		if(typeid(mValue) == typeid(Vector3D8i))
			return TYPE_VEC3I8;
		if(typeid(mValue) == typeid(Vector3D8ui))
			return TYPE_VEC3UI8;

		if(typeid(mValue) == typeid(Vector4D8i))
			return TYPE_VEC4I8;
		if(typeid(mValue) == typeid(Vector4D8ui))
			return TYPE_VEC4UI8;

		if(typeid(mValue) == typeid(Vector2D16i))
			return TYPE_VEC2I16;
		if(typeid(mValue) == typeid(Vector2D16ui))
			return TYPE_VEC2UI16;

		if(typeid(mValue) == typeid(Vector3D16i))
			return TYPE_VEC3I16;
		if(typeid(mValue) == typeid(Vector3D16ui))
			return TYPE_VEC3UI16;

		if(typeid(mValue) == typeid(Vector4D16i))
			return TYPE_VEC4I16;
		if(typeid(mValue) == typeid(Vector4D16ui))
			return TYPE_VEC4UI16;

		if(typeid(mValue) == typeid(Half))
			return TYPE_HALF_FLOAT;
		if(typeid(mValue) == typeid(Vector2Dhalf))
			return TYPE_VEC2F16;
		if(typeid(mValue) == typeid(Vector3Dhalf))
			return TYPE_VEC3F16;
		if(typeid(mValue) == typeid(Vector4Dhalf))
			return TYPE_VEC4F16;
		///\}
		//-----------------------

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
		mRootNode=FLEWNIT_INSTANTIATE(new ConfigStructNode("flewnitGlobalConfig"));
	}

	virtual ~Config(){delete mRootNode;}

	ConfigStructNode & root(){return *mRootNode;}

};


}

