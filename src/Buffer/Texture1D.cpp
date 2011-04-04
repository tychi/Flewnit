/*
 * Texture1D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"
#include "Simulator/ParallelComputeManager.h"

namespace Flewnit
{

Texture1D::Texture1D(String name, BufferSemantics bufferSemantics,
		int width, const BufferElementInfo& texeli,
		bool allocHostMemory, const void* data,  bool genMipmaps)
:
Texture	(
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags( (allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG ) | OPEN_GL_CONTEXT_TYPE_FLAG ),
			bufferSemantics,
			texeli
		),
		1,
		Vector3Dui(width,1,1),
		//texeli,
		GL_TEXTURE_1D,
		false, //no depth tex
		genMipmaps,
		false,
		false,
		1,
		1
	)
)
{
	allocMem();

	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}

	setupDefaultSamplerParameters();
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
//				BufferElementInfo(),
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


void Texture1D::setupDefaultSamplerParameters()
{
	GUARD(bindGL());

	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S,
			GL_REPEAT));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE));

	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MAG_FILTER,
			GL_LINEAR));

	if(mTextureInfoCastPtr->isMipMapped)
	{
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR));
	}
	else
	{
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR));
	}

	GUARD(glTexParameterfv(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_BORDER_COLOR,
			& Vector4D(0.0f,0.0f,0.0f,0.0f)[0]));
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



//------------------------------------------------------------------------------------

Texture1DArray::Texture1DArray(String name, BufferSemantics bufferSemantics,
		int width, int numLayers,  const BufferElementInfo& texeli,
		 bool allocHostMemory, const void* data,  bool genMipmaps)
:
	Texture2D	(
		TextureInfo(
			BufferInfo(
				name,
				ContextTypeFlags(
						(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
						| OPEN_GL_CONTEXT_TYPE_FLAG
				),
				bufferSemantics,
				texeli
			),
			1,
			Vector3Dui(width,1,1),
			//texeli,
			GL_TEXTURE_1D_ARRAY,
			false,
			genMipmaps,
			false,
			false,
			1,
			numLayers
		),
		data
	)
{

}

Texture1DArray::~Texture1DArray()
{
	//do nothing
}

bool Texture1DArray::operator==(const BufferInterface& rhs) const
{
	const Texture1DArray* rhsTexPtr = dynamic_cast<const Texture1DArray*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}




}
