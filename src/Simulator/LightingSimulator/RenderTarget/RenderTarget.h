/*
 * RenderTarget.h
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 *
 * Class wrapping an OpenGL Framebuffer Object with optional maintainance capabilities for textures;
 *
 * You can request the creation of textures with desired semantics, and the internal format
 * (data types, number of channels, precision, normalization etc.) will be handled for you.
 * The precision is mostly 32bit per channel, but can be less, e.g. for normal map semantics;
 * Will be subject to experimentation.
 */

#pragma once



#include "Common/BasicObject.h"

#define FLEWNIT_MAX_COLOR_ATTACHMENTS 8

namespace Flewnit
{

class RenderTarget
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//multiSample flag needed for render buffer generation
	RenderTarget(String name, const Vector2Di& resolution, bool useStencilBuffer = false, int numMultisamples = 0);
	virtual ~RenderTarget();


	void bind(bool forReading = false);
	//calls bind() automatically

	void renderToAttachedTextures();

	void clear();

	void detachAllTextures();
	void renderToScreen();

	void setEnableDepthBuffering(bool value);
	void setEnableStencilTest(bool value);

	void addTexture(Texture* tex);
	//can return NULL
	Texture* getTexture(BufferSemantics bs);
	void removeTexture(Texture* tex);

	void attachColorTexture(Texture* tex, int where);
	void attachStoredColorTexture(BufferSemantics which, int where) throw(BufferException);

	/*
	 * Getter for shaders which have to glBindFragDataLocation();
	 * Throws exception when no texture with specified semantics is currently attached;
	 */
	int getAttachmentPoint(BufferSemantics which)const throw(BufferException);

	/**
	 * A texture will be created with the specified semantics if it doesn't exist;
	 * If it exists, it nothing will be done but printed a warning
	 */
	void requestCreateAndStoreTexture(BufferSemantics which);


	///\{ Stuff useful for shadowmap generation
	void attachDepthTexture(Texture2DDepth* depthTex)throw(BufferException);
	void renderDepthOnly();
	void detachDepthTexture();
	///\}

	void checkFrameBufferErrors()throw(BufferException);

	inline String getName()const{return mName;}

private:

	String mName;

	//In case of deferred rendering we need several textures for writing and reading
	//available in every simulation stage. Hence, a Rendertarget in this framework is
	//not just something to temporarily attach textures but to hold a user-defined subset
	//of them even if they are not attached;

	Texture* mOwnedTexturePool[__NUM_TOTAL_SEMANTICS__];

	//tracker, what texture is attached where
	//Initialized everywhere to INVALID_SEMANTICS
	BufferSemantics	mCurrentlyAttachedTextures[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	GLenum	mCurrentDrawBuffers[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	GLsizei mNumCurrentDrawBuffers;

	GLuint mFBO;
	Vector2Di mFrameBufferResolution;

	//i had no time to wrap the render buffer to my buffer concept; hope I won't need it
	//somewhere else but here; Otherwise, wrapping will follow

	GLuint mGLRenderBufferHandle;
	//usually GL_DEPTH_COMPONENT32F with attachment type GL_DEPTH_ATTACHMENT
	//or GL_DEPTH32F_STENCIL8 with attachment type GL_DEPTH_STENCIL_ATTACHMENT ;
	GLenum mGLRenderBufferType;

	bool mUseStencilBuffer;
	int  mNumMultisamples;

	bool mDepthTestEnabled;
	bool mStencilTestEnabled;



	//won't work supposedly with current hardware/drivers
//	GLuint mGLRenderBufferStencilHandle;
//	GLenum mGLRenderBufferStencilType; //usually GL_STENCIL_INDEX16

};

}

