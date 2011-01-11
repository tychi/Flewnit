/*
 * Texture1D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

namespace Flewnit
{

Texture1D::Texture1D(String name, BufferSemantics bufferSemantics,
		int width, const TexelInfo& texeli,
		bool allocHostMemory, const void* data,  bool genMipmaps)
:
Texture	(
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags( (allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG ) | OPEN_GL_CONTEXT_TYPE_FLAG ),
			bufferSemantics
		),
		1,
		Vector3Dui(width,1,1),
		texeli,
		GL_TEXTURE_1D,
		genMipmaps,
		false,
		false,
		0,
		0
	)
)
{
	allocMem();

	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}
}

//Texture1D::Texture1D(String name, BufferSemantics bufferSemantics, bool allocHostMemory,
//		Path fileName,  bool genMipmaps)
//:
//		Texture	(
//			TextureInfo(
//				BufferInfo(
//					name,
//					ContextTypeFlags( (allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG ) | OPEN_GL_CONTEXT_TYPE_FLAG ),
//					bufferSemantics
//				),
//				1,
//				//default invalid
//				Vector3Dui(0,0,0),
//				//default invalid
//				TexelInfo(),
//				GL_TEXTURE_1D,
//				genMipmaps,
//				false,
//				false,
//				0,
//				0
//			)
//		)
//{
//
//}

Texture1D::~Texture1D()
{

}

bool Texture1D::operator==(const BufferInterface& rhs) const
{
	const Texture1D* rhsTexPtr = dynamic_cast<const Texture1D*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}



void Texture1D::allocGL()throw(BufferException)
{
	glTexImage1D(
			mTextureInfoCastPtr->textureTarget,
			0,
			mTextureInfoCastPtr->glImageFormat.desiredInternalFormat,
			mTextureInfoCastPtr->dimensionExtends.x,
			0,
			mTextureInfoCastPtr->glImageFormat.channelOrder,
			mTextureInfoCastPtr->glImageFormat.channelDataType,
			//don't set data yet, just alloc mem
			0
	);
}
void Texture1D::writeGL(const void* data)throw(BufferException)
{
	glTexSubImage1D(
			mTextureInfoCastPtr->textureTarget,
			0,
			0,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->glImageFormat.channelOrder,
			mTextureInfoCastPtr->glImageFormat.channelDataType,
			//don't set data yet, just alloc mem
			data
	);

	if(mTextureInfoCastPtr->isMipMapped)
	{
		glGenerateMipmap( mTextureInfoCastPtr->textureTarget );
	}
}




}
