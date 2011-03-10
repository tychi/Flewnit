/*
 * BufferSharedDefinitions.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief Definitions needed to discriminate the concrete buffers all implementing "BufferInterface". Furthermore,\
 * they make validations of render targets and Geometries easier;
 *
 */

#pragma once

#include "Simulator/SimulatorForwards.h"


#include "Common/Math.h"
#include "Common/CL_GL_Common.h"

//#define FLEWNIT_NOT_INCLUDED_FOR_QT_META_OBJECT
#include "Simulator/ShaderFeatureRelevantEnums.h"
//#undef FLEWNIT_NOT_INCLUDED_FOR_QT_META_OBJECT



namespace Flewnit
{


typedef void* CPUBufferHandle;
typedef GLuint GraphicsBufferHandle;
typedef cl::Memory ComputeBufferHandle;



//--------------------------

enum ContextType
{
	HOST_CONTEXT_TYPE				=0,
	OPEN_CL_CONTEXT_TYPE			=1,
	OPEN_GL_CONTEXT_TYPE			=2,
	__NUM_CONTEXT_TYPES__			=3,
	NO_CONTEXT_TYPE					=4,
};

///\note we need a flag type to indicate GL-CL-interoperation buffers and CPU-memory-mapped buffers
enum ContextTypeFlags
{
	NO_CONTEXT_TYPE_FLAG				=0,
	HOST_CONTEXT_TYPE_FLAG				=1<<0,
	OPEN_CL_CONTEXT_TYPE_FLAG			=1<<1,
	OPEN_GL_CONTEXT_TYPE_FLAG			=1<<2
};





//-------------------------
//enumerate all non-texture- or render buffer types currently supported by the framework
enum GLBufferType
{
	NO_GL_BUFFER_TYPE = 0,
	VERTEX_ATTRIBUTE_BUFFER_TYPE = 1,
	VERTEX_INDEX_BUFFER_TYPE = 2,
	UNIFORM_BUFFER_TYPE = 3
};

enum BufferSemantics
{
	// following semantics generally used in
	//Generic Vertex Attribute Buffers and/or OpenCL (interop) Buffers
	///\{
	POSITION_SEMANTICS,
	NORMAL_SEMANTICS,
	TANGENT_SEMANTICS,
	//BINORMAL_SEMANTICS,
	TEXCOORD_SEMANTICS,


	VELOCITY_SEMANTICS,
	MASS_SEMANTICS,
	DENSITY_SEMANTICS,
	PRESSURE_SEMANTICS,
	FORCE_SEMANTICS,

	Z_INDEX_SEMANTICS,

	DIFFUSE_COLOR_SEMANTICS,

	CUSTOM_SEMANTICS,

	//we need tha value to have static-length arrays holding VBO maintainance information
	__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__,

	//Semantic of the index buffer from a VBO used via glDrawElements()
	INDEX_SEMANTICS,

	//for a uniform buffer for matrices of instanced rendering
	TRANSFORMATION_MATRICES_SEMANTICS,
	LIGHT_SOURCE_BUFFER_SEMANTICS,
	///\}

	//following texture-only semantics; Texture can also have the above Semantics

	DISPLACEMENT_SEMANTICS, //normal-depth or normal map
	ENVMAP_SEMANTICS,
	MATERIAL_ID_SEMANTICS,
	PRIMITIVE_ID_SEMANTICS,
	SHADOW_MAP_SEMANTICS,
	AMBIENT_OCCLUSION_SEMANTICS, //attenuation value as result of AO calculations on a depth/position buffer
	DETAIL_TEXTURE_SEMANTICS, //for stuff like terrain to hide low res decal texture
	NOISE_SEMANTICS,
	DEPTH_BUFFER_SEMANTICS, //e.g. for memory optimized ambient occlusion calculation
	STENCIL_BUFFER_SEMANTICS,
	INTERMEDIATE_RENDERING_SEMANTICS,
	FINAL_RENDERING_SEMANTICS,

	//actually real amount is one less than this valu, but that doesnt matter
	__NUM_TOTAL_SEMANTICS__,

	//indicator for "empty" stuff, e.g. an empty Color Attachment slot in an FBO
	INVALID_SEMANTICS
};




//-----------------------------------------------


class BufferException : public std::exception
{
	String mDescription;
 public:
	BufferException(String description = "unspecified Buffer exception") throw()
	: mDescription(description)
	{ }

	virtual ~BufferException() throw(){}

	virtual const char* what() const throw()
	{
	    return mDescription.c_str();
	}
};


enum GPU_DataType
{
	GPU_DATA_TYPE_FLOAT,
	GPU_DATA_TYPE_INT,
	GPU_DATA_TYPE_UINT
};


struct BufferElementInfo
{
	/*
	In this framework, it is strongly recommended to specify the desired internal layout:
	exactly, i.e. specify
		1. number of channels: 1,2 or 4; three channel types not supported by this framework due to alignment reasons;
		2. internal data format on GPU memory: float,int, unsigned int
		3. byte size per element: 8,16, or 32
		4. flag, if  unsigned integer data formats should be normalized to float in [0..1]
			resp.if    signed integer data formats should be normalized to float in [-1..1]

	The Constructors of especially the Texture classes will try to enforce that as few as
	possible invalid parameter permutations are able to specify;
	The user of this framework shall be freed of the headache, which GL-#define-permutations are
	valid and which not; Plus, he shall not have to think TOO much about interoperability
	of certain texture types; The flag "clInterop" will indicate, if the texture is sharable
	with CL; There is much less share-ability in OpenCL than in CUDA; Thus, there will be
	relatively few sharable texture types, namely

	Texture2D
	Texture2DRect
	Texture2DCube
	Texture3D	 <-- careful when writing in CL-kernel! CL-implementation must support this (can be queried)

	There is one minor drawback at the moment: CPU data must have the same format as the data which
	will reside on the GPU;

	Here is a table about the mapping from this BufferElementInfo struct to real
	GL "GLint internalFormat"  resp.
	CL  "cl_channel_type cl_image_format::image_channel_order":

	internal 	||	float		|	signed int	|	unsigned int  |unsigned   	|signed
	data type	||				|				| 				  |normalized 	|normalized

	conversion	||	float->float|	int->int	| 	uint->uint	  |uint->float	|int->float
	on lookup	||

	sampler 	||	(samplerXD)	|	(isamplerXD)| 	(usamplerXD)  |(samplerXD)	|(samplerXD)
	type		||				|				|				  |				|

	unsupported ||	8 bit 		|				|				  |32 bit 		| 32 bit
	bit size	||				|				|				  |				|

	OpenCL		||	CL_FLOAT	|CL_SIGNED_INT32|CL_UNSIGNED_INT32|--			|--
	channel 	||CL_HALF_FLOAT	|CL_SIGNED_INT16|CL_UNSIGNED_INT16|CL_UNORM_INT16|CL_SNORM_INT16
	data type	||	--			|CL_SIGNED_INT8	|CL_UNSIGNED_INT8 |CL_UNORM_INT8|CL_SNORM_INT8

	===========================================================================================

	four channel|| 	GL_RGBA32F,   GL_RGBA32I, 		GL_RGBA32UI,	--			--
	types:		||	GL_RGBA16F,   GL_RGBA16I,		GL_RGBA16UI,  	GL_RGBA16,	GL_RGBA16_SNORM
	CL_RGBA		||	--			  GL_RGBA8I, 		GL_RGBA8UI,		GL_RGBA8,	GL_RGBA8_SNORM


	two channel	|| GL_RG32F, 	  GL_RG32I, 		GL_RG32UI,		--			--
	types:		|| GL_RG16F,	  GL_RG16I, 		GL_RG16UI,  	GL_RG16,	GL_RG16_SNORM,
	CL_RG		|| --			  GL_RG8I,			GL_RG8UI,	 	GL_RG8,		GL_RG8_SNORM,

	one channel	||	GL_R32F, 	  GL_R32I, 			GL_R32UI,		--			--
	types:		||	GL_R16F, 	  GL_R16I, 			GL_R16UI, 		GL_R16,		GL_R16_SNORM
	CL_R		||	--			  GL_R8I, 	 		 GL_R8UI,		GL_R8,		GL_R8_SNORM

	*/


	//GLint  	texelDesiredinternalGPUFormat;


	//1.:
	int numChannels; 	//will be mapped internally to:
						//the "format" param of glTexImageXD:
						//GL_RED, 	GL_RG, 	GL_RGB (forbidden), 	GL_RGBA resp.
						//the  cl::ImageFormat.image_channel_order:
						//CL_R, 	CL_RG,	CL_RGB (forbidden), CL_RGBA
	//2.:
	GPU_DataType internalGPU_DataType;
	//3.
	int bitsPerChannel;	//8,16 or 32
	//4
	bool normalizeIntegralValuesFlag;

	//guard to tell the valiadte() function that this struct is unused
	//(e.g. for lightsource uniform buffers, generic opencl buffers etc);
	//is set to true in the default constructor,
	//copied in the copy constructor and set to false in the value-passing constrctor
	bool hasNoChanneledElements;


	//default constructor for internal image loading or if this struct is unneeded;
	//set the hasNoChanneledElements explicitely to force the programmer to think about what he is doing;
	//has to be set always to true with this constrctor, otherwise, validation will fail;
	explicit BufferElementInfo(bool hasNoChanneledElements);
	explicit BufferElementInfo(int numChannels,GPU_DataType internalGPU_DataType,int bitsPerChannel, bool normalizeIntegralValuesFlag);
	explicit BufferElementInfo(const BufferElementInfo& rhs);
	virtual ~BufferElementInfo(){}
	//const BufferElementInfo& operator=(const BufferElementInfo& rhs);
	bool operator==(const BufferElementInfo& rhs)const;
	const BufferElementInfo& operator=(const BufferElementInfo& rhs);
	//called by constructor to early detect invalid values and permutations, like 8-bit float or 32bit normalized (u)int
	void validate()const throw (BufferException);
};


//struct definition to make CL/GL-handling more symmetric;
struct GLImageFormat
{
	//"internalformat" paramater of glTexImageXD; specifies
	//	-	number of channels
	//	-	number of bits per channel
	//	-	data type of channel values (float, uint, int)
	//	-	if (u)int values shall be normalized;
	// All of this packed in one enum! Plus, this enum must comply with the channelOrder
	// and the channelDataType below! So much invalid combinations are possible!
	// Thus, the values of those enums are determined
	//valid values: see the big table within the comment of BufferElementInfo: GL_RGBA32F .. GL_R8_SNORM;
	GLint  	desiredInternalFormat;


	//"format" paramater of glTexImageXD resp. cl_channel_order:
	// texelInfo.numChannels == 1 --> GL_RED, 2 -->	GL_RG, 4 --> GL_RGBA
	GLenum channelOrder;
	//"type" parameter of glTexImageXD, i.e cpu data layout;
	//Compared to OpenCL, the fact if a (u)int- texel is normalized during texture lookup or not is
	//determined via the "internalformat" param and not together with the CPU data type specifier;
	//Therefore, only the following values are allowed;
	//	GL_FLOAT		|GL_INT		|GL_UNSIGNED_INT
	//	GL_HALF_FLOAT	|GL_SHORT   |GL_UNSIGNED_SHORT
	//	--				|GL_BYTE	|GL_UNSIGNED_BYTE
	GLenum	channelDataType;

	//default (invalid) =0 - params for initialization, as value computations must happen in a sophisticated
	//way in the constructor _body_ ;(
	explicit GLImageFormat(
			GLint  	desiredInternalFormat =0 ,
			GLenum channelOrder = 0,
			GLenum	channelDataType =0	);
	explicit GLImageFormat(const GLImageFormat& rhs);
	virtual ~GLImageFormat(){}
	bool operator==(const GLImageFormat& rhs) const;
	const GLImageFormat& operator=(const GLImageFormat& rhs);
};




/**
 * some partially redundant information about the buffer;
 */
class BufferInfo
{
public:
	String name;
	ContextTypeFlags usageContexts;
	BufferSemantics bufferSemantics;


	Type elementType; //default TYPE_UNDEF
	cl_GLuint numElements;


	//"clean" user provided info about the texels in the texture, or the components and types in a vertex attribute/index buffer;
	BufferElementInfo elementInfo;
	//guard if some buffer is mapped

	GLBufferType glBufferType; //default NO_GL_BUFFER_TYPE; must have other value if the GL-flag is set in usageContexs
	bool isPingPongBuffer; //default false, set by the appropriate class;

	ContextType mappedToCPUContext; //default NO_CONTEXT_TYPE, valid only the latter and OPEN_CL_CONTEXT_TYPE and OPEN_GL_CONTEXT_TYPE


	//value automatically calulated by  numElements * BufferHelper::elementSize(elementType);
	cl_GLuint bufferSizeInByte;

	//default constructor for non-image buffers
	explicit BufferInfo(String name,
			ContextTypeFlags usageContexts,
			BufferSemantics bufferSemantics,
			Type elementType,
			cl_GLuint numElements,
			const BufferElementInfo& elementInfo,
			GLBufferType glBufferType = NO_GL_BUFFER_TYPE,
			ContextType mappedToCPUContext = NO_CONTEXT_TYPE);



	BufferInfo(const BufferInfo& rhs);

	virtual ~BufferInfo();
	bool operator==(const BufferInfo& rhs) const;
	const BufferInfo& operator=(const BufferInfo& rhs);


	//add lean contructor for the cases where the concrete values must be computed later, but things
	//like the name etc must already be known; useful for TextureInfo
	explicit BufferInfo(String name,
				ContextTypeFlags usageContexts,
				BufferSemantics bufferSemantics,
				const BufferElementInfo& elementInfo);

};




/**
 * container class to contain all relevant GL and CL-specific enums, flags, state etc.
 * so that it can be queried easily by the user;
 * The values are set by the concrte texture constructors themseves;
 */

class TextureInfo:
	public BufferInfo
 {
public:
	///\{
	//general info
	cl_GLuint dimensionality; //interesting for textures: 1,2 or 3 dimensions;
	Vector3Dui dimensionExtends; //must be zero for unused dimensions;
	///\}

	//"clean" user provided info about the texels in the texture;
	//BufferElementInfo elementInfo;

	//texture target, automatically determined by the constructors of the concrete Texture classes;

	GLenum textureTarget; //e.g. GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_2D_MULTISAMPLE_ARRAY etc.



	//Flags/values indicating special features, depending on the concrete texture types
	///\{
	TextureType textureType;

	bool isDepthTexture;	//default false,
	bool isMipMapped;		//default false;
	bool isRectangleTex;	//default false;
	bool isCubeTex;			//default false;

	GLint numMultiSamples; 	 //default 1 to indicate no multisampling
	GLint numArrayLayers;	 //default 1 to indicate no array stuff
	///\}

	//automatically determined values; only needed for internal GL/CL calls:


	GLImageFormat glImageFormat;
	//for clImageFormat.image_channel_order: texelInfo.numChannels == 1 --> CL_R, 2 -->	CL_RG, 4 --> CL_RGBA
	//for clImageFormat.image_channel_data_type	: depending on
	//		texelInfo.internalGPU_DataType,texelInfo.bitsPerChannel, texelInfo.isNormalizedFlag;
	//valid values in this framework, see BufferElementInfo (there are more valid values in OpenCL,
	//but I don't explicitly support them):
	//	CL_FLOAT		|CL_SIGNED_INT32|CL_UNSIGNED_INT32|--			|--
	//	CL_HALF_FLOAT	|CL_SIGNED_INT16|CL_UNSIGNED_INT16|CL_UNORM_INT16|CL_SNORM_INT16
	//	--				|CL_SIGNED_INT8	|CL_UNSIGNED_INT8 |CL_UNORM_INT8|CL_SNORM_INT8
	cl::ImageFormat clImageFormat;



	explicit TextureInfo(
			const BufferInfo& buffi,
			cl_GLuint dimensionality,
			Vector3Dui dimensionExtends,
			//const BufferElementInfo& texelInfo,
			GLenum textureTarget,
			bool isDepthTexture = false,
			bool isMipMapped = false,
			bool isRectangleTex = false,
			bool isCubeTex = false,
			GLint numMultiSamples = 1,
			GLint numArrayLayers = 1
			)throw(BufferException);

	explicit TextureInfo(const TextureInfo& rhs)throw(BufferException);

	virtual ~TextureInfo(){}
	bool operator==(const TextureInfo& rhs) const;
	const TextureInfo& operator=(const TextureInfo& rhs);

	bool isRenderTargetCompatibleTo(const TextureInfo& rhs)const;

	bool calculateCLGLImageFormatValues()throw (BufferException);
 };



//enum GLRenderBufferType
//{
//	RENDER_DEPTH_BUFFER_TYPE =0,
//	RENDER_STENCIL_BUFFER_TYPE =1
//};
//--------------------------



}


/*
 * What  follows is a structure for coordinating buffer stuff which will be frequently used;
 * So, copypaste might be useful sometimes... (I don't know a boilerplate-free way yet ;( )
 *
 * POSSIBLE OBSOLTE TODO DELETE LATER
 *

 	//CPU
	if( where & HOST_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo->usageContexts & HOST_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to cpu buffer requested, but this buffer has no CPU storage!"));}

		//TODO

	}

	//GL
	if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo->usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to GL buffer requested, but this buffer has no GL storage!"));}

		//TODO
	}

	//CL
	if( mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo->usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to CL buffer requested, but this buffer has no CL storage!"));}


		//interop
		if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
		{
			//storage has already been set by the GL context; do nothing
		}
		//pure CL
		else
		{
			//TODO
		}
	}

 */

