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


namespace Flewnit
{

enum BufferTypeFlags
{
	CPU_BUFFER_FLAG  				=1<<0,
	TEXTURE_BUFFER_FLAG				=1<<1,
	VERTEX_ATTRIBUTE_BUFFER_FLAG 	=1<<2,
	VERTEX_INDEX_BUFFER_FLAG		=1<<3,
	OPEN_CL_BUFFER_FLAG				=1<<4
};

enum ContextType
{
	HOST_CONTEXT_TYPE				=0,
	OPEN_GL_CONTEXT_TYPE			=1,
	OPEN_CL_CONTEX_TYPE				=2
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
	RENDERDED_IMAGE_SEMANTICS,

	PARTICLE_VELOCITY_SEMANTICS,
	PARTICLE_MASS_SEMANTICS,
	PARTICLE_DENSITY_SEMANTICS,
	PARTICLE_PRESSURE_SEMANTICS,
	PARTICLE_FORCE_SEMANTICS,

	Z_INDEX_SEMANTICS,

	CUSTOM_SEMANTICS
};

}
