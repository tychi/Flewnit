/*
 * RenderTarget.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "RenderTarget.h"

#include "Common/CL_GL_Common.h"
#include "Util/Log/Log.h"
#include "Simulator/OpenCL_Manager.h"

#include "Buffer/BufferSharedDefinitions.h"
#include "Buffer/BufferHelperUtils.h"


#include "Buffer/Texture.h"



namespace Flewnit
{

//multiSample flag needed for render buffer generation
RenderTarget::RenderTarget(String name, const Vector2Di& resolution,
		bool useRectangleTextures,RenderBufferFlags rbf, int numMultisamples)
: mName(name), mNumCurrentDrawBuffers(0),
  mFrameBufferResolution(resolution),
  mUseRectangleTextures(useRectangleTextures), mRenderBufferFlags(rbf),
  //following two components play no role if NO_RENDERBUFFER, hence the comparison to only DEPTH_RENDER_BUFFER is correct
  mRenderBufferAttachmentPoint(
		  	(rbf == DEPTH_RENDER_BUFFER )
			? GL_DEPTH_ATTACHMENT
			: GL_DEPTH_STENCIL_ATTACHMENT),
  mRenderBufferInternalFormat(
		    (mRenderBufferFlags == DEPTH_RENDER_BUFFER )
			? GL_DEPTH_COMPONENT32F
			: GL_DEPTH32F_STENCIL8),
  mNumMultisamples(numMultisamples)
{
	//default initialization
	mColorRenderingEnabled = true;
//	mDepthTestEnabled = (mRenderBufferFlags != NO_RENDER_BUFFER);
//	setEnableDepthTest(mDepthTestEnabled);
//	mStencilTestEnabled = false ; //(mRenderBufferFlags == DEPTH_STENCIL_RENDER_BUFFER );
//	glDisable(GL_STENCIL_TEST);

	mIsReadFrameBuffer = false;

	for(int i= 0 ; i < __NUM_TOTAL_SEMANTICS__; i++)
	{
		mOwnedTexturePool[i] = 0;
	}


	for(int i= 0 ; i < FLEWNIT_MAX_COLOR_ATTACHMENTS; i++)
	{
		mCurrentlyAttachedTextures[i] = 0;
		//default stuff
		mCurrentDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	assert(mFrameBufferResolution.x > 0 &&  mFrameBufferResolution.y >0
			&& BufferHelper::isPowerOfTwo(mNumMultisamples) && mNumMultisamples <= FLEWNIT_MAX_MULTISAMPLES);


	GUARD( glGenFramebuffers(1,&mFBO) );

	bind();

	if( mRenderBufferFlags != NO_RENDER_BUFFER )
	{
		//create the renderbuffer

//		GLenum renderBufferAttachmentPoint =
//				(mRenderBufferFlags == DEPTH_RENDER_BUFFER )
//				? GL_DEPTH_ATTACHMENT
//				: GL_DEPTH_STENCIL_ATTACHMENT
//		;
//
//		GLenum renderBufferInternalFormat =
//				(mRenderBufferFlags == DEPTH_RENDER_BUFFER )
//				? GL_DEPTH_COMPONENT32F
//				: GL_DEPTH32F_STENCIL8
//		;


		GUARD_FRAMEBUFFER( glGenRenderbuffers(1, &mGLRenderBufferHandle));
		GUARD_FRAMEBUFFER( glBindRenderbuffer(GL_RENDERBUFFER, mGLRenderBufferHandle));


		//according to the spec the following must work in any case:
		//GUARD_FRAMEBUFFER( glRenderbufferStorageMultisample(GL_RENDERBUFFER, mNumMultisamples, renderBufferInternalFormat,
		//		mFrameBufferResolution.x, mFrameBufferResolution.y) );

		//but I don't trust the implementors, so lets do it the newbi way:
		if(mNumMultisamples == 0)
		{
			GUARD_FRAMEBUFFER( glRenderbufferStorage(GL_RENDERBUFFER, mRenderBufferInternalFormat,
				mFrameBufferResolution.x, mFrameBufferResolution.y) );
		}
		else
		{
			GUARD_FRAMEBUFFER( glRenderbufferStorageMultisample(GL_RENDERBUFFER, mNumMultisamples,
				mRenderBufferInternalFormat,
				mFrameBufferResolution.x, mFrameBufferResolution.y) );
		}

		//attach renderbuffer to depth/stencil attachment point of the FBO
		GUARD_FRAMEBUFFER(
				glFramebufferRenderbuffer(
						GL_FRAMEBUFFER,
						mRenderBufferAttachmentPoint,
						GL_RENDERBUFFER,
						mGLRenderBufferHandle
						);
		);

		GUARD_FRAMEBUFFER( glBindRenderbuffer(GL_RENDERBUFFER, 0) );
	}
	else
	{
		mGLRenderBufferHandle =0;
	}


	renderToScreen();


}
RenderTarget::~RenderTarget()
{
	if(mGLRenderBufferHandle != 0)
	{
		GUARD_FRAMEBUFFER( glDeleteRenderbuffers(1, &mGLRenderBufferHandle));
	}
	GUARD( glDeleteFramebuffers(1, &mFBO));
}


bool RenderTarget::isCurrentlyBound()
{
	GLint currentFrameBufferBinding;
	GUARD_FRAMEBUFFER(glGetIntegerv(mIsReadFrameBuffer? GL_READ_FRAMEBUFFER_BINDING: GL_DRAW_FRAMEBUFFER_BINDING,
					&currentFrameBufferBinding));
	return ((GLuint)currentFrameBufferBinding == mFBO);
}

void RenderTarget::bind(bool forReading)
{
	mIsReadFrameBuffer = forReading;
	GUARD_FRAMEBUFFER(glBindFramebuffer(mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, mFBO));
}

//calls bind() automatically
void RenderTarget::renderToAttachedTextures()
{
	//assert( ! (mIsReadFrameBuffer || isCurrentlyBound()) && "must be bound as draw buffer!" );
	if( mIsReadFrameBuffer || !isCurrentlyBound())
	{
		LOG<<WARNING_LOG_LEVEL<<"RenderTarget::renderToAttachedTextures: FBO was not bound as draw buffer! Doing the binding for you...";
		bind();
	}
	assert(mNumCurrentDrawBuffers >0 && "attach at least one texture to the FBO!" );
	glDrawBuffers(mNumCurrentDrawBuffers, mCurrentDrawBuffers);
}

void RenderTarget::clear()
{
	bindSave();

	//throw exception if stencil test is enabled but a stencil buffer isn't currently available
	if(stencilTestEnabled()  &&   ! hasStencilAttachment() )
	{
		throw(BufferException("This rendertarget has no stencil buffer attached! disable stencil test!"));
	}
	if(depthTestEnabled()  &&   ! hasDepthAttachment() )
	{
		throw(BufferException("This rendertarget has no depth buffer attached! disable depth test or attach a depth texture!"));
	}

	GUARD_FRAMEBUFFER (
		glClear(
			mColorRenderingEnabled ? GL_COLOR_BUFFER_BIT : 0
					|
			depthTestEnabled()? GL_DEPTH_BUFFER_BIT : 0
					|
			stencilTestEnabled()? GL_STENCIL_BUFFER_BIT : 0
		)
	);
	unbindSave();
}



void RenderTarget::renderToScreen()
{
	//GUARD_FRAMEBUFFER(glBindFramebuffer(mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0));
	GUARD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTarget::setEnableColorRendering(bool value)
{
	bindSave();

	mColorRenderingEnabled = value;

	if(value)
	{
		renderToAttachedTextures();
	}
	else
	{
		if( mIsReadFrameBuffer || !isCurrentlyBound())
		{
			LOG<<WARNING_LOG_LEVEL<<"RenderTarget: Color rendering disabling: FBO is not bound as draw buffer! Doing the binding for you...";
			bind();
		}
		GUARD_FRAMEBUFFER (glDrawBuffer(GL_NONE));
		GUARD_FRAMEBUFFER (glReadBuffer(GL_NONE));
	}

	unbindSave();
}

void RenderTarget::setEnableDepthTest(bool value)
{
	//mDepthTestEnabled = value;

	if(value)
	{
		GUARD(glEnable(GL_DEPTH_TEST));
	}
	else
	{
		GUARD(glDisable(GL_DEPTH_TEST));
	}
}

void RenderTarget::setEnableStencilTest(bool value)
{
	//throw exception if a stencil buffer wasn't specified on RenderTarget creation;
//	if(value &&   (mRenderBufferFlags != DEPTH_RENDER_BUFFER ))
//	{
//		throw(BufferException("Stis rendertarget has no stencil buffer! Cannot activate stencil test!"));
//	}
//
//
//	mStencilTestEnabled = value;

	if(value)
	{
		GUARD(glEnable(GL_STENCIL_TEST));
	}
	else
	{
		GUARD(glDisable(GL_STENCIL_TEST));
	}
}

//void RenderTarget::addTexture(Texture* tex)
//{
//	//TODO implement IF needed
//}
//
//void RenderTarget::removeTexture(Texture* tex)
//{
//	//TODO implement IF needed
//}

//can return NULL
Texture* RenderTarget::getStoredTexture(BufferSemantics bs)
{
	return mOwnedTexturePool[bs];
}


void RenderTarget::attachColorTexture(Texture* tex, int where)
{
	bindSave();

	assert(where < FLEWNIT_MAX_COLOR_ATTACHMENTS);

	GUARD_FRAMEBUFFER(
			glFramebufferTexture2D(
					mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + where,
					tex -> getTextureInfo().textureTarget,
					tex -> getGraphicsBufferHandle(),
					0
			)
	);


	if(mCurrentlyAttachedTextures[where] == 0)
	{
		//attach to an empty attachment point, total number of draw buffers increases;
		//otherwise, the draw buffer array doesn't change, just the texture attachment
		mCurrentDrawBuffers[mNumCurrentDrawBuffers++]= GL_COLOR_ATTACHMENT0 + where;
	}

	mCurrentlyAttachedTextures[where] = tex;


	unbindSave();
}

void RenderTarget::attachStoredColorTexture(BufferSemantics which, int where) throw(BufferException)
{
	if(mOwnedTexturePool[which] == 0)
	{
		throw(BufferException("attachStoredColorTexture: texture must exist!"));
	}

	attachColorTexture(mOwnedTexturePool[which],where);

}



void RenderTarget::detachAllColorTextures()
{
	bindSave();

	for(int i= 0 ; i < FLEWNIT_MAX_COLOR_ATTACHMENTS; i++)
	{
		if( mCurrentlyAttachedTextures[i] != 0 )
		{
			//attch zero to attachment point to provoke a detach
			GUARD_FRAMEBUFFER(
					glFramebufferTexture2D(
							mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
							GL_COLOR_ATTACHMENT0 + i,
							mCurrentlyAttachedTextures[i] -> getTextureInfo().textureTarget,
							0,
							0
					)
			);
			mCurrentlyAttachedTextures[i] = 0;
		}
		//reset to default stuff
		mCurrentDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	mNumCurrentDrawBuffers =0;

	unbindSave();
}

/**
 * A texture will be created with the specified semantics if it doesn't exist;
 * If it exists, it nothing will be done but printed a warning
 */
void RenderTarget::requestCreateAndStoreTexture(BufferSemantics which)throw(BufferException)
{
	if(mOwnedTexturePool[which])
	{
		LOG<<WARNING_LOG_LEVEL<<"requestCreateAndStoreTexture: Texture with BufferSemantics "
				<< BufferHelper::BufferSemanticsToString(which) << " already exists!\n";
		return;
	}

	TexelInfo texeli(4,GPU_DATA_TYPE_FLOAT,32,false);

	switch(which)
	{
	//floating point four-component textures:
	case POSITION_SEMANTICS:
	case TEXCOORD_SEMANTICS:
	case DECAL_COLOR_SEMANTICS:
	case VELOCITY_SEMANTICS:
	case FORCE_SEMANTICS:
	case DISPLACEMENT_SEMANTICS: //normal-depth or normal map
	case INTERMEDIATE_RENDERING_SEMANTICS:
	case FINAL_RENDERING_SEMANTICS:

		//do nothing as texeli has no noeed to be modded;

		break;

	//treat them seperately in case we wanna save memory by storing them as normalized signed byte
	case NORMAL_SEMANTICS:
	case TANGENT_SEMANTICS:

		//do nothing, as stuff shall work before one tries to optimize;
		//TODO

		break;

	//floating point one-component textures:
	case SHADOW_MAP_SEMANTICS:
		LOG<<WARNING_LOG_LEVEL<<"Requesting a COLOR texture with SHADOW_MAP_SEMANTICS;"
				"<< Think about requesting a DEPTH texture instead;\n";
	case MASS_SEMANTICS:
	case DENSITY_SEMANTICS:
	case PRESSURE_SEMANTICS:
	case AMBIENT_OCCLUSION_SEMANTICS:
	case NOISE_SEMANTICS:
	case DEPTH_BUFFER_SEMANTICS:

		texeli.numChannels = 1;
		texeli.validate();

		break;
	//unsigned integer one-component semantics
	case Z_INDEX_SEMANTICS:
	case INDEX_SEMANTICS:
	case MATERIAL_ID_SEMANTICS:
	case PRIMITIVE_ID_SEMANTICS:
	case STENCIL_BUFFER_SEMANTICS:

		texeli.numChannels = 1;
		texeli.internalGPU_DataType = GPU_DATA_TYPE_UINT;
		texeli.validate();

		break;
	case ENVMAP_SEMANTICS:
		throw(BufferException("Sorry Rendering to Environment Maps is not yet implemented!"));
		return;
	default:
		//completely stupid enums like:
		/*
			TRANSFORMATION_MATRICES_SEMANTICS,
			__NUM_VALID_GEOMETRY_ATTRIBUTE_SEMANTICS__,
			CUSTOM_SEMANTICS,
			__NUM_TOTAL_SEMANTICS__,
			INVALID_SEMANTICS
		 * */
		String exString("RenderTarget::requestCreateAndStoreTexture(): invalid bufferSemantics enum: ");
		exString.append(BufferHelper::BufferSemanticsToString(which));
		throw(BufferException(exString));
		return;
	};

	//construct unique texture name
	String textureName = mName;
	textureName.append(BufferHelper::BufferSemanticsToString(which));

	if(mNumMultisamples == 0)
	{
		mOwnedTexturePool[which] = new Texture2D(
				textureName,
				which, mFrameBufferResolution.x,mFrameBufferResolution.y,
				texeli,
				//no host memory needed
				false,
				//no CL interop needed (I suppose ;) )
				false,
				mUseRectangleTextures,
				//don't set data
				0,
				//no mipmaps needed
				false
		);
	}
	else
	{
		//multisample stuff desired:
		mOwnedTexturePool[which] = new Texture2DMultiSample(
				textureName,
				which, mFrameBufferResolution.x,mFrameBufferResolution.y,
				mNumMultisamples,
				texeli
		);
	}

}


void RenderTarget::attachDepthTexture(Texture2DDepth* depthTex)throw(BufferException)
{
	bindSave();

	//TODO check if texture and renderbuffer conflict when both attached;
	//maybe renderbuffer to detach before
	if( stencilTestEnabled() )
	{
		LOG<<WARNING_LOG_LEVEL<<"Stencil test is enabled while attaching a depth texture "
				"without stencil channel; It is strongly recommended to disable stencil test "
				"when rendering to this RenderTarget;\n";
		//GUARD_FRAMEBUFFER(setEnableStencilTest(false));
	}
	if(mRenderBufferFlags != NO_RENDER_BUFFER)
	{
		GUARD_FRAMEBUFFER(
				glFramebufferRenderbuffer(
						GL_FRAMEBUFFER,
						mRenderBufferAttachmentPoint,
						GL_RENDERBUFFER,
						0
						);
		);
	}


	GUARD_FRAMEBUFFER(
			glFramebufferTexture2D(
					mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					depthTex -> getTextureInfo().textureTarget,
					depthTex -> getGraphicsBufferHandle(),
					0
			)
	);

	unbindSave();
}
void RenderTarget::detachDepthTexture(Texture2DDepth* depthTex)
{
	unbindSave();

	GUARD_FRAMEBUFFER(
			glFramebufferTexture2D(
					mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
					GL_DEPTH_ATTACHMENT,
					depthTex -> getTextureInfo().textureTarget,
					0,
					0
			)
	);

	//TODO check if texture and renderbuffer conflict when both attached;
	//maybe renderbuffer to re-attach hereafter
	if(mRenderBufferFlags != NO_RENDER_BUFFER)
	{
		GUARD_FRAMEBUFFER(
			glFramebufferRenderbuffer(
					GL_FRAMEBUFFER,
					mRenderBufferAttachmentPoint,
					GL_RENDERBUFFER,
					mGLRenderBufferHandle
					);
		);
	}
	//can't track every stuff; user has to maintain his testing usage for himself
	//	if(mStencilTestEnabled)
	//	{
	//		GUARD_FRAMEBUFFER(glEnable(GL_STENCIL_TEST));
	//	}

	unbindSave();
}

void RenderTarget::bindSave()
{
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,
					&mOldReadBufferBinding);
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,
					&mOldDrawBufferBinding);

	if(
		( mIsReadFrameBuffer && ( (GLuint)mOldReadBufferBinding != mFBO ) )
		||
		(!mIsReadFrameBuffer && ( (GLuint)mOldDrawBufferBinding != mFBO ) )
	)
	{
		bind(mIsReadFrameBuffer);
	}
}
void RenderTarget::unbindSave()
{
	if(
		( mIsReadFrameBuffer && ( (GLuint)mOldReadBufferBinding != mFBO ) )
	)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER,
				mOldReadBufferBinding);
	}

	if(
		(!mIsReadFrameBuffer && ( (GLuint)mOldDrawBufferBinding != mFBO ) )
	)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
				mOldDrawBufferBinding);
	}
}

bool RenderTarget::depthTestEnabled()
{
	GLboolean depthtest;
	glGetBooleanv(GL_DEPTH_TEST,&depthtest);
	return (depthtest == GL_TRUE);
}
bool RenderTarget::stencilTestEnabled()
{
	GLboolean stenciltest;
	glGetBooleanv(GL_STENCIL_TEST,&stenciltest);
	return (stenciltest == GL_TRUE);
}

bool RenderTarget::hasDepthAttachment()
{
	bindSave();
	GLint val;
	glGetFramebufferAttachmentParameteriv(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
		&val
	);
	unbindSave();

	return (val != GL_NONE);
}
bool RenderTarget::hasStencilAttachment()
{
	bindSave();
	GLint val;
	glGetFramebufferAttachmentParameteriv(
		GL_FRAMEBUFFER,
		GL_STENCIL_ATTACHMENT,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
		&val
	);
	unbindSave();

	return (val != GL_NONE);
}

void RenderTarget::checkFrameBufferErrors()throw(BufferException)
{
	bindSave();

	//check all attached textures for "compatibility" (implementation dependent)
//	bool dataTypesInconsistent = false;
//	bool channelNumbersInconsistent = false;
//	bool resolutionsInconsitent = false;
//	bool numMultiSamplesInconsistent = false;
	//TODO continue
	for(int i= 0 ; i < __NUM_TOTAL_SEMANTICS__; i++)
	{
		if(mOwnedTexturePool[i] != 0)
		{

		}
	}

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    String statusString="";

    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
    	statusString="Framebuffer complete.";
    	break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    	statusString= "Framebuffer incomplete: Attachment is NOT complete.";
    	break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    	statusString= "Framebuffer incomplete: No image is attached to FBO.";
    	break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    	statusString= "Framebuffer incomplete: Draw buffer.";
    	break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    	statusString= "Framebuffer incomplete: Read buffer.";
    	break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
    	statusString= "Unsupported by FBO implementation.";
    	break;
    default:
    	statusString= "glCheckFramebufferStatus Unknown error.";
    	break;
    }

    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
    	LOG<<WARNING_LOG_LEVEL<<statusString<<"\n";

    	 //LOG<<ERROR_LOG_LEVEL<<statusString;
    	 //throw(BufferException(statusString));
    }
    else
    {
    	LOG<<INFO_LOG_LEVEL<<statusString<<"\n";
    }

    unbindSave();
}

}
