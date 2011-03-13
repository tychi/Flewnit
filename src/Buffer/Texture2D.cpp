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
		int width, int height, const BufferElementInfo& texeli,
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
			bufferSemantics,
			texeli
		),
		2,
		Vector3Dui(width,height,1),
		//texeli,
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

	setupDefaultSamplerParameters();
}

//protected constructor to be called by Texture2DDepth:
Texture2D:: Texture2D(String name,
			int width, int height,
			bool rectangle,
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
				DEPTH_BUFFER_SEMANTICS, //SHADOW_MAP_SEMANTICS,
				BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false)
			),
			2,
			Vector3Dui(width,height,1),
			//BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
			rectangle ? GL_TEXTURE_RECTANGLE :GL_TEXTURE_2D,
			true, //yes, is depth tex
			false,
			rectangle,
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

	setupDefaultSamplerParameters();
}


Texture2D::~Texture2D()
{
	//nothing to do, base classes do the rest ;)
}


void Texture2D::setupDefaultSamplerParameters()
{
	GUARD(bindGL());

	if(! mTextureInfoCastPtr->isRectangleTex)
	{
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S,
				GL_REPEAT));
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T,
				GL_REPEAT));
	}
	else
	{
		//to border instead ogf edge in order to have a defined value outside the domain (usually black)
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S,
				GL_CLAMP_TO_BORDER));
		GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T,
				GL_CLAMP_TO_BORDER));
	}


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
		int quadraticSize, const BufferElementInfo& texeli,
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
				ENVMAP_SEMANTICS,
				texeli
			),
			2,
			Vector3Dui(quadraticSize,quadraticSize,1),
			//texeli,
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

	setupDefaultSamplerParameters();
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
				DEPTH_BUFFER_SEMANTICS, //SHADOW_MAP_SEMANTICS,
				BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false)
			),
			2,
			Vector3Dui(quadraticSize,quadraticSize,1),
			//BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
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


void Texture2DCube::setupDefaultSamplerParameters()
{
	GUARD(bindGL());

	//TODO try seamless cubemapping stuff

	//don't know any useful domain setup for cubemaps... but i fear more problems when no
	//params are set than when dumb params are set ;(
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_BORDER));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_BORDER));
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

Texture2DDepthCube::Texture2DDepthCube(String name,
			int quadraticSize,
			bool allocHostMemory)
: Texture2DCube(name, quadraticSize, allocHostMemory)
{
	LOG<<INFO_LOG_LEVEL << "Creating 2D CUBE DEPTH texture named "<< name <<";\n";

	setupDefaultSamplerParameters();
}

Texture2DDepthCube::~Texture2DDepthCube()
{
	//do nothing, base classes do the rest ;(
}


void Texture2DDepthCube::setupDefaultSamplerParameters()
{
	GUARD(bindGL());
	//TODO try seamless cubemapping stuff

	//TODO handle this more professionally via sampler objects within material/shader...
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));

	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    float fourZeros[] = {0.0f,0.0f,0.0f,0.0f};
    GUARD(glTexParameterfv(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_BORDER_COLOR, fourZeros));
    //TODO try 	& Vector4D(0.0f,0.0f,0.0f,0.0f)[0];

    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    assert("depth textures have no mip maps" && ! mTextureInfoCastPtr->isMipMapped);
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
}

bool Texture2DDepthCube::operator==(const BufferInterface& rhs) const
{
	const Texture2DDepthCube* rhsTexPtr = dynamic_cast<const Texture2DDepthCube*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}

//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
void Texture2DDepthCube::allocGL()throw(BufferException)
{
	Texture2DCube::allocGL();
	//setupDepthTextureParameters();
}
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
Texture2DDepth::Texture2DDepth(String name,
			int width, int height,
			bool rectangle,
			bool allocHostMemory, bool clInterOp)
: Texture2D(name,width,height,rectangle, allocHostMemory,clInterOp)
{
	LOG<<INFO_LOG_LEVEL << "Creating 2D depth texture named "<< name <<";\n";

	setupDefaultSamplerParameters();
}

Texture2DDepth::~Texture2DDepth()
{
	//nothing to do, base classes do the rest ;)
}

void Texture2DDepth::setupDefaultSamplerParameters()
{
	GUARD(bindGL());
	//TODO try seamless cubemapping stuff

	//TODO handle this more professionally via sampler objects within material/shader...
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));

    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    float fourZeros[] = {0.0f,0.0f,0.0f,0.0f};
    GUARD(glTexParameterfv(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_BORDER_COLOR, fourZeros));
    //TODO try 	& Vector4D(0.0f,0.0f,0.0f,0.0f)[0];

    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    assert("depth textures have no mip maps" && ! mTextureInfoCastPtr->isMipMapped);
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
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
	//setupDepthTextureParameters();
}
//------------------------------------------------------------------------



//--------------------------------------------------------------------------

Texture2DArray::Texture2DArray(String name, BufferSemantics bufferSemantics,
	int width, int height, int numLayers,  const BufferElementInfo& texeli,
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
			bufferSemantics,
			texeli
		),
		3,
		Vector3Dui(width,height,1),
		//texeli,
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
	setupDefaultSamplerParameters();
}


//protected constructor to be called by Texture2DDepthArray:
Texture2DArray::Texture2DArray(String name,
			int width, int height, int numLayers,
			bool allocHostMemory)
:
	Texture3D	(
		TextureInfo(
			BufferInfo(
				name,
				ContextTypeFlags(
						(allocHostMemory ? HOST_CONTEXT_TYPE_FLAG: NO_CONTEXT_TYPE_FLAG )
						| OPEN_GL_CONTEXT_TYPE_FLAG
				),
				DEPTH_BUFFER_SEMANTICS, //SHADOW_MAP_SEMANTICS,
				BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false)
			),
			2,
			Vector3Dui(width,height,1),
			//BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
			GL_TEXTURE_2D_ARRAY,
			true, //is depth tex
			false,
			false,
			false,
			1,
			numLayers
		),
		0
	)
{
	assert(
			(mTextureInfoCastPtr->glImageFormat.desiredInternalFormat == GL_DEPTH_COMPONENT32)
			&&
			(mTextureInfoCastPtr->glImageFormat.channelOrder == GL_DEPTH_COMPONENT)
	);
}


Texture2DArray::~Texture2DArray()
{
	//do nothing
}


void Texture2DArray::setupDefaultSamplerParameters()
{
	GUARD(bindGL());

	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S,
			GL_REPEAT));
	GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T,
			GL_REPEAT));
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

bool Texture2DArray::operator==(const BufferInterface& rhs) const
{
	const Texture2DArray* rhsTexPtr = dynamic_cast<const Texture2DArray*>(&rhs);
	if (rhsTexPtr)
	{return   (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo()) ;}
	else {return false;}
}


//---------------------------------------------------------------------

Texture2DDepthArray::Texture2DDepthArray(String name,
			int width, int height, int numLayers,
			bool allocHostMemory)
: Texture2DArray(name,
		 width,  height,  numLayers,
		 allocHostMemory)
{
	LOG<<INFO_LOG_LEVEL << "Creating 2D DEPTH ARRAY texture named "<< name <<";\n";

	setupDefaultSamplerParameters();
}

Texture2DDepthArray::~Texture2DDepthArray()
{
	//do nothing, base classes do the rest
}


void Texture2DDepthArray::setupDefaultSamplerParameters()
{
	GUARD(bindGL());
	//TODO try seamless cubemapping stuff

	//TODO handle this more professionally via sampler objects within material/shader...
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));

    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    float fourZeros[] = {0.0f,0.0f,0.0f,0.0f};
    GUARD(glTexParameterfv(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_BORDER_COLOR, fourZeros));
    //TODO try 	& Vector4D(0.0f,0.0f,0.0f,0.0f)[0]);

    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    assert("depth textures have no mip maps" && ! mTextureInfoCastPtr->isMipMapped);
    GUARD(glTexParameteri(mTextureInfoCastPtr->textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
}



bool Texture2DDepthArray::operator==(const BufferInterface& rhs) const
{
	const Texture2DDepthArray* rhsTexPtr =
			dynamic_cast<const Texture2DDepthArray*> (&rhs);
	if (rhsTexPtr) {
		return (*mTextureInfoCastPtr) == (rhsTexPtr->getTextureInfo());
	} else {
		return false;
	}
}

//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
void Texture2DDepthArray::allocGL()throw(BufferException)
{
	Texture3D::allocGL();
	//setupDepthTextureParameters();
}



//------------------------------------------------------------------------------
Texture2DMultiSample::Texture2DMultiSample(String name, BufferSemantics bufferSemantics,
		int width, int height, int numMultiSamples,  const BufferElementInfo& texeli)
: Texture
  (
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(OPEN_GL_CONTEXT_TYPE_FLAG	),
			bufferSemantics,
			texeli
		),
		2,
		Vector3Dui(width,height,1),
		//texeli,
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
	setupDefaultSamplerParameters();
}

Texture2DMultiSample::~Texture2DMultiSample()
{
	//nothing
}

void Texture2DMultiSample::setupDefaultSamplerParameters()
{
	//do nothing for MS textures, as you can do only texel fetches anyway
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
		int width, int height, int numLayers, int numMultiSamples,  const BufferElementInfo& texeli)
: Texture
  (
	TextureInfo(
		BufferInfo(
			name,
			ContextTypeFlags(OPEN_GL_CONTEXT_TYPE_FLAG	),
			bufferSemantics,
			texeli
		),
		2,
		Vector3Dui(width,height,1),
		//texeli,
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
	setupDefaultSamplerParameters();
}

Texture2DArrayMultiSample::~Texture2DArrayMultiSample()
{
	//nothing
}

void Texture2DArrayMultiSample::setupDefaultSamplerParameters()
{
	//do nothing for MS textures, as you can do only texel fetches anyway
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
			mTextureInfoCastPtr->textureTarget, //GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
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
