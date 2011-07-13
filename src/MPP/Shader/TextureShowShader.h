/*
 * TextureShowShader.h
 *
 * Simple Shader to show a texture on the screen via a full screen quad.
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */


#pragma once


#include "Shader.h"

namespace Flewnit
{

class TextureShowShader
: public Shader
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	virtual ~TextureShowShader();


	virtual void use(SubObject* so)throw(SimulatorException)
	{
		throw(SimulatorException("don't use this routine in this shader"));
	}

	//@param layerToShow relevant for array or 3D- textures: provide layer resp texture coord to show
	//		 a specific slice;
	void use(Texture* tex, float layerOrTexcoord = 0.0f);

	//friend class ShaderManager;
	TextureShowShader( TextureType texType = TEXTURE_TYPE_2D);

	virtual void build();

protected:
	TextureType mTextureType;


	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	virtual void bindFragDataLocations(RenderTarget* rt) throw(BufferException);
};


}
