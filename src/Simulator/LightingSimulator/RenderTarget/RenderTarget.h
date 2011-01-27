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
#include "Common/Math.h"
#include "Buffer/BufferSharedDefinitions.h"

#define FLEWNIT_MAX_COLOR_ATTACHMENTS 8
#define FLEWNIT_MAX_MULTISAMPLES 8



namespace Flewnit
{

enum RenderBufferFlags
{
	NO_RENDER_BUFFER,
	DEPTH_RENDER_BUFFER,
	DEPTH_STENCIL_RENDER_BUFFER
};

class RenderTarget
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//multiSample flag needed already on creation for render buffer generation
	RenderTarget(String name, const Vector2Di& resolution,
			bool useRectangleTextures,
			//List< Pair<BufferSemantics, int> > initiallyDesiredStoredTextures = { {FINAL_RENDERING_SEMANTICS,0} } ,
			RenderBufferFlags rbf = DEPTH_RENDER_BUFFER,
			int numMultisamples = 0);
	virtual ~RenderTarget();

	static bool depthTestGloballyEnabled();
	static bool stencilTestGloballyEnabled();

	inline String getName()const{return mName;}

	void bind(bool forReading = false);
	//calls bind() automatically

	void renderToAttachedTextures();

	void clear();

	void detachAllColorTextures();
	//no single detaching; everything or nothing ;(
	//void detachColorTexture(BufferSemantics which);

	void renderToScreen();

	//useful for shadowmap generation: disable unnecessary color-overhead when only depth stuff is needed
	void setEnableColorRendering(bool value);
	void setEnableDepthTest(bool value);
	//throw exception if a stencil buffer wasn't specified on RenderTarget creation;
	void setEnableStencilTest(bool value) throw(BufferException);

	//TODO implement IF needed
	//void addTexture(Texture* tex);
	//void removeTexture(Texture* tex);

	//can return NULL
	Texture* getStoredTexture(BufferSemantics bs);


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
	void requestCreateAndStoreTexture(BufferSemantics which)throw(BufferException);


	///\{ Stuff useful for shadowmap generation
	void attachDepthTexture(Texture2DDepth* depthTex)throw(BufferException);
	void detachDepthTexture(Texture2DDepth* depthTex);
	///\}

	//TODO if needed provide some explicit cubemap attachment stuff for dynamic cube maps and point light shadow maps :D ;)


	void checkFrameBufferErrors()throw(BufferException);



private:

	String mName;

	//In case of deferred rendering we need several textures for writing and reading
	//available in every simulation stage. Hence, a Rendertarget in this framework is
	//not just something to temporarily attach textures but to hold a user-defined subset
	//of them even if they are not attached;

	Texture* mOwnedTexturePool[__NUM_TOTAL_SEMANTICS__];

	//tracker, what texture is attached where
	//Initialized everywhere to INVALID_SEMANTICS
	Texture*	mCurrentlyAttachedTextures[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	GLenum	mCurrentDrawBuffers[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	GLsizei mNumCurrentDrawBuffers;

	GLuint mFBO;
	Vector2Di mFrameBufferResolution;
	//for copying between textures, one can use glBlitFrameBuffers();
	//this function need a read buffer, hence we have to keep track;
	bool mIsReadFrameBuffer;

	//i had no time to wrap the render buffer to my buffer concept; hope I won't need it
	//somewhere else but here; Otherwise, wrapping will follow

	GLuint mGLRenderBufferHandle;
	//usually GL_DEPTH_COMPONENT32F with attachment type GL_DEPTH_ATTACHMENT
	//or GL_DEPTH32F_STENCIL8 with attachment type GL_DEPTH_STENCIL_ATTACHMENT ;
	//GLenum mGLRenderBufferType;


	bool mUseRectangleTextures;
	RenderBufferFlags mRenderBufferFlags;
	GLenum mRenderBufferAttachmentPoint;
	GLenum mRenderBufferInternalFormat;

	int  mNumMultisamples;



	bool mColorRenderingEnabled; //for shadowMap generation
	bool mDepthTestEnabled;
	bool mStencilTestEnabled;



	//won't work supposedly with current hardware/drivers
//	GLuint mGLRenderBufferStencilHandle;
//	GLenum mGLRenderBufferStencilType; //usually GL_STENCIL_INDEX16

};

}

