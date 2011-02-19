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
RenderTarget::RenderTarget(
		String name,
		const Vector2Dui& resolution,
		//the texture type of which will be the requested created/stored color/depth textures
		TextureType textureType,
		DepthBufferFlags dbf,
		const BufferElementInfo& defaultTexelLayout,
		int numMultisamples,
		int numArrayLayers
)
: mName(name),
  mDepthBufferFlags(dbf),
  //the following two components play no role if NO_RENDERBUFFER, hence the comparison to only DEPTH_RENDER_BUFFER is correct
  mDepthAndOrStencilAttachmentPoint(
		  	( (dbf == DEPTH_RENDER_BUFFER) || (dbf == DEPTH_TEXTURE) )
			? GL_DEPTH_ATTACHMENT
			: GL_DEPTH_STENCIL_ATTACHMENT),
  mDepthBufferOrTextureInternalFormat(
			( (dbf == DEPTH_RENDER_BUFFER) || (dbf == DEPTH_TEXTURE) )
			? GL_DEPTH_COMPONENT32F
			: GL_DEPTH32F_STENCIL8),

  mOwnedDepthTexture(0),
  mOwnedGLRenderBufferHandle(0),
  mCurrentlyAttachedDepthTexture(0),

  mNumCurrentDrawBuffers(0),

  mFBO(0),
  mTextureType(textureType),
  mFrameBufferResolution(resolution),
  mDefaultTexelLayout(defaultTexelLayout),
  mNumArrayLayers(numArrayLayers),
  mNumMultisamples(numMultisamples),

  mIsReadFrameBuffer(false),
  mColorRenderingEnabled(true)

{
	for(int i= 0 ; i < __NUM_TOTAL_SEMANTICS__; i++)
	{
		mOwnedTexturePool[i] = 0;
	}
	for(int i= 0 ; i < FLEWNIT_MAX_COLOR_ATTACHMENTS; i++)
	{
		mCurrentlyAttachedColorTextures[i] = 0;
		//default stuff
		mCurrentDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}


	validateMembers();


	GUARD( glGenFramebuffers(1,&mFBO) );

	//bind();
	bindSave();

	if( 	(mDepthBufferFlags == 	DEPTH_RENDER_BUFFER)
		||	(mDepthBufferFlags == 	DEPTH_STENCIL_RENDER_BUFFER) )
	{
		createAndStoreDepthRenderBuffer();
	}
	else
	{
		if( (mDepthBufferFlags == DEPTH_TEXTURE) )
		{
			createAndStoreDepthTexture();
		}
	}


	unbindSave();
	//renderToScreen();
}

void RenderTarget::createAndStoreDepthRenderBuffer()
{
	//create the renderbuffer

	GUARD_FRAMEBUFFER( glGenRenderbuffers(1, &mOwnedGLRenderBufferHandle));
	GUARD_FRAMEBUFFER( glBindRenderbuffer(GL_RENDERBUFFER, mOwnedGLRenderBufferHandle));


	//according to the spec the following must work in any case:
	//GUARD_FRAMEBUFFER( glRenderbufferStorageMultisample(GL_RENDERBUFFER, mNumMultisamples, renderBufferInternalFormat,
	//		mFrameBufferResolution.x, mFrameBufferResolution.y) );

	//but I don't trust the implementors, so lets do it the newbie way:
	//that no array or cube map texture is requested has already been asserted;
	if(mNumMultisamples <= 1)
	{
		GUARD_FRAMEBUFFER( glRenderbufferStorage(GL_RENDERBUFFER, mDepthBufferOrTextureInternalFormat,
			mFrameBufferResolution.x, mFrameBufferResolution.y) );
	}
	else
	{
		GUARD_FRAMEBUFFER( glRenderbufferStorageMultisample(GL_RENDERBUFFER, mNumMultisamples,
			mDepthBufferOrTextureInternalFormat,
			mFrameBufferResolution.x, mFrameBufferResolution.y) );
	}

//	//attach renderbuffer to depth/stencil attachment point of the FBO
//	GUARD_FRAMEBUFFER(
//			glFramebufferRenderbuffer(
//					GL_FRAMEBUFFER,
//					mDepthAndOrStencilAttachmentPoint,
//					GL_RENDERBUFFER,
//					mOwnedGLRenderBufferHandle
//					);
//	);
	attachStoredDepthBuffer();

	GUARD_FRAMEBUFFER( glBindRenderbuffer(GL_RENDERBUFFER, 0) );
}

void RenderTarget::createAndStoreDepthTexture()
{
	//construct unique texture name
	String textureName = mName;
	textureName.append("OwnedDepthTexture");


	switch(mTextureType)
	{
	case TEXTURE_TYPE_2D:
		mOwnedDepthTexture = new Texture2DDepth(textureName,
				mFrameBufferResolution.x, mFrameBufferResolution.y,
				false, false,false);
		break;
	case TEXTURE_TYPE_2D_RECT:
		mOwnedDepthTexture = new Texture2DDepth(textureName,
				mFrameBufferResolution.x, mFrameBufferResolution.y,
				true, false,false);
		break;
	case TEXTURE_TYPE_2D_CUBE:
		mOwnedDepthTexture = new Texture2DDepthCube(textureName,
				mFrameBufferResolution.x, false);
		break;
	case TEXTURE_TYPE_2D_ARRAY:
		mOwnedDepthTexture = new Texture2DDepthArray(textureName,
				mFrameBufferResolution.x, mFrameBufferResolution.y, mNumArrayLayers,
				false);
		break;

	case TEXTURE_TYPE_2D_MULTISAMPLE:
	case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
		assert(0 &&
				"Sorry, multisample depth textures aren't currently supported;" &&
				"When rendering to multisample textures, request a multisample depth render buffer"
				"instead;");
		break;
	default:
		assert(0 && "no other depth texture type supported" );
		break;
	}

	validateTexture(mOwnedDepthTexture);
	attachStoredDepthBuffer();
}


void RenderTarget::validateMembers()throw(BufferException)
{
	assert(mFrameBufferResolution.x > 0 &&  mFrameBufferResolution.y >0 );

	assert("no depth texture types allowed as default render types, because this could lead to brainf***"
			&&  (mTextureType != TEXTURE_TYPE_2D_DEPTH)
			&&  (mTextureType != TEXTURE_TYPE_2D_CUBE_DEPTH)
			&&  (mTextureType != TEXTURE_TYPE_2D_ARRAY_DEPTH)
	);


	if(mTextureType == TEXTURE_TYPE_2D_CUBE)
	{
		assert(	(mNumArrayLayers == 1) );
		assert(	(mNumMultisamples == 1) );
		assert( "for cubemap rendering, no depthbuffer seems available; specify a cubic depth texture instead!"
			&&	(mDepthBufferFlags != 	DEPTH_RENDER_BUFFER)
			&&	(mDepthBufferFlags != 	DEPTH_STENCIL_RENDER_BUFFER)
		);
		assert(mFrameBufferResolution.x == mFrameBufferResolution.y);
		assert(BufferHelper::isPowerOfTwo(mFrameBufferResolution.x));
	}

	if( (mTextureType == TEXTURE_TYPE_2D_MULTISAMPLE)
			||
		(mTextureType == TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE)
	)
	{
		assert(
					(mNumMultisamples > 1)
				&& 	(mNumMultisamples <= FLEWNIT_MAX_MULTISAMPLES)
				&&  BufferHelper::isPowerOfTwo(mNumMultisamples)
		);
	}
	else
	{
		assert(	(mNumMultisamples == 1) );
	}


	if( (mTextureType == TEXTURE_TYPE_2D_ARRAY)
			||
		(mTextureType == TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE)
	)
	{
		assert(	(mNumArrayLayers > 1) );
		assert( "for layered rendering, no depth render buffer seems available; specify a depth texture array instead!"
			&&	(mDepthBufferFlags != 	DEPTH_RENDER_BUFFER)
			&&	(mDepthBufferFlags != 	DEPTH_STENCIL_RENDER_BUFFER)
		);
	}
	else
	{
		assert(	(mNumArrayLayers == 1) );
	}

}

void RenderTarget::validateTexture(Texture* tex, bool isDepthTex)throw(BufferException)
{
	const TextureInfo& texi = tex->getTextureInfo();

	for(int i =0; i < FLEWNIT_MAX_COLOR_ATTACHMENTS; i++)
	{
		if(mCurrentlyAttachedColorTextures[i])
		{
			const TextureInfo& texicurrent = mCurrentlyAttachedColorTextures[i]->getTextureInfo();
			assert(texicurrent.isRenderTargetCompatibleTo(texi));
		}
	}

	//ok, all tests against the other attached color textures have passed;
	//now, we have to validate against the depth buffer, if one exists, and if
	//the new texture is NOT itself a depth texture (then it would replac the old depth
	//tex and hence its state doesn't matter anymore)
	if(!isDepthTex)
	{
		if(mCurrentlyAttachedDepthTexture)
		{
			const TextureInfo& texicurrent = mCurrentlyAttachedDepthTexture->getTextureInfo();
			assert(texicurrent.isRenderTargetCompatibleTo(texi));
		}
		else
		{
			if(hasDepthAttachment())
			{
				//compare with the render buffer, i.e. the specs of the textures ownde by the RT;
				assert(
						mTextureType == texi.textureType &&
						mNumMultisamples == texi.numMultiSamples &&
						mNumArrayLayers == texi.numArrayLayers &&
						mFrameBufferResolution.x == texi.dimensionExtends.x &&
						mFrameBufferResolution.y == texi.dimensionExtends.y
				);
			}
		}
	}
	else
	{
		assert(texi.isDepthTexture);
	}
}

RenderTarget::~RenderTarget()
{
	if(mOwnedGLRenderBufferHandle != 0)
	{
		GUARD_FRAMEBUFFER( glDeleteRenderbuffers(1, &mOwnedGLRenderBufferHandle));
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
//	if(value &&   (mDepthBufferFlags != DEPTH_RENDER_BUFFER ))
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


//can return NULL
Texture* RenderTarget::getStoredColorTexture(BufferSemantics bs)
{
	return mOwnedTexturePool[bs];
}

/*
 * Getter for shaders which have to glBindFragDataLocation();
 * Throws exception when no texture with specified semantics is currently attached;
 */
int RenderTarget::getAttachmentPoint(BufferSemantics which)const throw(BufferException)
{
	for(int where = 0; where < FLEWNIT_MAX_COLOR_ATTACHMENTS; where++)
	{
		if(mCurrentlyAttachedColorTextures[where]->getTextureInfo().bufferSemantics == which)
		{
			return where;
		}
	}
	throw(BufferException("RenderTarget::getAttachmentPoint: Texture with specified semantics is not attached!"));
	return -1;
}


void RenderTarget::attachColorTexture(Texture* tex, int where)throw(BufferException)
{
	bindSave();

	assert(where < FLEWNIT_MAX_COLOR_ATTACHMENTS);

	validateTexture(tex,false);

	switch(mTextureType)
	{
	case TEXTURE_TYPE_2D:
	case TEXTURE_TYPE_2D_RECT:
		GUARD_FRAMEBUFFER(
				glFramebufferTexture2D(
						mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
						GL_COLOR_ATTACHMENT0 + where,
						tex -> getTextureInfo().textureTarget,
						tex -> getGraphicsBufferHandle(),
						0
				)
		);
		break;
	case TEXTURE_TYPE_2D_CUBE:
	case TEXTURE_TYPE_2D_ARRAY:
		//take the layered rendering-attachment function
		GUARD_FRAMEBUFFER(
				glFramebufferTexture(
						mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
						GL_COLOR_ATTACHMENT0 + where,
						tex -> getGraphicsBufferHandle(),
						0
				)
		);
		break;
	case TEXTURE_TYPE_2D_MULTISAMPLE:
	case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
			assert(0 &&
					"Sorry, multisample depth textures aren't currently supported;" &&
					"When rendering to multisample textures, request a multisample depth render buffer"
					"instead;");
			break;
	default:
		assert(0 && "no other depth texture type supported" );
		break;
	}




	if(mCurrentlyAttachedColorTextures[where] == 0)
	{
		//attach to an empty attachment point, total number of draw buffers increases;
		//otherwise, the draw buffer array doesn't change, just the texture attachment
		mCurrentDrawBuffers[mNumCurrentDrawBuffers++]= GL_COLOR_ATTACHMENT0 + where;
	}

	mCurrentlyAttachedColorTextures[where] = tex;


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
		if( mCurrentlyAttachedColorTextures[i] != 0 )
		{
			switch(mTextureType)
			{
			case TEXTURE_TYPE_2D:
			case TEXTURE_TYPE_2D_RECT:
				GUARD_FRAMEBUFFER(
						glFramebufferTexture2D(
								mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
								GL_COLOR_ATTACHMENT0 + i,
								mCurrentlyAttachedColorTextures[i] -> getTextureInfo().textureTarget,
								//zero to indicate detach
								0,
								0
						)
				);
				break;
			case TEXTURE_TYPE_2D_CUBE:
			case TEXTURE_TYPE_2D_ARRAY:
				//take the layered rendering-attachment function
				GUARD_FRAMEBUFFER(
						glFramebufferTexture(
								mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
								GL_COLOR_ATTACHMENT0 + i,
								//zero to indicate detach
								0,
								0
						)
				);
				break;
			case TEXTURE_TYPE_2D_MULTISAMPLE:
			case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
					assert(0 &&
							"Sorry, multisample depth textures aren't currently supported;" &&
							"When rendering to multisample textures, request a multisample depth render buffer"
							"instead;");
					break;
			default:
				assert(0 && "no other depth texture type supported" );
				break;
			}

			mCurrentlyAttachedColorTextures[i] = 0;
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
void RenderTarget::requestCreateAndStoreColorTexture(BufferSemantics which)throw(BufferException)
{
	if(mOwnedTexturePool[which])
	{
		LOG<<WARNING_LOG_LEVEL<<"requestCreateAndStoreTexture: Texture with BufferSemantics "
				<< BufferHelper::BufferSemanticsToString(which) << " already exists!\n";
		return;
	}

	BufferElementInfo texeli(mDefaultTexelLayout);

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
			LIGHT_SOURCE_BUFFER_SEMANTICS,
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

	if( ! (mDefaultTexelLayout == texeli))
	{
		LOG<<WARNING_LOG_LEVEL<<"requestCreateAndStoreColorTexture: "
				"Texel layout of requested texture differs from mDefaultTexelLayout;"
				"Maybe Rendering will be corrupted if you will render to attachments"
				"with different texel layouts; Test carefully the rendering to integer "
				"and/or less-than-four-component textures";
	}

	//construct unique texture name
	String textureName = mName;
	textureName.append("Owned");
	textureName.append(BufferHelper::BufferSemanticsToString(which));


	switch(mTextureType)
				{
				case TEXTURE_TYPE_2D:
				case TEXTURE_TYPE_2D_RECT:
					mOwnedTexturePool[which] = new Texture2D(
							textureName,
							which,
							mFrameBufferResolution.x,mFrameBufferResolution.y,
							texeli,
							//no host memory needed
							false,
							//no CL interop needed (I suppose ;) )
							false,
							(mTextureType==TEXTURE_TYPE_2D_RECT)?true:false,
							//don't set data
							0,
							//no mipmaps needed
							false
					);

					//Texture2D testTex;
					//testTex = (*(dynamic_cast<Texture2D*>(mOwnedTexturePool[which])));
					break;
				case TEXTURE_TYPE_2D_CUBE:
					mOwnedTexturePool[which] = new Texture2DCube(
							textureName,
							mFrameBufferResolution.x,
							texeli,
							false,
							0,
							0
					);
				case TEXTURE_TYPE_2D_ARRAY:
					//take the layered rendering-attachment function
					mOwnedTexturePool[which] = new Texture2DArray(
							textureName,
							which,
							mFrameBufferResolution.x,mFrameBufferResolution.y,
							mNumArrayLayers,
							texeli,
							false,
							0,
							false
					);
					break;
				case TEXTURE_TYPE_2D_MULTISAMPLE:
					//multisample stuff desired:
					mOwnedTexturePool[which] = new Texture2DMultiSample(
							textureName,
							which, mFrameBufferResolution.x,mFrameBufferResolution.y,
							mNumMultisamples,
							texeli
					);
					break;
				case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
						assert(0 &&
								"Sorry, multisample array textures aren't currently supported; "
								"(because the corresponding depth texture is not implemented and "
								"because there is currently no scenario where I could efficiently need this)"
								"When rendering to multisample textures, request a multisample depth render buffer"
								"instead;");
						break;
				default:
					assert(0 && "no other color texture attachment type supported" );
					break;
				}

}




//depthtex must have a depth texture type (2D, cube or array)
void RenderTarget::attachDepthTexture(Texture* depthTex)throw(BufferException)
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



	validateTexture(depthTex, true);


	if(hasDepthAttachment())
	{
		//detach current depth attachment; this shouldn't be necessary,
		//but maybe this omits some bugs.. ;(
		detachDepthBuffer();
	}


	switch(mTextureType)
	{
	case TEXTURE_TYPE_2D:
	case TEXTURE_TYPE_2D_RECT:
		GUARD_FRAMEBUFFER(
				glFramebufferTexture2D(
						mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
						GL_DEPTH_ATTACHMENT,
						depthTex -> getTextureInfo().textureTarget,
						depthTex -> getGraphicsBufferHandle(),
						0
				)
		);
		break;
	case TEXTURE_TYPE_2D_CUBE:
	case TEXTURE_TYPE_2D_ARRAY:
		//take the layered rendering-attachment function
		GUARD_FRAMEBUFFER(
				glFramebufferTexture(
						mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
						GL_DEPTH_ATTACHMENT,
						depthTex -> getGraphicsBufferHandle(),
						0
				)
		);
		break;

	case TEXTURE_TYPE_2D_MULTISAMPLE:
	case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
			assert(0 &&
					"Sorry, multisample depth textures aren't currently supported;" &&
					"When rendering to multisample textures, request a multisample depth render buffer"
					"instead;");
			break;
	default:
		assert(0 && "no other depth texture type supported" );
		break;
	}

	mCurrentlyAttachedDepthTexture = depthTex;

	unbindSave();
}

void RenderTarget::attachStoredDepthBuffer()throw(BufferException)
{
	bindSave();

	if(mDepthBufferFlags == NO_DEPTH_BUFFER)
	{
		throw(BufferException(" RenderTarget::attachStoredDepthBuffer: no depth buffer stored!"));
		return;
	}

	if(mDepthBufferFlags == DEPTH_TEXTURE)
	{
		attachDepthTexture(mOwnedDepthTexture);
	}
	else
	{
		assert(mOwnedGLRenderBufferHandle);
		//attach renderbuffer to depth/stencil attachment point of the FBO
		GUARD_FRAMEBUFFER(
				glFramebufferRenderbuffer(
						GL_FRAMEBUFFER,
						mDepthAndOrStencilAttachmentPoint,
						GL_RENDERBUFFER,
						mOwnedGLRenderBufferHandle
						);
		);
	}

	unbindSave();
}

//can return NULL
Texture* RenderTarget::getStoredDepthTexture()
{
	return mOwnedDepthTexture;
}


void RenderTarget::detachDepthBuffer()
{
	bindSave();

	if( mCurrentlyAttachedDepthTexture != 0 )
	{
		switch(mTextureType)
		{
		case TEXTURE_TYPE_2D:
		case TEXTURE_TYPE_2D_RECT:
			GUARD_FRAMEBUFFER(
					glFramebufferTexture2D(
							mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
							GL_DEPTH_ATTACHMENT,
							mCurrentlyAttachedDepthTexture->getTextureInfo().textureTarget,
							//zero to indicate detachment
							0,
							0
					)
			);
			break;
		case TEXTURE_TYPE_2D_CUBE:
		case TEXTURE_TYPE_2D_ARRAY:
			//take the layered rendering-attachment function
			GUARD_FRAMEBUFFER(
					glFramebufferTexture(
							mIsReadFrameBuffer ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER,
							GL_DEPTH_ATTACHMENT,
							//zero to indicate detachment
							0,
							0
					)
			);
			break;
		case TEXTURE_TYPE_2D_MULTISAMPLE:
		case TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE:
		case TEXTURE_TYPE_3D:
			assert(0 &&
					"Sorry, multisample depth textures aren't currently supported;" &&
					"When rendering to multisample textures, request a multisample depth render buffer"
					"instead;");
			break;
		default:
			assert(0 && "bullshit texturetype");
			break;
		}

		mCurrentlyAttachedDepthTexture = 0;
	}
	else
	{
		if(mDepthBufferFlags == NO_DEPTH_BUFFER){return;}

		if( 	(mDepthBufferFlags == DEPTH_RENDER_BUFFER)
			|| 	(mDepthBufferFlags == DEPTH_STENCIL_RENDER_BUFFER)	)
		{
			assert(mOwnedGLRenderBufferHandle);
			//attach renderbuffer to depth/stencil attachment point of the FBO
			GUARD_FRAMEBUFFER(
					glFramebufferRenderbuffer(
							GL_FRAMEBUFFER,
							mDepthAndOrStencilAttachmentPoint,
							GL_RENDERBUFFER,
							//zero to indicate unbinding
							0
					);
			);
		}

	}

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
