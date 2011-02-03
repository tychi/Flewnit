/*
 * Texture3D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

#include "Simulator/OpenCL_Manager.h"

namespace Flewnit
{
Texture3D::Texture3D(String name, BufferSemantics bufferSemantics,
		int width, int height, int depth, const TexelInfo& texeli,
		bool allocHostMemory, bool clInterOp, const void* data,  bool genMipmaps)

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
}

Texture3D::Texture3D(const TextureInfo& texi, const void* data)
:Texture(texi)
{
	allocMem();

	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}
}



Texture3D::~Texture3D()
{
	//nothing to do, base classes do the rest ;)
}

bool Texture3D::operator==(const BufferInterface& rhs) const
{
	const Texture3D* rhsTexPtr = dynamic_cast<const Texture3D*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}



void Texture3D::generateCLGL()throw(BufferException)
{
	mComputeBufferHandle = cl::Image3DGL(
			CLMANAGER->getCLContext(),
			CL_MEM_READ_WRITE,
			mTextureInfoCastPtr->textureTarget,
			0,
			mGraphicsBufferHandle,
			& CLMANAGER->getLastCLError()
	);
}

void Texture3D::allocGL()throw(BufferException)
{
	glTexImage3D(
			mTextureInfoCastPtr->textureTarget,
			0,
			mTextureInfoCastPtr->glImageFormat.desiredInternalFormat,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
			mTextureInfoCastPtr->dimensionExtends.z,
			0,
			mTextureInfoCastPtr->glImageFormat.channelOrder,
			mTextureInfoCastPtr->glImageFormat.channelDataType,
			//don't set data yet, just alloc mem
			0
	);

}
void Texture3D::writeGL(const void* data)throw(BufferException)
{
	glTexSubImage3D(
			mTextureInfoCastPtr->textureTarget,
			0,
			0,0,0,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
			mTextureInfoCastPtr->dimensionExtends.z,
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
