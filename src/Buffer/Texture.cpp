/*
 * Texture.cpp
 *
 *  Created on: Jan 2, 2011
 *      Author: tychi
 */

#include "Texture.h"
#include "Simulator/OpenCL_Manager.h"
#include "Simulator/SimulationResourceManager.h"

namespace Flewnit
{

Texture::Texture(const TextureInfo& texi)
:BufferInterface(texi) //, mTextureInfoCastPtr(new TextureInfo(texi))
{
	//mBufferInfo = mTextureInfoCastPtr;
	mTextureInfoCastPtr = dynamic_cast<TextureInfo*>(mBufferInfo);
	assert(mTextureInfoCastPtr && "mBufferinfo must be TextureInfo");

	SimulationResourceManager::getInstance().registerTexture(this);
}



Texture::~Texture()
{
	//delete texturem so that derived classes don't have to do it;
	GUARD(freeGL());
}


//not pure, as all the same for every texture;
void Texture::generateGL()throw(BufferException)
{
	glGenTextures(1, & mGraphicsBufferHandle);
}

//the two non-symmetrci GL-only routines:
//non-pure, as binding is all the same, only the enum is different, and that can be looked up in mTexturInfo;
void Texture::bindGL()throw(BufferException)
{
	glBindTexture(mTextureInfoCastPtr->textureTarget, mGraphicsBufferHandle);
}


//can be non-pure, as clEnqueueWriteImage is quite generic;
void Texture::writeCL(const void* data)throw(BufferException)
{
	cl::size_t<3> origin; origin[0]=0;origin[1]=0;origin[2]=0;

	cl::size_t<3> region;
	region[0] =	mTextureInfoCastPtr->dimensionExtends.x;
	region[1] =	mTextureInfoCastPtr->dimensionExtends.y;
	region[2] =	mTextureInfoCastPtr->dimensionExtends.z;

	CLMANAGER->getCommandQueue().enqueueWriteImage(
			static_cast<cl::Image&>(mComputeBufferHandle),
			CLMANAGER->getBlockAfterEnqueue(),
			origin,
			region,
			0,
			0,
			//BAAAD haxx due to a bug in the ocl-c++-bindings: a otherwise const-param is non-const here,
			//although it is const in the wrapped c-function 0_o
			const_cast<void*>( data ),
			0,
			& CLMANAGER->getLastEvent()
	);
}

//non-pure, as glGetTexImage is quite generic :); at least one generic GL function ;(
//BUT:
//must be overidden with an exception-throw-implementation for certain concrete Texture classes
//(namely multisample textures), which don't seem to be writable, copyable ar readable
//must also be overridden by CubeMap class, as the read call must happen six times;
void Texture::readGL(void* data)throw(BufferException)
{
	glGetTexImage(mTextureInfoCastPtr->textureTarget,
			0,
			mTextureInfoCastPtr->glImageFormat.channelOrder,
			mTextureInfoCastPtr->glImageFormat.channelDataType,
			data
			);
}

//can be non-pure, as clEnqueueReadImage is quite generic;
void Texture::readCL(void* data)throw(BufferException)
{
	cl::size_t<3> origin; origin[0]=0;origin[1]=0;origin[2]=0;

	cl::size_t<3> region;
	region[0] =	mTextureInfoCastPtr->dimensionExtends.x;
	region[1] =	mTextureInfoCastPtr->dimensionExtends.y;
	region[2] =	mTextureInfoCastPtr->dimensionExtends.z;

	CLMANAGER->getCommandQueue().enqueueReadImage(
			static_cast<cl::Image&>(mComputeBufferHandle),
			CLMANAGER->getBlockAfterEnqueue(),
			origin,
			region,
			0,
			0,
			data,
			0,
			& CLMANAGER->getLastEvent()
	);
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
	//TODO implement when FBO class exists;
	throw(BufferException("Texture::copyGLFrom: sorry, for copying, FBOs are needed, and the still must be implemented "));
}

//can be non-pure, as clEnqueueCopyImage is quite generic;
void Texture::copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException)
{
	cl::size_t<3> origin; origin[0]=0;origin[1]=0;origin[2]=0;

	cl::size_t<3> region;
	region[0] =	mTextureInfoCastPtr->dimensionExtends.x;
	region[1] =	mTextureInfoCastPtr->dimensionExtends.y;
	region[2] =	mTextureInfoCastPtr->dimensionExtends.z;

	CLMANAGER->getCommandQueue().enqueueCopyImage(
			//source
			static_cast<const cl::Image&>(bufferToCopyContentsFrom),
			//destination
			static_cast<const cl::Image&>(mComputeBufferHandle),
			origin,
			origin,
			region,
			0,
			& CLMANAGER->getLastEvent()
	);
}

//non-pure, as glDeleteTextures() applies to every texture type :)
void Texture::freeGL()throw(BufferException)
{
	glDeleteTextures(1, & mGraphicsBufferHandle);
}


}
