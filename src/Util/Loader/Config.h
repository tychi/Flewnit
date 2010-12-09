/*
 * Config.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

namespace Flewnit
{

enum Type
{
	TYPE_UNDEF,

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

class ConfigNodeInterface: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	String mName;

public:

	virtual ~ConfigNodeInterface(){}

	explicit ConfigNodeInterface(String name): mName(name){};
	const String& getName()const{return mName;}

	virtual Type getNodeType()const=0;
	virtual bool isLeafNode()const=0;

};

class ConfigStructNode: public ConfigNodeInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	Map<String, ConfigNodeInterface*> mChildren;

public:

	explicit ConfigStructNode(String name):ConfigNodeInterface(name){}
	virtual ~ConfigStructNode();

	virtual Type getNodeType()const{return TYPE_UNDEF;}
	virtual bool isLeafNode()const{return false;}

};

template <typename T>
class ConfigLeafNode: public ConfigNodeInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	Type mType;
	T mValue;

public:
	explicit ConfigLeafNode(String name, T& value):ConfigNodeInterface(name), mValue(value){}
	virtual ~ConfigLeafNode(){}

	virtual Type getNodeType()const{return mType;}
	virtual bool isLeafNode()const{return true;}

	T& value(){return mValue;}

};



class Config : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

	ConfigStructNode* mRootNode;

public:
	Config():
		mRootNode(0)
	{
		 mRootNode=FLEWNIT_INSTANTIATE(new ConfigStructNode("flewnitGlobalConfig"));
	}

	virtual ~Config(){delete mRootNode;}

};

}

