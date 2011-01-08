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

#include "Common/CL_GL_Common.h"

#include "Common/Math.h"


namespace Flewnit
{

typedef void* CPUBufferHandle;
typedef GLuint GraphicsBufferHandle;
typedef cl::Memory ComputeBufferHandle;

class BufferInfo;
class TextureInfo;

//abstract base class for all Buffers;
class BufferInterface;

//kind of wrapper for two buffers to be toggled for read/write access;
class PingPongBuffer;

//class for all buffers but textures and render buffers;
class Buffer;

//abstract base class for the several concrete-dimensioned textures;
class Texture;
//concrete stuff;
class Texture1D;
class Texture2D;
class Texture3D;

//the special class for offscreen rendering: kinda texture without filtering stuff for depth and stencil operations;
class RenderBuffer;



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
	POSITION_SEMANTICS,
	NORMAL_SEMANTICS,
	TANGENT_SEMANTICS,
	BINORMAL_SEMANTICS,
	TEXCOORD_SEMANTICS,

	DECAL_COLOR_SEMANTICS,
	DISPLACEMENT_SEMANTICS,
	MATERIAL_ID_SEMANTICS,
	PRIMITIVE_ID_SEMANTICS,
	SHADOW_MAP_SEMANTICS,
	AMBIENT_OCCLUSION_SEMANTICS,
	NOISE_SEMANTICS,

	//for a uniform buffer for matrices of instanced rendering
	TRANSFORMATION_MATRICES_SEMANTICS,

	INTERMEDIATE_RENDERING_SEMANTICS,

	PARTICLE_VELOCITY_SEMANTICS,
	PARTICLE_MASS_SEMANTICS,
	PARTICLE_DENSITY_SEMANTICS,
	PARTICLE_PRESSURE_SEMANTICS,
	PARTICLE_FORCE_SEMANTICS,

	Z_INDEX_SEMANTICS,

	CUSTOM_SEMANTICS
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

	GLBufferType glBufferType; //default NO_GL_BUFFER_TYPE; must have other value if the GL-flag is set in usageContexs
	bool isPingPongBuffer; //default false, set by the appropriate class;
	//guard if some buffer is mapped
	ContextType mappedToCPUContext; //default NO_CONTEXT_TYPE, valid only the latter and OPEN_CL_CONTEXT_TYPE and OPEN_GL_CONTEXT_TYPE

	//value automatically calulated by  numElements * BufferHelper::elementSize(elementType);
	cl_GLuint bufferSizeInByte;

	explicit BufferInfo(String name,
			ContextTypeFlags usageContexts,
			BufferSemantics bufferSemantics,
			Type elementType,
			cl_GLuint numElements,
			GLBufferType glBufferType = NO_GL_BUFFER_TYPE,
			ContextType mappedToCPUContext = NO_CONTEXT_TYPE);
	BufferInfo(const BufferInfo& rhs);
	virtual ~BufferInfo();
	bool operator==(const BufferInfo& rhs) const;
	const BufferInfo& operator=(const BufferInfo& rhs);

};


enum GPU_DataType
{
	GPU_DATA_TYPE_FLOAT,
	GPU_DATA_TYPE_INT,
	GPU_DATA_TYPE_UINT
};


struct TexelInfo
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


	Here is a table about the mapping from this TexelInfo struct to real GL and CL enums:

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
	CL_R		||	GL_R8I, 	  GL_R8UI,		GL_R8,		GL_R8_SNORM

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
	bool isNormalizedFlag;

	TexelInfo(int numChannels,GPU_DataType internalGPU_DataType,int bitsPerChannel, bool isNormalizedFlag);
	TexelInfo(const TexelInfo& rhs);
	virtual ~TexelInfo(){}
	void operator==(const TexelInfo& rhs);
	const TexelInfo& operator=(const TexelInfo& rhs);
	void validate() throw (BufferException);
};


/**
 * container class to contain all relevant GL and CL-specific enums, flags, state etc.
 * so that it can be queried easily by the user;
 * The values are set by the concrte texture constructors themseves;
 */

class TextureInfo
 {
public:
	cl_GLuint dimensionality; //interesting for textures: 1,2 or 3 dimensions;
	Vector3Dui dimensionExtends; //must be zero for unused dimensions;


	GLenum textureTarget; //GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_2D_MULTISAMPLE_ARRAY e
	GLint imageInternalChannelLayout; //usually GL_RGBA or GL_LUMINANCE
	GLenum imageInternalDataType;	//usually GL_UNSIGNED_INT or GL_FLOAT

	GLint numMultiSamples; 	 //default 0 to indicate no multisampling
	GLint numArrayLayers;	 //default 0 to indicate no array stuff


	bool isMipMapped;		//default false;
	bool isRectangleTex;	//default false;
	bool isCubeTex;			//default false;

private:


	explicit TextureInfo(
			GLenum textureTarget,
			GLint imageInternalChannelLayout,
			GLenum imageInternalDataType
			);
	TextureInfo(const TextureInfo& rhs);
	virtual ~TextureInfo();
	bool operator==(const TextureInfo& rhs) const;
	const TextureInfo& operator=(const TextureInfo& rhs);
 };



enum GLRenderBufferType
{
	RENDER_DEPTH_BUFFER_TYPE =0,
	RENDER_STENCIL_BUFFER_TYPE =1
};
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
		if( ! (mBufferInfo.usageContexts & HOST_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to cpu buffer requested, but this buffer has no CPU storage!"));}

		//TODO

	}

	//GL
	if( where & OPEN_GL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & OPEN_GL_CONTEXT_TYPE_FLAG))
		{throw(BufferException("data copy to GL buffer requested, but this buffer has no GL storage!"));}

		//TODO
	}

	//CL
	if( mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG )
	{
		if( ! (mBufferInfo.usageContexts & OPEN_CL_CONTEXT_TYPE_FLAG))
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

