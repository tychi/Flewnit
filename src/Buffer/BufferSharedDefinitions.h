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


enum BufferTypeFlags
{
	EMPTY_BUFFER_FLAG				=0,
	CPU_BUFFER_FLAG  				=1<<0,

	VERTEX_ATTRIBUTE_BUFFER_FLAG 	=1<<1,
	VERTEX_INDEX_BUFFER_FLAG		=1<<2,
	//the interface of this framework doesn't distinguish between "pure" openCL-image objects and cl-gl-interop-images;
	//usually, the cl-gl-interop-stuff will be used;
	OPEN_CL_BUFFER_FLAG				=1<<3,

	//there is one big flaw in openCL compared to CUDA:
	// 1. casting between image objects and buffer objects won't work without copying every time;
	// 2. there is no OpenCL-1D-image object,
	// Thus, a (1D)-CL-Buffer can't profit from the texture cache of the GPU for even two reasons!
	// Let's hope that the fermi architecture has made the "buffer-as-texture"-performance-optimazation-hack obsolete ;(
	TEXTURE_1D_BUFFER_FLAG			=1<<4,
	TEXTURE_2D_BUFFER_FLAG			=1<<5,
	TEXTURE_3D_BUFFER_FLAG			=1<<6,
	RENDER_BUFFER_FLAG				=1<<7
};

enum ContextType
{
	HOST_CONTEXT_TYPE				=0,
	OPEN_CL_CONTEXT_TYPE			=1,
	OPEN_GL_CONTEXT_TYPE			=2,
	__NUM_CONTEXT_TYPES__			=3
};

///\note we need a flag type to indicate GL-CL-interoperation buffers and CPU-memory-mapped buffers
enum ContextTypeFlags
{
	NO_CONTEXT_TYPE_FLAG				=0,
	HOST_CONTEXT_TYPE_FLAG				=1<<0,
	OPEN_CL_CONTEXT_TYPE_FLAG			=1<<1,
	OPEN_GL_CONTEXT_TYPE_FLAG			=1<<2
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
