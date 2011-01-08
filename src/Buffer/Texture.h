/*
 * Texture.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 * \brief The base class of all buffers;
 *
 *
 * ((( the below restriction might not apply anymore ;(
 * For the sake of not too much special cases, there will be no "pure CL texture" possible
 * in this framework; It will always be a GL texture, and if supported by CL, there will be
 * an option to enable interop; )))
 *
 */

#pragma once

#include "BufferInterface.h"


namespace Flewnit
{


//enum TextureType
//{
//	NO_TEXTURE_BUFFER_TYPE 		=0,
//	TEXTURE_1D_BUFFER_TYPE		=1,
//	TEXTURE_2D_BUFFER_TYPE		=2,
//	TEXTURE_3D_BUFFER_TYPE		=3
//};

enum TextureFeatureFlags
{

	/**
	 * mutual exclusive with:
	 *   ARRAY_TEXTURE_FLAG  <-- see above
	 *   RECTANGLE_TEXTURE_FLAG <-- mipmapping on non-square-power-of-2-textures is useless ;(
	 *	 MULTISAMPLE_TEXTURE_FLAG <-- as no filtering is defined on
	 *
	 * NOT mutual exclusive with:
	 *   OPEN_CL_CONTEXT_TYPE_FLAG <-- but be careful to use only mipmap level 0 for sharing, as there might be driver bugs
	 *	 CUBE_MAP_TEXTURE_FLAG 		<--filtering appropriate;
	 */
	MIPMAP_TEXTURE_FLAG = 1<<0,


	/**
	 * mutual exclusive with:
	 * 		TEXTURE_3D_BUFFER_TYPE, <-- array of 2d is kind of 3d texture "internally", hence array of 3d doesn't exist
	 *
	 * 		OPEN_CL_CONTEXT_TYPE_FLAG, <-- sharing texture arrays with OpenCL is not allowed (afaik)
	 *
	 * 		MIPMAP_TEXTURE_FLAG, 	<-- array types "occupy" the logical mip map layers, so mip mapping is impossible
	 * 		RECTANGLE_TEXTURE_FLAG	<-- Rectangle Tex Arrays seem not be defined in GL
	 * 		CUBE_MAP_TEXTURE_FLAG	<-- in GL3.3, in GL4, there is a  GL_TEXTURE_CUBE_MAP_ARRAY target
	 *
	 * NOT mutual exclusive with:
	 * 		MULTISAMPLE_TEXTURE_FLAG && TEXTURE_2D_BUFFER_TYPE <--  only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
	 *
	 */
	ARRAY_TEXTURE_FLAG = 1<<1,

	/**
	 * mutual exclusive with:
	 * 	TEXTURE_1D_BUFFER_TYPE
	 *
	 *	CUBE_MAP_TEXTURE_FLAG,	<--	no defined by GL (afaik)
	 *	RECTANGLE_TEXTURE_FLAG, <-- no defined by GL (afaik)
	 *	MIPMAP_TEXTURE_FLAG
	 *
	 *	OPEN_CL_CONTEXT_TYPE_FLAG <-- not defined in CL (afaik)
	 *
	 * NOT mutual exclusive with:
	 * 	ARRAY_TEXTURE_FLAG	<--but only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
	 *
	 */
	MULTISAMPLE_TEXTURE_FLAG = 1<<2,


	/**
	 * mutual exclusive with:
	 * everything but:
	 *
	 * NOT mutual exclusive with:
	 * 	TEXTURE_2D_BUFFER_TYPE
	 * 	MIPMAP_TEXTURE_FLAG
	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
	 *
	 */
	CUBE_MAP_TEXTURE_FLAG = 1<<3,


	/**
	 * mutual exclusive with:
	 * everything but:
	 *
	 * NOT mutual exclusive with:
	 * 	TEXTURE_2D_BUFFER_TYPE
	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
	 *
	 */
	RECTANGLE_TEXTURE_FLAG = 1<<4,
};

/*
	MipMap
	{
		//no multisample with MipMap "per definitionem" ;)

		Array:
		{
			Texture1DArray(bool genMipmaps,int numLayers)
			Texture2DArray(bool genMipmaps,int numLayers)
		}
		no Array:
		{
			Texture1D		(bool genMipmaps);	//ocl bindung not supported :@

			Texture2D		(bool genMipmaps, bool oclbinding);
			Texture2DCube	(bool genMipmaps, bool oclbinding); // array (in GL 3.3), multisample forbidden

			Texture3D		(bool genMipmaps, bool oclbinding); //array, multisample forbidden (2D tex-arrays logically don't count as 3D tex)
		}
	}
	no MipMap:
	{
		Array:
		{
			MultiSample:
			{
				Texture2DArrayMultiSample(int numMultiSamples, int numLayers)
			}

		}
		no Array
		{
			MultiSample:
			{
				Texture2DMultisample( int numMultiSamples);
				Texture3DMultisample( int numMultiSamples);
			}
			no MultiSample
			{
				Texture2DRect	(bool oclbinding); //mipmap, array, multisample forbidden
			}
		}
	}
*/




/**
 * abstract base class for all textures
 */
class Texture: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	Texture(const BufferInfo& buffi, const TextureInfo& texi);
	virtual ~Texture();
	//must be implemented by concrete textures;
	virtual bool operator==(const BufferInterface& rhs) const =0;

	//virtual void loadFromFile(Path filename) throw(BufferException){throw(BufferException("concete class has no loader functionality"));}
	const TextureInfo& getTextureInfo()const{return mTextureInfo;}
protected:

	TextureInfo mTextureInfo;

#	define FLEWNIT_PURE_VIRTUAL
#		include "BufferVirtualSignatures.h"
#	undef FLEWNIT_PURE_VIRTUAL
};




/**
 * 	1D Texture;
 *
 * 	Features:
 * 		-	MipMapping;
* 	Drawbacks:
* 		-	no CL interop possible, opposed to CUDA :@
 */
class Texture1D: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture1D(int width, const TexelInfo& texeli, const void* data =0,  bool genMipmaps = false);
	explicit Texture1D(Path fileName,  bool genMipmaps = false);
	virtual ~Texture1D();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:
#	include "BufferVirtualSignatures.h"
};


/**
 * 	2D Texture;
 *
 * 	Features:
 * 		-	MipMapping;
 * 		-	CL interOp;
 */
class Texture2D: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2D(int width, int height, const TexelInfo& texeli, bool clInterOp, const void* data =0,  bool genMipmaps = false);
	explicit Texture2D(Path fileName, bool clInterOp, bool genMipmaps = false);
	virtual ~Texture2D();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:
#	include "BufferVirtualSignatures.h"
};


/**
 * 	3D Texture;
 *
 * 	Features:
 * 		-	MipMapping;
 * 		-	CL interOp;
 *
 * 	Drawbacks:
 * 		-	writing to 3D images in CL kernels might not work with some CL implementations ;( be careful when using this funktionality!
 * 		- 	array and multisample versions of 3D textures are forbidden (2D tex-arrays logically don't count as 3D tex for me ;) )
 */
class Texture3D: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture3D(int width, int height, int depth, const TexelInfo& texeli, bool clInterOp, const void* data =0,  bool genMipmaps = false);
	explicit Texture3D(Path fileName, bool clInterOp, bool genMipmaps = false);
	virtual ~Texture3D();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:
#	include "BufferVirtualSignatures.h"
};

/**
 * 	2D Texture;
 *
 * 	Features:
 * 		-	MipMapping;
 * 		-	CL interOp;
 */
class Texture2DCube: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DCube(int quadraticSize, const TexelInfo& texeli, bool clInterOp,
			/*an array of six void*-data pointers of appropriate size, or NULL */
			const void** data =0,
			bool genMipmaps = false);
	explicit Texture2DCube(Path fileName, bool clInterOp,  bool genMipmaps = false);
	virtual ~Texture2DCube();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:
#	include "BufferVirtualSignatures.h"
};



//TODO CONTINUE BUT NOW COFFE

//Texture1D		(bool genMipmaps);	//ocl bindung not supported :@
//
//			Texture2D		(bool genMipmaps, bool oclbinding);
//			Texture2DCube	(bool genMipmaps, bool oclbinding); // array (in GL 3.3), multisample forbidden
//
//			Texture3D		(bool genMipmaps, bool oclbinding); //array, multisample forbidden (2D tex-arrays logically don't count as 3D tex)



}

