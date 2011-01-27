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
			BufferSemantics bufferSemantics)
: name(name),
  usageContexts(usageContexts),
  bufferSemantics(bufferSemantics),
  elementType(TYPE_UNDEF),
  numElements(0),
  glBufferType(NO_GL_BUFFER_TYPE),
  isPingPongBuffer(false),
  mappedToCPUContext(NO_CONTEXT_TYPE),
  bufferSizeInByte(0)
{
}


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
				mappedToCPUContext==rhs.mappedToCPUContext &&
				bufferSizeInByte==rhs.bufferSizeInByte;
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
	bufferSizeInByte=rhs.bufferSizeInByte;
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
	validate();
}

TexelInfo::TexelInfo()
: numChannels(0), internalGPU_DataType(GPU_DATA_TYPE_FLOAT),bitsPerChannel(0), normalizeIntegralValuesFlag(false)
{}


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
void TexelInfo::validate()const throw (BufferException)
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

		//TODO
		if(internalGPU_DataType == GPU_DATA_TYPE_FLOAT)
		{
			LOG<<ERROR_LOG_LEVEL << "TexelInfo::validate: only integral values can be normalized;\n";
			throw(BufferException("TexelInfo::validate: only integral values can be normalized;"));

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
		const BufferInfo& buffi,
		cl_GLuint dimensionality,
		Vector3Dui dimensionExtends,
		const TexelInfo& texelInfo,
		GLenum textureTarget,
		bool isMipMapped,
		bool isRectangleTex,
		bool isCubeTex,
		GLint numMultiSamples,
		GLint numArrayLayers
		) throw(BufferException)
:	BufferInfo(buffi),
		dimensionality(dimensionality),
		dimensionExtends(dimensionExtends),
		texelInfo(texelInfo),
		textureTarget(textureTarget),
		isMipMapped(isMipMapped),
		isRectangleTex(isRectangleTex),
		isCubeTex(isCubeTex),
		numMultiSamples(numMultiSamples),
		numArrayLayers(numArrayLayers)
{

//TODO validate that there are no bad combinations:

//	if( (isRectangleTex && (isMipMapped || (numMultiSamples != 0) || (numArrayLayers != 0)) ) ||
//		(isMipMapped && (numMultiSamples != 0)) ||
//		(isCubeTex && (numMultiSamples != 0) ) ||
//		(isCubeTex && (numArrayLayers != 0) )||  //cube map arrays possible in gl4 but not gl3.3
//		((dimensionality == 1) && ())
//	)
//	{
//
//	}


	//TexelInfo class checks itself during construction for integrity; so, we can "construct"
	//our enums without error checking:
	calculateCLGLImageFormatValues();

}

bool TextureInfo::calculateCLGLImageFormatValues()throw (BufferException)
{
	texelInfo.validate();

	glBufferType= NO_GL_BUFFER_TYPE;
	numElements = dimensionExtends.x * dimensionExtends.y * dimensionExtends.z
			* numArrayLayers * numMultiSamples;

	//first, set the most trivial stuff: the number of channels
	switch(texelInfo.numChannels)
	{
	case 1:
		glImageFormat.channelOrder  = GL_RED;
		clImageFormat.image_channel_order = CL_R ;
		break;
	case 2:
		glImageFormat.channelOrder  = GL_RG;
		clImageFormat.image_channel_order = CL_RG ;
		break;
	case 4:
		glImageFormat.channelOrder  = GL_RGBA;
		clImageFormat.image_channel_order = CL_RGBA ;
		break;
	default:
		assert(0&&"should never end here");
	}
	//------------------------------------------------------

	bool normalize =  texelInfo.normalizeIntegralValuesFlag;

	switch(texelInfo.internalGPU_DataType)
	{
	case GPU_DATA_TYPE_UINT :
		switch(texelInfo.bitsPerChannel)
		{
		case 8:
			glImageFormat.channelDataType = GL_UNSIGNED_BYTE;

			if(normalize)	clImageFormat.image_channel_data_type = CL_UNORM_INT8;
			else			clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_UINT8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_R8;
				else			glImageFormat.desiredInternalFormat  = GL_R8UI;
				break;
			case 2:
				elementType = TYPE_VEC2UI8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RG8;
				else			glImageFormat.desiredInternalFormat  = GL_RG8UI;
				break;
			case 4:
				elementType = TYPE_VEC4UI8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RGBA8;
				else			glImageFormat.desiredInternalFormat  = GL_RGBA8UI;
				break;
			default:
				assert(0&&"should never end here");
			}
			break;
		case 16:
			glImageFormat.channelDataType = GL_UNSIGNED_SHORT;

			if(normalize)	clImageFormat.image_channel_data_type = CL_UNORM_INT16;
			else			clImageFormat.image_channel_data_type = CL_UNSIGNED_INT16;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_UINT16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_R16;
				else			glImageFormat.desiredInternalFormat  = GL_R16UI;
				break;
			case 2:
				elementType = TYPE_VEC2UI16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RG16;
				else			glImageFormat.desiredInternalFormat  = GL_RG16UI;
				break;
			case 4:
				elementType = TYPE_VEC4UI16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RGBA16;
				else			glImageFormat.desiredInternalFormat  = GL_RGBA16UI;
				break;
			default:
				assert(0&&"should never end here");
			}
			break;
		case 32:
			//no normalization valid here!
			glImageFormat.channelDataType = GL_UNSIGNED_INT;

			clImageFormat.image_channel_data_type = CL_UNSIGNED_INT32;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_UINT32;
				glImageFormat.desiredInternalFormat  = GL_R32UI;
				break;
			case 2:
				elementType = TYPE_VEC2UI32;
				glImageFormat.desiredInternalFormat  = GL_RG32UI;
				break;
			case 4:
				elementType = TYPE_VEC4UI32;
				glImageFormat.desiredInternalFormat  = GL_RGBA32UI;
				break;
			default:
				assert(0&&"should never end here");
			}
			break;
		default:
			assert(0&&"should never end here");
			break;
		}
		break;
	//-----------------------------------------------------------------------------------
	case GPU_DATA_TYPE_INT:
		switch(texelInfo.bitsPerChannel)
		{
		case 8:
			glImageFormat.channelDataType = GL_BYTE;
			if(normalize)	clImageFormat.image_channel_data_type = CL_SNORM_INT8;
			else			clImageFormat.image_channel_data_type = CL_SIGNED_INT8;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_INT8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_R8_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_R8I;
				break;
			case 2:
				elementType = TYPE_VEC2I8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RG8_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_RG8I;
				break;
			case 4:
				elementType = TYPE_VEC4I8;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RGBA8_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_RGBA8I;
				break;
			default:
				assert(0&&"should never end here");
			}
			break;
		case 16:
			glImageFormat.channelDataType = GL_SHORT;
			if(normalize)	clImageFormat.image_channel_data_type = CL_SNORM_INT16;
			else			clImageFormat.image_channel_data_type = CL_SIGNED_INT16;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_INT16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_R16_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_R16I;
				break;
			case 2:
				elementType = TYPE_VEC2I16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RG16_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_RG16I;
				break;
			case 4:
				elementType = TYPE_VEC4I16;
				if(normalize)	glImageFormat.desiredInternalFormat  = GL_RGBA16_SNORM;
				else			glImageFormat.desiredInternalFormat  = GL_RGBA16I;
				break;
			default:
				assert(0&&"should never end here");
			}
			break;
		case 32:
			//no normalization valid here!
			glImageFormat.channelDataType = GL_INT;
			clImageFormat.image_channel_data_type = CL_SIGNED_INT32;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_INT32;
				glImageFormat.desiredInternalFormat  = GL_R32I;		break;
			case 2:
				elementType = TYPE_VEC2I32;
				glImageFormat.desiredInternalFormat  = GL_RG32I;	break;
			case 4:
				elementType = TYPE_VEC4I32;
				glImageFormat.desiredInternalFormat  = GL_RGBA32I;	break;
			default:
				assert(0&&"should never end here");
			}
			break;
		default:
			assert(0&&"should never end here");
			break;
		}
		break;
	//----------------------------------------------------------------------------------
	case GPU_DATA_TYPE_FLOAT:
		switch(texelInfo.bitsPerChannel)
		{
		case 16:
			//no normalization valid here!
			glImageFormat.channelDataType = GL_HALF_FLOAT;
			clImageFormat.image_channel_data_type = CL_HALF_FLOAT;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_HALF_FLOAT;
				glImageFormat.desiredInternalFormat  = GL_R16F;		break;
			case 2:
				elementType = TYPE_VEC2F16;
				glImageFormat.desiredInternalFormat  = GL_RG16F;	break;
			case 4:
				elementType = TYPE_VEC4F16;
				glImageFormat.desiredInternalFormat  = GL_RGBA16F;	break;
			default:
				assert(0&&"should never end here");
			}
			break;
		case 32:
			//no normalization valid here!
			glImageFormat.channelDataType = GL_FLOAT;
			clImageFormat.image_channel_data_type = CL_FLOAT;

			switch(texelInfo.numChannels)
			{
			case 1:
				elementType = TYPE_FLOAT;
				glImageFormat.desiredInternalFormat  = GL_R32F;		break;
			case 2:
				elementType = TYPE_VEC2F;
				glImageFormat.desiredInternalFormat  = GL_RG32F;	break;
			case 4:
				elementType = TYPE_VEC4F;
				glImageFormat.desiredInternalFormat  = GL_RGBA32F;	break;
			default:
				assert(0&&"should never end here");
			}
			break;
		default:
			assert(0&&"should never end here");
			break;
		}
		break;
	default:
		assert(0&&"should never and here");
		break;
	}

	GLint maxNumMultiSamples;
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, & maxNumMultiSamples);

	if(! BufferHelper::isPowerOfTwo(numMultiSamples) ||
			(numMultiSamples > maxNumMultiSamples))
	{
		throw(BufferException("numMultiSamples must be power of two "
				"and <= GL_MAX_COLOR_TEXTURE_SAMPLES"));
	}


	bufferSizeInByte = BufferHelper::elementSize(elementType) * numElements;
	if(isCubeTex)
	{
		bufferSizeInByte *= 6;
	}

	return true;
}

TextureInfo::TextureInfo(const TextureInfo& rhs)throw(BufferException)
//init as the is no default contructor for texelInfo
: 		BufferInfo(rhs),
		texelInfo(rhs.texelInfo)
{
	(*this) = rhs;
}



bool TextureInfo::operator==(const TextureInfo& rhs) const
{
	return
			//TODO check if works
			BufferInfo::operator ==(rhs) &&

			dimensionality == rhs.dimensionality &&
			glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
			texelInfo == rhs.texelInfo &&
			textureTarget == rhs.textureTarget &&

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
	//TODO check if works
	BufferInfo::operator=(rhs) ;

	dimensionality = rhs.dimensionality;
	dimensionExtends = rhs.dimensionExtends;
	texelInfo = rhs.texelInfo;
	textureTarget = rhs.textureTarget;

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
