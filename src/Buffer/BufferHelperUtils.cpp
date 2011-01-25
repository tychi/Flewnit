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
			// following semantics generally used in
			//Generic Vertex Attribute Buffers and/or OpenCL (interop) Buffers
			///\{
			"POSITION_SEMANTICS",
			"NORMAL_SEMANTICS",
			"TANGENT_SEMANTICS",
			//"BINORMAL_SEMANTICS",
			"TEXCOORD_SEMANTICS",

			"VELOCITY_SEMANTICS",
			"MASS_SEMANTICS",
			"DENSITY_SEMANTICS",
			"PRESSURE_SEMANTICS",
			"FORCE_SEMANTICS",

			"Z_INDEX_SEMANTICS",

			"DECAL_COLOR_SEMANTICS",

			"CUSTOM_SEMANTICS",

			//we need tha value to have static-length arrays holding VBO maintainance information
			"__NUM_VALID_VERTEX_ATTRIBUTE_SEMANTICS__",

			//Semantic of the index buffer from a VBO used via glDrawElements()
			"INDEX_SEMANTICS",

			//for a uniform buffer for matrices of instanced rendering
			"TRANSFORMATION_MATRICES_SEMANTICS",
			///\}

			//following texture-only semantics; Texture can also have the above Semantics

			"DISPLACEMENT_SEMANTICS", //normal-depth or normal map
			"ENVMAP_SEMANTICS",
			"MATERIAL_ID_SEMANTICS",
			"PRIMITIVE_ID_SEMANTICS",
			"SHADOW_MAP_SEMANTICS",
			"AMBIENT_OCCLUSION_SEMANTICS",
			"NOISE_SEMANTICS",
			"DEPTH_BUFFER_SEMANTICS",
			"STENCIL_BUFFER_SEMANTICS",
			"INTERMEDIATE_RENDERING_SEMANTICS",
			"FINAL_RENDERING_SEMANTICS"
	};

	return BufferSemanticsStrings[bs];
}


}
