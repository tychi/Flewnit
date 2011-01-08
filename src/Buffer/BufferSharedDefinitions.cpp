/*
 * BufferSharedDefinitions.cpp
 *
 *  Created on: Jan 8, 2011
 *      Author: tychi
 */

#include "BufferSharedDefinitions.h"

#include "Buffer/BufferHelperUtils.h"
#include "Util/Log/Log.h"

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

TexelInfo::TexelInfo(int numChannels,GPU_DataType internalGPU_DataType,int bitsPerChannel, bool normalizeIntegralValuesFlag)
: numChannels(numChannels), internalGPU_DataType(internalGPU_DataType),bitsPerChannel(bitsPerChannel), normalizeIntegralValuesFlag(normalizeIntegralValuesFlag)
{
	validate();
}

TexelInfo::TexelInfo(const TexelInfo& rhs)
{
	*this = rhs;
}


bool TexelInfo::operator==(const TexelInfo& rhs)const
{
	return
		numChannels == rhs.numChannels &&
		internalGPU_DataType == rhs.internalGPU_DataType &&
		bitsPerChannel == rhs.bitsPerChannel &&
		normalizeIntegralValuesFlag == rhs.normalizeIntegralValuesFlag;
}

const TexelInfo& TexelInfo::operator=(const TexelInfo& rhs)
{
	numChannels = rhs.numChannels;
	internalGPU_DataType = rhs.internalGPU_DataType;
	bitsPerChannel = rhs.bitsPerChannel;
	normalizeIntegralValuesFlag = rhs.normalizeIntegralValuesFlag;

	return *this;
}

//called by constructor to early detect invalid values and permutations, like 8-bit float or 32bit normalized (u)int
void TexelInfo::validate() throw (BufferException)
{
	//check channel number:
	if(! ( (numChannels == 1) || (numChannels == 2) || (numChannels ==4)  ))
	{
		LOG<<ERROR_LOG_LEVEL << "TexelInfo::validate: wrong amount of channels: "<<numChannels<<";\n";
		throw(BufferException("TexelInfo::validate: numChannels must be 1,2 or 4!"));
	}

	//check bits per channel:
	if(! ( (bitsPerChannel == 8) || (bitsPerChannel == 16) || (bitsPerChannel ==32)  ))
	{
		LOG<<ERROR_LOG_LEVEL << "TexelInfo::validate: bitsPerChannel is not 8,16 or 32, but "<<bitsPerChannel<<";\n";
		throw(BufferException("TexelInfo::validate: bitsPerChannel must be 8,16 or 32!"));
	}

	if(internalGPU_DataType == GPU_DATA_TYPE_FLOAT)
	{
		if(normalizeIntegralValuesFlag)
		{
			throw(BufferException("normalization on float values makes no sense!"));
		}

		if(! ( (bitsPerChannel == 16) || (bitsPerChannel ==32)  ))
		{
			LOG<<ERROR_LOG_LEVEL << "TexelInfo::validate: float types need 16 or 32 bits per channel and not "<<bitsPerChannel<<";\n";
			throw(BufferException("float types need 16 or 32 bits per channel;"));
		}

	}

	if(normalizeIntegralValuesFlag)
	{
		//no check for floating pont anymore, is catched above;

		//but check that we don't have 32 bit channels
		if(! ( (bitsPerChannel == 8) || (bitsPerChannel == 16)  ))
		{
			LOG<<ERROR_LOG_LEVEL << "TexelInfo::validate: normalized integer types need 8 or 16 bits per channel and not "<<bitsPerChannel<<";\n";
			throw(BufferException("TexelInfo::validate: normalized integer types need 8 or 16 bits per channel;"));
		}
	}
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
	//TODO SETUP glImageFormat and clImageFormat from texelInfo;
	assert("not implemented yet" && 0);

}



TextureInfo::TextureInfo(const TextureInfo& rhs)
//init as the is no default contructor for texelInfo
: texelInfo(rhs.texelInfo)
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
			clImageFormat.image_channel_data_type == rhs.clImageFormat.image_channel_data_type &&
			clImageFormat.image_channel_order == rhs.clImageFormat.image_channel_order
			;

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
