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
 *
 * FYI: There is a lot of messing around with the question "Separate stencil and depth buffers/textures or not?";
 * Strictly logically thought, I would separate them. But on the internet, there are many posts (even from the end of 2010)
 * claiming that "pure stencil buffers" don't work due to hardware and/or driver issues;
 * To be on the save side, in this Framework, stenciling with FBOs will only be supported via combined renderbuffers
 * (GL_DEPTH_STENCIL_ATTACHMENT and GL_DEPTH32F_STENCIL8), and NOT with any textures; Hence, when attaching a depth texture,
 * the more powerful renderbuffer must be explicitely detached and reattached later (will happen automatically).
 * Stencil testing must be disabled temporarily while rendering to an FBO with a depth _texture_ attached;
 *
 * TODO check if the Renderbuffer with internal type GL_DEPTH32F_STENCIL8 ca't just be left bound to the stencil attachment...
 *
 */

#pragma once



#include "Common/BasicObject.h"
#include "Common/Math.h"
#include "Buffer/BufferSharedDefinitions.h"

#define FLEWNIT_MAX_COLOR_ATTACHMENTS 8
#define FLEWNIT_MAX_MULTISAMPLES 8



namespace Flewnit
{

enum DepthBufferFlags
{
	NO_DEPTH_BUFFER,
	DEPTH_TEXTURE,
	DEPTH_RENDER_BUFFER,
	DEPTH_STENCIL_RENDER_BUFFER
};

class RenderTarget
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//multiSample flag needed already on creation for render buffer generation
	RenderTarget(
			String name,
			const Vector2Dui& resolution,
			//the texture type of which will be the requested created/stored color/depth textures
			TextureType textureType,
			DepthBufferFlags dbf = DEPTH_RENDER_BUFFER,
			const BufferElementInfo& defaultTexelLayout = BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
			int numMultisamples = 1,
			int numArrayLayers = 1
	);

	virtual ~RenderTarget();

	static bool depthTestEnabled();
	static bool stencilTestEnabled();

	static void renderToScreen();

	bool hasDepthAttachment();
	bool hasStencilAttachment();

	inline String getName()const{return mName;}
	inline TextureType getTextureType()const{return mTextureType;}
	bool isCurrentlyBound();

	void bind(bool forReading = false);
	void renderToAttachedTextures();
	void clear();


	//useful for shadowmap generation: disable unnecessary color-overhead when only depth stuff is needed
	void setEnableColorRendering(bool value);
	static void setEnableDepthTest(bool value);
	static void setEnableStencilTest(bool value);


	//throws exception if texturee type of texture is incompatible
	void attachColorTexture(Texture* tex, int where)throw(BufferException);
	void attachStoredColorTexture(BufferSemantics which, int where) throw(BufferException);
	//can return NULL
	Texture* getStoredColorTexture(BufferSemantics bs);
	//no single detaching; everything or nothing ;(
	void detachAllColorTextures();

	///\{ Stuff useful for shadowmap generation
	//depthtex must have a depth texture type (2D, cube or array)
	void attachDepthTexture(Texture* depthTex)throw(BufferException);
	//throw exception if no stored depth buffer (texture or renderbuffer) exists;
	void attachStoredDepthBuffer()throw(BufferException);
	//can return NULL
	Texture* getStoredDepthTexture();
	void detachDepthBuffer();
	///\}


	/*
	 * Getter for shaders which have to glBindFragDataLocation();
	 * Throws exception when no texture with specified semantics is currently attached;
	 */
	int getAttachmentPoint(BufferSemantics which)const throw(BufferException);

	/**
	 * A texture will be created with the specified semantics if it doesn't exist;
	 * If it exists, it nothing will be done but printed a warning
	 */
	void requestCreateAndStoreColorTexture(BufferSemantics which)throw(BufferException);
protected:
	void createAndStoreDepthRenderBuffer();
	void createAndStoreDepthTexture();
public:

	void checkFrameBufferErrors()throw(BufferException);

private:

	void validateMembers()throw(BufferException);
	void validateTexture(Texture* tex, bool isDepthTex)throw(BufferException);

	void bindSave();
	void unbindSave();
	GLint mOldReadBufferBinding;
	GLint mOldDrawBufferBinding;

	String mName;

	//In case of deferred rendering we need several textures for writing and reading
	//available in every simulation stage. Hence, a Rendertarget in this framework is
	//not just something to temporarily attach textures but to hold a user-defined subset
	//of them even if they are not attached;

	Texture* mOwnedTexturePool[__NUM_TOTAL_SEMANTICS__];
	//can be NULL if one is using a render buffer instead
	//must be of compatible texturetype with the color textures, if used


	///\{
	/*
	 * the following Texture/Renderbuffer variables are mutual exclusive:
	 * One of them must be NULL, according to mDepthBufferFlags
	 */
	DepthBufferFlags mDepthBufferFlags;
		GLenum mDepthAndOrStencilAttachmentPoint;
		GLenum mDepthBufferOrTextureInternalFormat;

	Texture* mOwnedDepthTexture;
	//i had no time to wrap the render buffer to my buffer concept; hope I won't need it
	//somewhere else but here; Otherwise, wrapping will follow
	GLuint mOwnedGLRenderBufferHandle;
	///\}

	//tracker, what texture is attached where
	Texture*	mCurrentlyAttachedColorTextures[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	//is NULL if non attached or mOwnedDepthTexture if its own is attached
	Texture* 	mCurrentlyAttachedDepthTexture;
	GLenum	mCurrentDrawBuffers[FLEWNIT_MAX_COLOR_ATTACHMENTS];
	GLsizei mNumCurrentDrawBuffers;


	GLuint mFBO;
	TextureType mTextureType;
	Vector2Dui mFrameBufferResolution;
	BufferElementInfo mDefaultTexelLayout;
	int mNumArrayLayers;
	int mNumMultisamples;
	//for copying between textures, one can use glBlitFrameBuffers();
	//this function need a read buffer, hence we have to keep track;

	bool mIsReadFrameBuffer;
	bool mColorRenderingEnabled; //for shadowMap generation

	//won't work supposedly with current hardware/drivers
//	GLuint mGLRenderBufferStencilHandle;
//	GLenum mGLRenderBufferStencilType; //usually GL_STENCIL_INDEX16

};

}

