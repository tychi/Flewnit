/*
 * RenderTarget.h
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#pragma once
#include "Common/BasicObject.h"

namespace Flewnit
{

class RenderTarget
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	//multiSample flag needed for render buffer generation
	RenderTarget(String name, bool useStencilBuffer = false, bool useMultisample = false);
	virtual ~RenderTarget();

	void bind();
	void renderToScreen();

	void setEnableDepthBuffering(bool value);

	void addTexture(Texture* tex);
	//can return NULL
	Texture* getTexture(BufferSemantics bs);
	void removeTexture(Texture* tex);

	void attachColorTexture(Texture* tex, int where);
	void attachStoredColorTexture(BufferSemantics which, int where) throw(BufferException);

	tocontinue

	void attachDepthTexture(Texture2DDepth* depthTex);
private:

	//In case of deferred rendering we need several textures for writing and reading
	//available in every simulation stage. Hence, a Rendertarget in this framework is
	//not just something to temporarily attach textures but to hold a user-defined subset
	//of them even if they are not attached;

	Texture* mOwnedTexturePool[__NUM_TOTAL_SEMANTICS__];

	//tracker, what texture is attached where
	//Initialized everywhere to INVALID_SEMANTICS
	BufferSemantics	mCurrentlyAttachedTextures[8];

	//i had no time to wrap the render buffer to my buffer concept; hope I won't need it
	//somewhere else but here; Otherwise, wrapping will follow
	GLuint mGLRenderBufferHandle;
	GLenum mGLRenderBufferType;

};

}

