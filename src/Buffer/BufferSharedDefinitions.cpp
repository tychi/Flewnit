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

TextureInfo::TextureInfo(
		cl_GLuint dimensionality,
		Vector3Dui dimensionExtends,

		GLenum textureTarget,
		GLint imageInternalChannelLayout,
		GLenum imageInternalDataType,

		GLint numMultiSamples,
		bool isMipMapped
		)
: dimensionality(dimensionality),
  dimensionExtends(dimensionExtends),
  textureTarget(textureTarget),
  imageInternalChannelLayout(imageInternalChannelLayout),
  imageInternalDataType(imageInternalDataType),
  numMultiSamples(numMultiSamples),
  isMipMapped(isMipMapped)
{}

TextureInfo::TextureInfo(const TextureInfo& rhs)
{
	(*this) = rhs;
}

TextureInfo::~TextureInfo()
{}


bool TextureInfo::operator==(const TextureInfo& rhs) const
{
	return
		dimensionality==rhs.dimensionality &&
		glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
//		dimensionExtends.x==rhs.dimensionExtends.x &&
//		dimensionExtends.y==rhs.dimensionExtends.y &&
//		dimensionExtends.z==rhs.dimensionExtends.z &&
		textureTarget==rhs.textureTarget &&
		imageInternalChannelLayout==rhs.imageInternalChannelLayout &&
		imageInternalDataType==rhs.imageInternalDataType &&
		numMultiSamples==rhs.numMultiSamples &&
		isMipMapped==rhs.isMipMapped
		;
}

const TextureInfo& TextureInfo::operator=(const TextureInfo& rhs)
{
	dimensionality=rhs.dimensionality;
	dimensionExtends=rhs.dimensionExtends ;
	textureTarget=rhs.textureTarget ;
	imageInternalChannelLayout=rhs.imageInternalChannelLayout;
	imageInternalDataType=rhs.imageInternalDataType;
	numMultiSamples=rhs.numMultiSamples;
	isMipMapped=rhs.isMipMapped;

	return *this;
}


}
