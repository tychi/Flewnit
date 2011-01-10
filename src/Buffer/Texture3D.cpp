/*
 * Texture3D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

namespace Flewnit
{
Texture3D::Texture3D(String name, BufferSemantics bufferSemantics, bool allocHostMemory,
		int width, int height, int depth, const TexelInfo& texeli, bool clInterOp, const void* data,  bool genMipmaps)

:
Texture	(
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(
					(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
					| OPEN_GL_CONTEXT_TYPE_FLAG
					| (clInterOp ? OPEN_CL_CONTEXT_TYPE_FLAG : NO_CONTEXT_TYPE_FLAG )
			),
			bufferSemantics
		),
		3,
		Vector3Dui(width,height,depth),
		texeli,
		GL_TEXTURE_3D,
		genMipmaps,
		false,
		false,
		0,
		0
	)
)
{
	//TODO from here whole file
	allocMem();

	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}

}


Texture3D::~Texture3D()
{

}

bool Texture3D::operator==(const BufferInterface& rhs) const
{

}

void Texture3D::generateCLGL()throw(BufferException)
{

}
void Texture3D::allocGL()throw(BufferException)
{

}
void Texture3D::writeGL(const void* data)throw(BufferException)
{

}



}
