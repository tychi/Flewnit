/*
 * Texture2D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

namespace Flewnit
{
Texture2D::Texture2D(String name, BufferSemantics bufferSemantics, bool allocHostMemory,
		int width, int height, const TexelInfo& texeli, bool clInterOp, const void* data,  bool genMipmaps)

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
		2,
		Vector3Dui(width,height,1),
		texeli,
		GL_TEXTURE_2D,
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


Texture2D::~Texture2D()
{

}

bool Texture2D::operator==(const BufferInterface& rhs) const
{

}

void Texture2D::generateCLGL()throw(BufferException)
{

}
void Texture2D::allocGL()throw(BufferException)
{

}
void Texture2D::writeGL(const void* data)throw(BufferException)
{

}


}
