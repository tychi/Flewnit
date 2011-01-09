/*
 * Texture.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"
#include "Simulator/OpenCL_Manager.h"

namespace Flewnit
{

Texture::Texture(const TextureInfo& texi)
:BufferInterface(), mTextureInfoCastPtr(new TextureInfo(texi))
{
	mBufferInfo = mTextureInfoCastPtr;


}



Texture::~Texture()
{
	//delete texturem so that derived classes don't have to do it;
	GUARD(freeGL());
}


//not pure, as all the same for every texture;
void Texture::generateGL()throw(BufferException)
{

}

//the two non-symmetrci GL-only routines:
//non-pure, as binding is all the same, only the enum is different, and that can be looked up in mTexturInfo;
void Texture::bindGL()throw(BufferException)
{

}


//can be non-pure, as clEnqueueWriteImage is quite generic;
void Texture::writeCL(const void* data)throw(BufferException)
{

}

//non-pure, as glGetTexImage is quite generic :); at least one generic GL function ;(
//BUT:
//must be overidden with an exception-throw-implementation for certain concrete Texture classes
//(namely multisample textures), which don't seem to be writable, copyable ar readable
//must also be overridden by CubeMap class, as the read call must happen six times;
void Texture::readGL(void* data)throw(BufferException)
{

}
//can be non-pure, as clEnqueueReadImage is quite generic;
void Texture::readCL(void* data)throw(BufferException)
{

}

//as it seems that a generic copying of many texture types can happen in an agnostic way
//via two helper FBOs, this can be non-pure;
//BUT:
//must be overidden with an exception-throw-implementation for certain concrete Texture classes
//(namely multisample textures), which don't seem to be writable, copyable ar readable;
//must also be overriden by CubeMap, as there are six color attachments to to or to work with
//glFrameBufferTextureFace... we'll see
void Texture::copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{

}
//can be non-pure, as clEnqueueCopyImage is quite generic;
void Texture::copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{

}

//non-pure, as glDeleteTextures() applies to every texture type :)
void Texture::freeGL()throw(BufferException)
{

}


}
