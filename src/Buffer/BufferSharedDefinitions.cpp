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
			const BufferElementInfo& elementInfo)
: name(name),
  usageContexts(usageContexts),
  bufferSemantics(bufferSemantics),
  elementType(TYPE_UNDEF),
  numElements(0),
  elementInfo(elementInfo),
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
		const BufferElementInfo& elementInfo,
		GLBufferType glBufferType,
		ContextType mappedToCPUContext)
	: name(name),
	  usageContexts(usageContexts),
	  bufferSemantics(bufferSemantics),
	  elementType(elementType),
	  numElements(numElements),
	  elementInfo(elementInfo),
	  glBufferType(glBufferType),
	  isPingPongBuffer(false),
	  mappedToCPUContext(mappedToCPUContext)
{
	bufferSizeInByte = numElements * BufferHelper::elementSize(elementType);
}

BufferInfo::BufferInfo(const BufferInfo& rhs)
: elementInfo(rhs.elementInfo)
{
	(*this) = rhs;
	//in constructor, copy name; we only dont want a name copy in assignment per operator=
	name=rhs.name;
}

BufferInfo::~BufferInfo()
{

}

bool BufferInfo::operator==(const BufferInfo& rhs) const
{
		return
				//DON'T compare the name! the structure of the framework asserts unique names!
				//name==rhs.name	&&
				usageContexts==rhs.usageContexts &&
				bufferSemantics==rhs.bufferSemantics &&
				elementType==rhs.elementType &&
				elementInfo==rhs.elementInfo &&
				numElements==rhs.numElements &&
				glBufferType==rhs.glBufferType &&
				isPingPongBuffer==rhs.isPingPongBuffer &&
				mappedToCPUContext==rhs.mappedToCPUContext &&
				bufferSizeInByte==rhs.bufferSizeInByte;
				;
}

const BufferInfo& BufferInfo::operator=(const BufferInfo& rhs)
{
	//DON'T copy the name!
	//name=rhs.name; //TODO check if name copying has side effects;
	usageContexts=rhs.usageContexts;
	bufferSemantics=rhs.bufferSemantics;
	elementType=rhs.elementType;
	elementInfo=rhs.elementInfo;
	numElements=rhs.numElements;
	glBufferType=rhs.glBufferType;
	isPingPongBuffer=rhs.isPingPongBuffer;
	mappedToCPUContext=rhs.mappedToCPUContext;
	bufferSizeInByte=rhs.bufferSizeInByte;
	return *this;
}


//----------------------------------------------------------------

BufferElementInfo::BufferElementInfo(int numChannels,GPU_DataType internalGPU_DataType,int bitsPerChannel, bool normalizeIntegralValuesFlag)
: numChannels(numChannels),
  internalGPU_DataType(internalGPU_DataType),
  bitsPerChannel(bitsPerChannel),
  normalizeIntegralValuesFlag(normalizeIntegralValuesFlag),
  hasNoChanneledElements(false)
{
	validate();
}

BufferElementInfo::BufferElementInfo(const BufferElementInfo& rhs)
{
	*this = rhs;
	validate();
}

BufferElementInfo::BufferElementInfo(bool hasNoChanneledElements)
: numChannels(0),
  internalGPU_DataType(GPU_DATA_TYPE_FLOAT),
  bitsPerChannel(0),
  normalizeIntegralValuesFlag(false),
  hasNoChanneledElements(hasNoChanneledElements)
{
	validate();
}


//const BufferElementInfo& BufferElementInfo::operator=(const BufferElementInfo& rhs)
//{
//	numChannels = rhs.numChannels;
//	internalGPU_DataType = rhs.internalGPU_DataType;
//	bitsPerChannel = rhs.bitsPerChannel;
//	normalizeIntegralValuesFlag = rhs.normalizeIntegralValuesFlag;
//	hasNoChanneledElements = rhs.hasNoChanneledElements;
//
//	return *this;
//}

bool BufferElementInfo::operator==(const BufferElementInfo& rhs)const
{
	return
		numChannels == rhs.numChannels &&
		internalGPU_DataType == rhs.internalGPU_DataType &&
		bitsPerChannel == rhs.bitsPerChannel &&
		normalizeIntegralValuesFlag == rhs.normalizeIntegralValuesFlag &&
		hasNoChanneledElements == rhs.hasNoChanneledElements;
}

const BufferElementInfo& BufferElementInfo::operator=(const BufferElementInfo& rhs)
{
	numChannels = rhs.numChannels;
	internalGPU_DataType = rhs.internalGPU_DataType;
	bitsPerChannel = rhs.bitsPerChannel;
	normalizeIntegralValuesFlag = rhs.normalizeIntegralValuesFlag;
	hasNoChanneledElements = rhs.hasNoChanneledElements;

	return *this;
}

//called by constructor to early detect invalid values and permutations, like 8-bit float or 32bit normalized (u)int
void BufferElementInfo::validate()const throw (BufferException)
{
	if(hasNoChanneledElements)
	{
		//nothing to validate, unused
		return;
	}

	//check channel number:
	if(! ( (numChannels == 1) || (numChannels == 2) || (numChannels ==4)  ))
	{
		LOG<<ERROR_LOG_LEVEL << "BufferElementInfo::validate: wrong amount of channels: "<<numChannels<<";\n";
		throw(BufferException("BufferElementInfo::validate: numChannels must be 1,2 or 4!"));
	}

	//check bits per channel:
	if(! ( (bitsPerChannel == 8) || (bitsPerChannel == 16) || (bitsPerChannel ==32)  ))
	{
		LOG<<ERROR_LOG_LEVEL << "BufferElementInfo::validate: bitsPerChannel is not 8,16 or 32, but "<<bitsPerChannel<<";\n";
		throw(BufferException("BufferElementInfo::validate: bitsPerChannel must be 8,16 or 32!"));
	}

	if(internalGPU_DataType == GPU_DATA_TYPE_FLOAT)
	{
		if(normalizeIntegralValuesFlag)
		{
			throw(BufferException("normalization on float values makes no sense!"));
		}

		if(! ( (bitsPerChannel == 16) || (bitsPerChannel ==32)  ))
		{
			LOG<<ERROR_LOG_LEVEL << "BufferElementInfo::validate: float types need 16 or 32 bits per channel and not "<<bitsPerChannel<<";\n";
			throw(BufferException("float types need 16 or 32 bits per channel;"));
		}

	}

	if(normalizeIntegralValuesFlag)
	{
		//no check for floating point anymore, is catched above;

		//but check that we don't have 32 bit channels
		if(! ( (bitsPerChannel == 8) || (bitsPerChannel == 16)  ))
		{
			throw(BufferException("BufferElementInfo::validate: normalized integer types need 8 or 16 bits per channel; maybe 32 bit integral values are technically possible, but they don't make sense to me ;("));
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
		//const BufferElementInfo& texelInfo,
		GLenum textureTarget,
		bool isDepthTexture,
		bool isMipMapped,
		bool isRectangleTex,
		bool isCubeTex,
		GLint numMultiSamples,
		GLint numArrayLayers
		) throw(BufferException)
:	BufferInfo(buffi),
		dimensionality(dimensionality),
		dimensionExtends(dimensionExtends),
		//elementInfo(texelInfo),
		textureTarget(textureTarget),
		isDepthTexture(isDepthTexture),
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


	//BufferElementInfo class checks itself during construction for integrity; so, we can "construct"
	//our enums without error checking:
	calculateCLGLImageFormatValues();

}

bool TextureInfo::calculateCLGLImageFormatValues()throw (BufferException)
{
	elementInfo.validate();

	glBufferType= NO_GL_BUFFER_TYPE;
	numElements = dimensionExtends.x * dimensionExtends.y * dimensionExtends.z
			* numArrayLayers * numMultiSamples;

	if(isDepthTexture)
	{
		if( (elementInfo.numChannels != 1 )
				||
			(elementInfo.normalizeIntegralValuesFlag)
				||
			(elementInfo.bitsPerChannel != 32 )
				||
			(elementInfo.internalGPU_DataType != GPU_DATA_TYPE_FLOAT )
		)
		{
			throw(BufferException("BufferElementInfo is does not indicate a supported depth renderable format!"
					"Currently, only 32bit float is accepted"));
		}
		else
		{
			if( 	( dimensionality ==1 )
					//check for 1d array texture
					|| ((dimensionality == 1) && (numArrayLayers > 1))
					|| isMipMapped
					|| (numMultiSamples > 1)
			)
			{
				throw(BufferException("For depth textures, 1D textures, mip mapping"
				 " and/or multisampling is not supported (by this framework (yet) ;( )"));
			}
			else
			{
				if(isRectangleTex)
				{
					LOG<<WARNING_LOG_LEVEL<<"sorry at the moment there is no official support for rectangle depth textures;"
							<<"It should work, but use on your own risk, especially for shadowmapping (other sampler (samplerRectShadow) and bias matrix!);\n";
				}

				glImageFormat.desiredInternalFormat = GL_DEPTH_COMPONENT32;
				glImageFormat.channelOrder= GL_DEPTH_COMPONENT;

				glImageFormat.channelDataType = GL_FLOAT;

				clImageFormat.image_channel_data_type = CL_FLOAT;
				clImageFormat.image_channel_order = CL_R; //will it work?#

				switch(textureTarget)
				{
				case GL_TEXTURE_2D:
					textureType = TEXTURE_TYPE_2D_DEPTH;
					break;
				case GL_TEXTURE_RECTANGLE:
					textureType = TEXTURE_TYPE_2D_RECT_DEPTH;
					break;
				case GL_TEXTURE_CUBE_MAP:
					textureType = TEXTURE_TYPE_2D_CUBE_DEPTH;
					break;
				case GL_TEXTURE_2D_ARRAY:
					textureType = TEXTURE_TYPE_2D_ARRAY_DEPTH;
					break;
				}
			}
		}
	}
	else //depthtexture
	{
		//first, set the most trivial stuff: the number of channels
			switch(elementInfo.numChannels)
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

			switch(textureTarget)
			{
			case GL_TEXTURE_1D:
				textureType = TEXTURE_TYPE_1D;
				break;
			case GL_TEXTURE_1D_ARRAY:
				textureType = TEXTURE_TYPE_1D_ARRAY;
				break;


			case GL_TEXTURE_2D:
				textureType = TEXTURE_TYPE_2D;
				break;
			case GL_TEXTURE_RECTANGLE:
				textureType = TEXTURE_TYPE_2D_RECT;
				break;
			case GL_TEXTURE_CUBE_MAP:
				textureType = TEXTURE_TYPE_2D_CUBE;
				break;
			case GL_TEXTURE_2D_ARRAY:
				textureType = TEXTURE_TYPE_2D_ARRAY;
				break;
			case GL_TEXTURE_2D_MULTISAMPLE:
				textureType = TEXTURE_TYPE_2D_MULTISAMPLE;
				break;
			case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
				textureType = TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE;
				break;


			case GL_TEXTURE_3D:
				textureType = TEXTURE_TYPE_3D;
				break;

			}

			//------------------------------------------------------

			bool normalize =  elementInfo.normalizeIntegralValuesFlag;

			switch(elementInfo.internalGPU_DataType)
			{
			case GPU_DATA_TYPE_UINT :
				switch(elementInfo.bitsPerChannel)
				{
				case 8:
					glImageFormat.channelDataType = GL_UNSIGNED_BYTE;

					if(normalize)	clImageFormat.image_channel_data_type = CL_UNORM_INT8;
					else			clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;

					switch(elementInfo.numChannels)
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

					switch(elementInfo.numChannels)
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

					switch(elementInfo.numChannels)
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
				switch(elementInfo.bitsPerChannel)
				{
				case 8:
					glImageFormat.channelDataType = GL_BYTE;
					if(normalize)	clImageFormat.image_channel_data_type = CL_SNORM_INT8;
					else			clImageFormat.image_channel_data_type = CL_SIGNED_INT8;

					switch(elementInfo.numChannels)
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

					switch(elementInfo.numChannels)
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

					switch(elementInfo.numChannels)
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
				switch(elementInfo.bitsPerChannel)
				{
				case 16:
					//no normalization valid here!
					glImageFormat.channelDataType = GL_HALF_FLOAT;
					clImageFormat.image_channel_data_type = CL_HALF_FLOAT;

					switch(elementInfo.numChannels)
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

					switch(elementInfo.numChannels)
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
: 		BufferInfo(rhs)
		//,
		//elementInfo(rhs.elementInfo)
{
	(*this) = rhs;
}



bool TextureInfo::operator==(const TextureInfo& rhs) const
{
	return
			//TODO check if works
			BufferInfo::operator==(rhs) &&

			dimensionality == rhs.dimensionality &&
			glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
			//elementInfo == rhs.elementInfo &&
			textureTarget == rhs.textureTarget &&
			textureType == rhs.textureType &&

			isDepthTexture == rhs.isDepthTexture &&
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

bool TextureInfo::isRenderTargetCompatibleTo(const TextureInfo& rhs)const
{
	//partially redundant, omitting texel layout, ( latter must be tested if works)
	return
		dimensionality == rhs.dimensionality &&
		glm::all(glm::equal(dimensionExtends, rhs.dimensionExtends)) &&
		textureTarget == rhs.textureTarget &&
		isMipMapped == rhs.isMipMapped &&
		numMultiSamples == rhs.numMultiSamples &&
		numArrayLayers == rhs.numArrayLayers
		;
}


const TextureInfo& TextureInfo::operator=(const TextureInfo& rhs)
{
	//TODO check if works
	BufferInfo::operator=(rhs) ;

	dimensionality = rhs.dimensionality;
	dimensionExtends = rhs.dimensionExtends;
	//elementInfo = rhs.elementInfo;
	textureTarget = rhs.textureTarget;
	textureType = rhs.textureType;

	isDepthTexture=rhs.isDepthTexture;
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
