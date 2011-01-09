/*
 * Texture1D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

namespace Flewnit
{

Texture1D::Texture1D(String name, BufferSemantics bufferSemantics, bool allocHostMemory,
		int width, const TexelInfo& texeli, const void* data,  bool genMipmaps)
:
Texture	(
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags( (allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG ) | OPEN_GL_CONTEXT_TYPE_FLAG ),
			bufferSemantics
		),
		1,
		Vector3Dui(width,0,0),
		texeli,
		genMipmaps,
		false,
		false,
		0,
		0
	)
)
{

}

Texture1D::Texture1D(String name, BufferSemantics bufferSemantics, bool allocHostMemory,
		Path fileName,  bool genMipmaps)
:
		Texture	(
			TextureInfo(
				BufferInfo(
					name,
					ContextTypeFlags( (allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG ) | OPEN_GL_CONTEXT_TYPE_FLAG ),
					bufferSemantics
				),
				1,
				//default invalid
				Vector3Dui(0,0,0),
				//default invalid
				TexelInfo(),
				GL_TEXTURE_1D,
				genMipmaps,
				false,
				false,
				0,
				0
			)
		)
{

}

Texture1D::~Texture1D()
{

}

bool Texture1D::operator==(const BufferInterface& rhs) const
{

}

//throw exception due to non-interoperability
void Texture1D::generateCLGL()throw(BufferException)
{

}
void Texture1D::allocGL()throw(BufferException)
{

}
void Texture1D::writeGL(const void* data)throw(BufferException)
{

}

}
