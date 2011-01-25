/*
 * BufferHelperUtils.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */

#include "BufferHelperUtils.h"

namespace Flewnit
{

bool BufferHelper::isPowerOfTwo(int value)
{
	if(value <0 ) return false;

	int cnt=0;
	for (unsigned int i=0;i< sizeof(int)*8 ; i++)
	{
		if( ( value & (1<<i) ) !=0) cnt++;
	}
	return (cnt <= 1);
}



String BufferHelper::BufferSemanticsToString(BufferSemantics bs)
{
	static const String BufferSemanticsStrings[] =
	{
		//generally semantics of VertexBuffers, partially OpenCL Buffers (shared)
		"POSITION_SEMANTICS",
		"NORMAL_SEMANTICS",
		"TANGENT_SEMANTICS",
		//"BINORMAL_SEMANTICS",
		"TEXCOORD_SEMANTICS",
		"INDEX_SEMANTICS",

		"VELOCITY_SEMANTICS",
		"MASS_SEMANTICS",
		"DENSITY_SEMANTICS",
		"PRESSURE_SEMANTICS",
		"FORCE_SEMANTICS",

		"Z_INDEX_SEMANTICS",

		//generally texture semantics
		"DECAL_COLOR_SEMANTICS",
		"DISPLACEMENT_SEMANTICS",
		"ENVMAP_SEMANTICS",
		"MATERIAL_ID_SEMANTICS",
		"PRIMITIVE_ID_SEMANTICS",
		"SHADOW_MAP_SEMANTICS",
		"AMBIENT_OCCLUSION_SEMANTICS",
		"NOISE_SEMANTICS",
		"DEPTH_BUFFER_SEMANTICS",
		"STENCIL_BUFFER_SEMANTICS",
		"INTERMEDIATE_RENDERING_SEMANTICS",
		"FINAL_RENDERING_SEMANTICS",

		//for a uniform buffer for matrices of instanced rendering
		"TRANSFORMATION_MATRICES_SEMANTICS",

		"CUSTOM_SEMANTICS"
	};

	return BufferSemanticsStrings[bs];
}


}
