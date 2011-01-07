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

enum TextureType
{
	NO_TEXTURE_BUFFER_TYPE 		=0,
	TEXTURE_1D_BUFFER_TYPE		=1,
	TEXTURE_2D_BUFFER_TYPE		=2,
	TEXTURE_3D_BUFFER_TYPE		=3
};

enum TextureFeatureFlags
{

	/**
	 * mutual exclusive with:
	 *   ARRAY_TEXTURE_FLAG  <-- see above
	 *   RECTANGLE_TEXTURE_FLAG <-- mipmapping on non-square-power-of-2-textures is useless ;(
	 *	 MULTISAMPLE_TEXTURE_FLAG <-- as no filtering is defined on
	 *
	 * NOT mutual exclusive with:
	 *   OPEN_CL_CONTEXT_TYPE_FLAG <-- but be careful to use only mipmap level 0 for sharing, as there might be driver bugs
	 *	 CUBE_MAP_TEXTURE_FLAG 		<--filtering appropriate;
	 */
	MIPMAP_TEXTURE_FLAG = 1<<0,


	/**
	 * mutual exclusive with:
	 * 		TEXTURE_3D_BUFFER_TYPE, <-- array of 2d is kind of 3d texture "internally", hence array of 3d doesn't exist
	 *
	 * 		OPEN_CL_CONTEXT_TYPE_FLAG, <-- sharing texture arrays with OpenCL is not allowed (afaik)
	 *
	 * 		MIPMAP_TEXTURE_FLAG, 	<-- array types "occupy" the logical mip map layers, so mip mapping is impossible
	 * 		RECTANGLE_TEXTURE_FLAG	<-- Rectangle Tex Arrays seem not be defined in GL
	 * 		CUBE_MAP_TEXTURE_FLAG	<-- in GL3.3, in GL4, there is a  GL_TEXTURE_CUBE_MAP_ARRAY target
	 *
	 * NOT mutual exclusive with:
	 * 		MULTISAMPLE_TEXTURE_FLAG && TEXTURE_2D_BUFFER_TYPE <--  only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
	 *
	 */
	ARRAY_TEXTURE_FLAG = 1<<1,

	/**
	 * mutual exclusive with:
	 * 	TEXTURE_1D_BUFFER_TYPE
	 *
	 *	CUBE_MAP_TEXTURE_FLAG,	<--	no defined by GL (afaik)
	 *	RECTANGLE_TEXTURE_FLAG, <-- no defined by GL (afaik)
	 *	MIPMAP_TEXTURE_FLAG
	 *
	 *	OPEN_CL_CONTEXT_TYPE_FLAG <-- not defined in CL (afaik)
	 *
	 * NOT mutual exclusive with:
	 * 	ARRAY_TEXTURE_FLAG	<--but only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
	 *
	 */
	MULTISAMPLE_TEXTURE_FLAG = 1<<2,


	/**
	 * mutual exclusive with:
	 * everything but:
	 *
	 * NOT mutual exclusive with:
	 * 	TEXTURE_2D_BUFFER_TYPE
	 * 	MIPMAP_TEXTURE_FLAG
	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
	 *
	 */
	CUBE_MAP_TEXTURE_FLAG = 1<<3,


	/**
	 * mutual exclusive with:
	 * everything but:
	 *
	 * NOT mutual exclusive with:
	 * 	TEXTURE_2D_BUFFER_TYPE
	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
	 *
	 */
	RECTANGLE_TEXTURE_FLAG = 1<<4,
};

/*
	MipMap
	{
		//no multisample with MipMap "per definitionem" ;)

		Array:
		{
			Texture1DArray(bool genMipmaps,int numLayers)
			Texture2DArray(bool genMipmaps,int numLayers)
		}
		no Array:
		{
			Texture1D		(bool genMipmaps);	//ocl bindung not supported :@

			Texture2D		(bool genMipmaps, bool oclbinding);
			Texture2DCube	(bool genMipmaps, bool oclbinding); // array (in GL 3.3), multisample forbidden

			Texture3D		(bool genMipmaps, bool oclbinding); //array, multisample forbidden (2D tex-arrays logically don't count as 3D tex)
		}
	}
	no MipMap:
	{
		Array:
		{
			MultiSample:
			{
				Texture2DArrayMultiSample(int numMultiSamples, int numLayers)
			}

		}
		no Array
		{
			MultiSample:
			{
				Texture2DMultisample( int numMultiSamples);
				Texture3DMultisample( int numMultiSamples);
			}
			no MultiSample
			{
				Texture2DRect	(bool oclbinding); //mipmap, array, multisample forbidden
			}
		}
	}
*/

;

enum GLRenderBufferType
{
	RENDER_DEPTH_BUFFER_TYPE =0,
	RENDER_STENCIL_BUFFER_TYPE =1
};
//--------------------------





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

}


/*
 * What  follows is a structure for coordinating buffer stuff which will be frequently used;
 * So, copypaste might be useful sometimes... (I don't know a boilerplate-free way yet ;( )
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

