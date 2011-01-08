/*
 * BufferSharedDefinitions.cpp
 *
 *  Created on: Jan 8, 2011
 *      Author: tychi
 */

#include "BufferSharedDefinitions.h"

#include "Buffer/BufferHelperUtils.h"

namespace Flewnit
{


BufferInfo::BufferInfo(String name,
		ContextTypeFlags usageContexts,
		BufferSemantics bufferSemantics,
		Type elementType,
		cl_GLuint numElements,
		GLBufferType glBufferType,
		ContextType mappedToCPUContext)
	: name(name),
	  usageContexts(usageContexts),
	  bufferSemantics(bufferSemantics),
	  elementType(elementType),
	  numElements(numElements),
	  glBufferType(glBufferType),
	  isPingPongBuffer(false),
	  mappedToCPUContext(mappedToCPUContext)
{
	bufferSizeInByte = numElements * BufferHelper::elementSize(elementType);
}

BufferInfo::BufferInfo(const BufferInfo& rhs)
{
	(*this) = rhs;
}

BufferInfo::~BufferInfo()
{

}

bool BufferInfo::operator==(const BufferInfo& rhs) const
{
		return
				//NOT name equality, this doesn't matter, in the contrary, the names should be unique for convenient referancation in the data base!
				name==rhs.name	&&
				usageContexts==rhs.usageContexts &&
				bufferSemantics==rhs.bufferSemantics &&
				elementType==rhs.elementType &&
				numElements==rhs.numElements &&
				glBufferType==rhs.glBufferType &&
				isPingPongBuffer==rhs.isPingPongBuffer &&
				mappedToCPUContext==rhs.mappedToCPUContext
				;
}

const BufferInfo& BufferInfo::operator=(const BufferInfo& rhs)
{
	name=rhs.name; //TODO check if name copying has side effects;
	usageContexts=rhs.usageContexts;
	bufferSemantics=rhs.bufferSemantics;
	elementType=rhs.elementType;
	numElements=rhs.numElements;
	glBufferType=rhs.glBufferType;
	isPingPongBuffer=rhs.isPingPongBuffer;
	mappedToCPUContext=rhs.mappedToCPUContext;

	return *this;
}


//----------------------------------------------------------------


GLImageFormat::GLImageFormat(
		GLint  	desiredInternalFormat,
		GLenum 	channelOrder,
		GLenum	channelDataType	)
:
	desiredInternalFormat(desiredInternalFormat),
	channelOrder(channelOrder),
	channelDataType(channelDataType)
{}

GLImageFormat::GLImageFormat(const GLImageFormat& rhs)
{
	*this = rhs;
}

bool GLImageFormat::operator==(const GLImageFormat& rhs) const
{
	return
			desiredInternalFormat == rhs.desiredInternalFormat &&
			channelOrder == rhs.channelOrder &&
			channelDataType == rhs.channelDataType
			;
}

const GLImageFormat& GLImageFormat::operator=(const GLImageFormat& rhs)
{
	desiredInternalFormat = rhs.desiredInternalFormat;
	channelOrder = rhs.channelOrder;
	channelDataType = rhs.channelDataType;

	return *this;
}



//----------------------------------------------------------------



TextureInfo::TextureInfo(
		cl_GLuint dimensionality,
		Vector3Dui dimensionExtends,
		const TexelInfo& texelInfo,
		bool isMipMapped,
		bool isRectangleTex,
		bool isCubeTex,
		GLint numMultiSamples,
		GLint numArrayLayers
		)
:
		dimensionality(dimensionality),
		dimensionExtends(dimensionExtends),
		texelInfo(texelInfo),
		isMipMapped(isMipMapped),
		isRectangleTex(isRectangleTex),
		isCubeTex(isCubeTex),
		numMultiSamples(numMultiSamples),
		numArrayLayers(numArrayLayers)
{
	TODO SETUP glImageFormat and clImageFormat

}



TextureInfo::TextureInfo(const TextureInfo& rhs)
{
	(*this) = rhs;
}



bool TextureInfo::operator==(const TextureInfo& rhs) const
{
	return
			dimensionality == rhs.dimensionality &&
			glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
			texelInfo == rhs.texelInfo &&
			isMipMapped == rhs.isMipMapped &&
			isRectangleTex == rhs.isRectangleTex &&
			isCubeTex == rhs.isCubeTex &&
			numMultiSamples == rhs.numMultiSamples &&
			numArrayLayers == rhs.numArrayLayers &&

			glImageFormat == rhs.glImageFormat &&
			clImageFormat == rhs.clImageFormat;

}

const TextureInfo& TextureInfo::operator=(const TextureInfo& rhs)
{
	dimensionality = rhs.dimensionality;
	dimensionExtends = rhs.dimensionExtends;
	texelInfo = rhs.texelInfo;
	isMipMapped = rhs.isMipMapped;
	isRectangleTex = rhs.isRectangleTex;
	isCubeTex = rhs.isCubeTex;
	numMultiSamples = rhs.numMultiSamples;
	numArrayLayers = rhs.numArrayLayers;

	glImageFormat = rhs.glImageFormat;
	clImageFormat = rhs.clImageFormat;

	return *this;
}


}
