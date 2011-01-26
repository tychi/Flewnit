/*
 * RenderTarget.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "RenderTarget.h"

#include "Common/CL_GL_Common.h"

namespace Flewnit
{

//multiSample flag needed for render buffer generation
RenderTarget::RenderTarget(String name, const Vector2Di& resolution, bool useStencilBuffer, int numMultisamples)
: mName(name), mFrameBufferResolution(resolution), mUseStencilBuffer(useStencilBuffer), mNumMultisamples(numMultisamples)
{
	//default initialization
	mDepthTestEnabled = true;
	bool mStencilTestEnabled = mUseStencilBuffer;

	for(int i= 0 ; i < __NUM_TOTAL_SEMANTICS__; i++)
	{
		mOwnedTexturePool[i] = 0;
	}


	mNumCurrentDrawBuffers = 0;
	for(int i= 0 ; i < FLEWNIT_MAX_COLOR_ATTACHMENTS; i++)
	{
		mCurrentlyAttachedTextures[i] = INVALID_SEMANTICS;
		//default stuff
		mCurrentDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}


	glGenFramebuffers(1,&mFBO);

	bind();

	glGenRenderbuffers(1, &mGLRenderBufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, mGLRenderBufferHandle);

	GLuint mFBO;
	Vector2Di mFrameBufferResolution;

	//i had no time to wrap the render buffer to my buffer concept; hope I won't need it
	//somewhere else but here; Otherwise, wrapping will follow

	GLuint mGLRenderBufferHandle;
	//usually GL_DEPTH_COMPONENT32F with attachment type GL_DEPTH_ATTACHMENT
	//or GL_DEPTH32F_STENCIL8 with attachment type GL_DEPTH_STENCIL_ATTACHMENT ;
	GLenum mGLRenderBufferType;



}
RenderTarget::~RenderTarget()
{
	glDeleteRenderbuffers(1, &mGLRenderBufferHandle);
	glDeleteFramebuffers(1, &mFBO);
}


void RenderTarget::bind(bool forReading)
{
	glBindFramebuffer(forReading ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER, mFBO);
}

//calls bind() automatically
void renderToAttachedTextures();

void clear();

void detachAllTextures();

void RenderTarget::renderToScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setEnableDepthBuffering(bool value);
void setEnableStencilTest(bool value);

void addTexture(Texture* tex);
//can return NULL
Texture* getTexture(BufferSemantics bs);
void removeTexture(Texture* tex);

void attachColorTexture(Texture* tex, int where);
void attachStoredColorTexture(BufferSemantics which, int where) throw(BufferException);

/**
 * A texture will be created with the specified semantics if it doesn't exist;
 * If it exists, it nothing will be done but printed a warning
 */
void requestCreateAndStoreTexture(BufferSemantics which);


void attachDepthTexture(Texture2DDepth* depthTex)throw(BufferException);
void renderDepthOnly();
void detachDepthTexture();

void RenderTarget::checkFrameBufferErrors()throw(BufferException)
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    String statusString="";

    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
    	statusString="Framebuffer complete.";

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    	statusString= "Framebuffer incomplete: Attachment is NOT complete.";

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    	statusString= "Framebuffer incomplete: No image is attached to FBO.";

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    	statusString= "Framebuffer incomplete: Draw buffer.";

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    	statusString= "Framebuffer incomplete: Read buffer.";

    case GL_FRAMEBUFFER_UNSUPPORTED:
    	statusString= "Unsupported by FBO implementation.";

    default:
    	statusString= "glCheckFramebufferStatus Unknown error.";
    }

    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
    	 LOG<<ERROR_LOG_LEVEL<<statusString;
    	 throw(BufferException(statusString));
    }
}

}
