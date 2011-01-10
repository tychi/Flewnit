/*
 * Texture2D.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"

#include "Simulator/OpenCL_Manager.h"

#include "Util/Log/Log.h"

namespace Flewnit
{
Texture2D::Texture2D(String name, BufferSemantics bufferSemantics,
		int width, int height, const TexelInfo& texeli,
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

	allocMem();

	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}

}

//protected constructor tobe called by Texture2DDepth:
Texture2D:: Texture2D(String name,
			int width, int height,
			bool allocHostMemory, bool clInterOp)
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
				SHADOW_MAP_SEMANTICS
			),
			2,
			Vector3Dui(width,height,1),
			TexelInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
			GL_TEXTURE_2D,
			false,
			false,
			false,
			0,
			0
		)
	)
{
	mTextureInfoCastPtr->glImageFormat.desiredInternalFormat = GL_DEPTH_COMPONENT32;
	mTextureInfoCastPtr->glImageFormat.channelOrder= GL_DEPTH_COMPONENT;
	//the rest of the values stay the same;

	allocMem();

	//don't set data to the depth texture;
}



Texture2D::~Texture2D()
{
	//nothing to do, base classes do the rest ;)
}

bool Texture2D::operator==(const BufferInterface& rhs) const
{
	const Texture2D* rhsTexPtr = dynamic_cast<const Texture2D*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}



//-----------------------------------------------------------------------
Texture2DDepth::Texture2DDepth(String name,
			int width, int height,
			bool allocHostMemory, bool clInterOp)
: Texture2D(name,width,height,allocHostMemory,clInterOp)
{
	LOG<<INFO_LOG_LEVEL << "Creating 2D depth texture named "<< name <<";\n";
}

Texture2DDepth::~Texture2DDepth()
{
	//nothing to do, base classes do the rest ;)
}

bool Texture2DDepth::operator==(const BufferInterface& rhs) const
{
	const Texture2DDepth* rhsTexPtr = dynamic_cast<const Texture2DDepth*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}


//------------------------------------------------------------------------




void Texture2D::generateCLGL()throw(BufferException)
{
	mComputeBufferHandle = cl::Image2DGL(
			CLMANAGER->getCLContext(),
			CL_MEM_READ_WRITE,
			mTextureInfoCastPtr->textureTarget,
			0,
			mGraphicsBufferHandle,
			& CLMANAGER->getLastCLError()
	);
}

void Texture2D::allocGL()throw(BufferException)
{
	glTexImage2D(
			mTextureInfoCastPtr->textureTarget,
			0,
			mTextureInfoCastPtr->glImageFormat.desiredInternalFormat,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
			0,
			mTextureInfoCastPtr->glImageFormat.channelOrder,
			mTextureInfoCastPtr->glImageFormat.channelDataType,
			//don't set data yet, just alloc mem
			0
	);

}
void Texture2D::writeGL(const void* data)throw(BufferException)
{
	glTexSubImage2D(
			mTextureInfoCastPtr->textureTarget,
			0,
			0,0,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
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


void Texture2DDepth::allocGL()throw(BufferException)
{
	Texture2D::allocGL();

	//TODO handle this more professionally via sampler objects within material/shader...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    float fourZeros[] = {0.0f,0.0f,0.0f,0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, fourZeros);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


}
