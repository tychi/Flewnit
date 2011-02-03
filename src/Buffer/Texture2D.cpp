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
		bool allocHostMemory, bool clInterOp,
		bool makeRectangleTex, const void* data,  bool genMipmaps)
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
		makeRectangleTex ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D,
		false,
		makeRectangleTex ? false : genMipmaps,
		makeRectangleTex,
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

//protected constructor to be called by Texture2DDepth:
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
			true, //yes, is depth tex
			false,
			false,
			false,
			1,
			1
		)
	)
{
	//mTextureInfoCastPtr->glImageFormat.desiredInternalFormat = GL_DEPTH_COMPONENT32;
	//mTextureInfoCastPtr->glImageFormat.channelOrder= GL_DEPTH_COMPONENT;

	assert(
		(mTextureInfoCastPtr->glImageFormat.desiredInternalFormat == GL_DEPTH_COMPONENT32)
		&&
		(mTextureInfoCastPtr->glImageFormat.channelOrder == GL_DEPTH_COMPONENT)
	);

	allocMem();

	//don't set data to the depth texture;
}

//protected constructor to be called by Texture1dArray()
Texture2D::Texture2D(const TextureInfo& texi, const void* data)
:Texture(texi)
{
	allocMem();
	if(data)
	{
		setData(data,mBufferInfo->usageContexts);
	}
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
//-----------------------------------------------------------------------

Texture2DCube::Texture2DCube(String name,
		int quadraticSize, const TexelInfo& texeli,
		 bool allocHostMemory,
		//an array containing all six images in the following order:
		// +x,-x,+y,-y,+z,-z
		const void* data,
		bool genMipmaps)
:
	Texture	(
		TextureInfo(
			BufferInfo(
				name,
				ContextTypeFlags(
						(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
						| OPEN_GL_CONTEXT_TYPE_FLAG
				),
				ENVMAP_SEMANTICS
			),
			2,
			Vector3Dui(quadraticSize,quadraticSize,1),
			texeli,
			GL_TEXTURE_CUBE_MAP,
			false, //no depth tex
			genMipmaps,
			false,
			true,
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

//protected constructor to be called by Texture2DDepth:
Texture2DCube::Texture2DCube(String name,	int quadraticSize, bool allocHostMemory)
	:
	Texture	(
		TextureInfo(
			BufferInfo(
				name,
				ContextTypeFlags(
						(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
						| OPEN_GL_CONTEXT_TYPE_FLAG
				),
				SHADOW_MAP_SEMANTICS
			),
			2,
			Vector3Dui(quadraticSize,quadraticSize,1),
			TexelInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
			GL_TEXTURE_CUBE_MAP,
			true, //yes, is depth tex
			false, //no mipmap for depth textures
			false,
			false,
			1,
			1
		)
	)
{
	//mTextureInfoCastPtr->glImageFormat.desiredInternalFormat = GL_DEPTH_COMPONENT32;
	//mTextureInfoCastPtr->glImageFormat.channelOrder= GL_DEPTH_COMPONENT;

	assert(
		(mTextureInfoCastPtr->glImageFormat.desiredInternalFormat == GL_DEPTH_COMPONENT32)
		&&
		(mTextureInfoCastPtr->glImageFormat.channelOrder == GL_DEPTH_COMPONENT)
	);

	allocMem();

	//don't set data to the depth texture;
}


Texture2DCube::~Texture2DCube()
{
	//do nothing
}

bool Texture2DCube::operator==(const BufferInterface& rhs) const
{
	const Texture2DCube* rhsTexPtr = dynamic_cast<const Texture2DCube*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}

void Texture2DCube::allocGL()throw(BufferException)
{
	for(int i=0; i<6; i++)
	{
		glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
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
}

void Texture2DCube::writeGL(const void* data)throw(BufferException)
{
	assert( ((mTextureInfoCastPtr->bufferSizeInByte % 6) == 0)
			&& "buffer size of cube map  must be multiple of 6");
	int sizeInByteOfOneFace = mTextureInfoCastPtr->bufferSizeInByte / 6;
	for(int i=0; i<6; i++)
	{
		glTexSubImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				0,0,
				mTextureInfoCastPtr->dimensionExtends.x,
				mTextureInfoCastPtr->dimensionExtends.y,

				mTextureInfoCastPtr->glImageFormat.channelOrder,
				mTextureInfoCastPtr->glImageFormat.channelDataType,
				//pointer to beginning of current face
				&  (reinterpret_cast<const GLubyte*>(data)[i* sizeInByteOfOneFace])
		);
	}
}

//must also be overridden by CubeMap class, as the read call must happen six times instead of one;
//be sure that the data buffer is 6 times bigger than one face;
void Texture2DCube::readGL(void* data)throw(BufferException)
{
	assert( ((mTextureInfoCastPtr->bufferSizeInByte % 6) == 0)
				&& "buffer size of cube map  must be multiple of 6");
		int sizeInByteOfOneFace = mTextureInfoCastPtr->bufferSizeInByte / 6;

	for(int i=0; i<6; i++)
	{
		glGetTexImage(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				mTextureInfoCastPtr->glImageFormat.channelOrder,
				mTextureInfoCastPtr->glImageFormat.channelDataType,
				//pointer to beginning of current face
				&  (reinterpret_cast<GLubyte*>(data)[i* sizeInByteOfOneFace])
				);
	}
}

//there are several ways to bind a cube map to an FBO
//TODO find at least a working one, better a short/efficient one
//must also be overridden by CubeMap, as there are six color attachments instead of one
//glFrameBufferTextureFace... we'll see
void Texture2DCube::copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	//TODO implement when FBO class exists;
	throw(BufferException("Texture::copyGLFrom: sorry, for copying, FBOs are needed, and the still must be implemented;"
			" Plus useful framebuffer binding of a cube map for blitting is still a TODO"));
}

//---------------------------------------------------------------------------------

Texture2DCubeDepth::Texture2DCubeDepth(String name,
			int quadraticSize,
			bool allocHostMemory)
: Texture2DCube(name, quadraticSize, allocHostMemory)
{
	LOG<<INFO_LOG_LEVEL << "Creating 2D CUBE DEPTH texture named "<< name <<";\n";
}

Texture2DCubeDepth::~Texture2DCubeDepth()
{
	//do nothing, base classes do the rest ;(
}

bool Texture2DCubeDepth::operator==(const BufferInterface& rhs) const
{
	const Texture2DCubeDepth* rhsTexPtr = dynamic_cast<const Texture2DCubeDepth*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}

//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
void Texture2DCubeDepth::allocGL()throw(BufferException)
{
	Texture2DCube::allocGL();
	setupDepthTextureParameters();
}
//-----------------------------------------------------------------------


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

void Texture2DDepth::allocGL()throw(BufferException)
{
	Texture2D::allocGL();
	setupDepthTextureParameters();
}
//------------------------------------------------------------------------



//--------------------------------------------------------------------------

Texture2DArray::Texture2DArray(String name, BufferSemantics bufferSemantics,
	int width, int height, int numLayers,  const TexelInfo& texeli,
	bool allocHostMemory, const void* data,  bool genMipmaps)
:
Texture3D	(
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(
					(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
					| OPEN_GL_CONTEXT_TYPE_FLAG
			),
			bufferSemantics
		),
		3,
		Vector3Dui(width,height,1),
		texeli,
		GL_TEXTURE_2D_ARRAY,
		false, //not depth tex
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

Texture2DArray::~Texture2DArray()
{
	//do nothing
}

bool Texture2DArray::operator==(const BufferInterface& rhs) const
{
	const Texture2DArray* rhsTexPtr = dynamic_cast<const Texture2DArray*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}



//------------------------------------------------------------------------------
Texture2DMultiSample::Texture2DMultiSample(String name, BufferSemantics bufferSemantics,
		int width, int height, int numMultiSamples,  const TexelInfo& texeli)
: Texture
  (
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(OPEN_GL_CONTEXT_TYPE_FLAG	),
			bufferSemantics
		),
		2,
		Vector3Dui(width,height,1),
		texeli,
		GL_TEXTURE_2D_MULTISAMPLE,
		false, //no depth tex
		false,
		false,
		false,
		numMultiSamples,
		1
	)
  )

{

}

Texture2DMultiSample::~Texture2DMultiSample()
{
	//nothing
}

bool Texture2DMultiSample::operator==(const BufferInterface& rhs) const
{
	const Texture2DMultiSample* rhsTexPtr = dynamic_cast<const Texture2DMultiSample*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}

void Texture2DMultiSample::allocGL()throw(BufferException)
{
	glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE,
			mTextureInfoCastPtr->numMultiSamples,
			mTextureInfoCastPtr->glImageFormat.desiredInternalFormat,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
			GL_TRUE
			);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Texture2DArrayMultiSample::Texture2DArrayMultiSample(String name, BufferSemantics bufferSemantics,
		int width, int height, int numLayers, int numMultiSamples,  const TexelInfo& texeli)
: Texture
  (
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(OPEN_GL_CONTEXT_TYPE_FLAG	),
			bufferSemantics
		),
		2,
		Vector3Dui(width,height,1),
		texeli,
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
		false, //no depth tex
		false,
		false,
		false,
		numMultiSamples,
		numLayers
	)
  )

{

}

Texture2DArrayMultiSample::~Texture2DArrayMultiSample()
{
	//nothing
}

bool Texture2DArrayMultiSample::operator==(const BufferInterface& rhs) const
{
	const Texture2DArrayMultiSample* rhsTexPtr = dynamic_cast<const Texture2DArrayMultiSample*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}

void Texture2DArrayMultiSample::allocGL()throw(BufferException)
{
	glTexImage3DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE,
			mTextureInfoCastPtr->numMultiSamples,
			mTextureInfoCastPtr->glImageFormat.desiredInternalFormat,
			mTextureInfoCastPtr->dimensionExtends.x,
			mTextureInfoCastPtr->dimensionExtends.y,
			mTextureInfoCastPtr->dimensionExtends.z,
			GL_TRUE
			);
}
//------------------------------------------------------------------------------


}
