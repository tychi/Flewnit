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
 *
 * Attention: every concrete Texture has to alloc mTextureInfo  assign this pointer also to mBufferInfo;
 * This is because some values have to be calculated in a complex manner,
 * so that there is no "on the fly" direct initialization via init-list possible
 */
class Texture: public BufferInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	Texture();
	virtual ~Texture();
	//must be implemented by concrete textures;
	virtual bool operator==(const BufferInterface& rhs) const =0;

	//virtual void loadFromFile(Path filename) throw(BufferException){throw(BufferException("concete class has no loader functionality"));}
	const TextureInfo& getTextureInfo()const{return *mTextureInfoCastPtr;}
protected:

	//this is not a "new" container member for mBufferInfo, but a pointer to mBuffferInfo, so that on does't have to cast every time ;(
	TextureInfo* mTextureInfoCastPtr;

//#	define FLEWNIT_PURE_VIRTUAL
//#		include "BufferVirtualSignatures.h"
//#	undef FLEWNIT_PURE_VIRTUAL

	//not pure, as all the same for every texture;
	virtual void generateGL()throw(BufferException);
	//not pure, may not be used (yet)
	virtual void generateCL()throw(BufferException){assert("pure CL textures not implemented; every textur is either pure GL or CL/GL shared" && 0);}
	//pure virtual, as there is difference between 2D and 3D alloc,
	//and the non-interoparable-texture types must throw an exception here;
	virtual void generateCLGL()throw(BufferException)=0;

	//the two non-symmetrci GL-only routines:
	//non-pure, as binding is all the same, only the enum is different, and that can be looked up in mTexturInfo;
	virtual void bindGL()throw(BufferException);
	//pure, as there are different glTexImage[1 2 3]+D[MultiSample]?() functions
	virtual void allocGL()throw(BufferException)=0;

	//pure, as writing via glTexSubImageXD is dependent on dimensionality
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable
	virtual void writeGL(const void* data)throw(BufferException)=0;
	//can be non-pure, as clEnqueueWriteImage is quite generic;
	virtual void writeCL(const void* data)throw(BufferException);

	//non-pure, as glGetTexImage is quite generic :); at least one generic GL function ;(
	//BUT:
	//must be overidden with an exception-throw-implementation for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable
	//must also be overridden by CubeMap class, as the read call must happen six times;
	virtual void readGL(void* data)throw(BufferException);
	//can be non-pure, as clEnqueueReadImage is quite generic;
	virtual void readCL(void* data)throw(BufferException)	;

	//as it seems that a generic copying of many texture types can happen in an agnostic way
	//via two helper FBOs, this can be non-pure;
	//BUT:
	//must be overidden with an exception-throw-implementation for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable;
	//must also be overriden by CubeMap, as there are six color attachments to to or to work with
	//glFrameBufferTextureFace... we'll see
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException);
	//can be non-pure, as clEnqueueCopyImage is quite generic;
	virtual void copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException);

	//non-pure, as glDeleteTextures() applies to every texture type :)
	virtual void freeGL()throw(BufferException);
	//non-pure, as c++-cl-object manages deletion for itself;
	//lets "implement" it directly
	virtual void freeCL()throw(BufferException){}

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
//#	include "BufferVirtualSignatures.h"


	//throw exception due to non-interoperability
	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);
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
//#	include "BufferVirtualSignatures.h"

	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);
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
//#	include "BufferVirtualSignatures.h"

	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);
};

/**
 * 	2D CubeMap Texture;
 *
 * 	Features:
 * 		-	MipMapping;
 * 	Drawbacks:
 * 		-	CL interOp <-- although possible in OpenCL, I won't implement CubeMap-interop,
 * 							due to bad mapping to the structure of this framework, and because
 * 							I don't see any useful application for having acess to cube maps in
 * 							OpenCL kernels ;(
 * 		-	array versions are possible in GL4, but not in GL3; as this is a GL3-framework (at first),
 * 			 and CubMap-arrays are quite special, this won't be supported
 * 		-	multisample versions aren't defined and don't seem to make any big sense, as cube maps aren't
 * 			 displayed directly, but looked up in a distorted way and then filtered; no, MS makes no sense here (afaik).
 *
 */
class Texture2DCube: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DCube(int quadraticSize, const TexelInfo& texeli,
			//an array containing all six images in the following order:
			// +x,-x,+y,-y,+z,-z
			const void* data =0,
			bool genMipmaps = false);
	explicit Texture2DCube(Path fileName, bool clInterOp,  bool genMipmaps = false);
	virtual ~Texture2DCube();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:
//#	include "BufferVirtualSignatures.h"

	//throw exception, as interop isn't supported (yet) by this framework
	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);

	//there are several ways to bind a cube map to an FBO
	//TODO find at least a working one, better a short/efficient one
	virtual void writeGL(const void* data)throw(BufferException);

	//must also be overridden by CubeMap class, as the read call must happen six times instead of one;
	//be sure that the data buffer is 6 times bigger than one face;
	virtual void readGL(void* data)throw(BufferException);
	//must also be overriden by CubeMap, as there are six color attachments to to or to work with
	//glFrameBufferTextureFace... we'll see
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException);

};



//TODO CONTINUE BUT NOW COFFE




}


//==========================================================================================

//following some old stuff which can be deleted; it has no use in my code any more, but helpd when developing a model, which
//feature-permutations are allowed and which not :


//enum TextureFeatureFlags
//{
//
//	/**
//	 * mutual exclusive with:
//	 *   ARRAY_TEXTURE_FLAG  <-- see above
//	 *   RECTANGLE_TEXTURE_FLAG <-- mipmapping on non-square-power-of-2-textures is useless ;(
//	 *	 MULTISAMPLE_TEXTURE_FLAG <-- as no filtering is defined on
//	 *
//	 * NOT mutual exclusive with:
//	 *   OPEN_CL_CONTEXT_TYPE_FLAG <-- but be careful to use only mipmap level 0 for sharing, as there might be driver bugs
//	 *	 CUBE_MAP_TEXTURE_FLAG 		<--filtering appropriate;
//	 */
//	MIPMAP_TEXTURE_FLAG = 1<<0,
//
//
//	/**
//	 * mutual exclusive with:
//	 * 		TEXTURE_3D_BUFFER_TYPE, <-- array of 2d is kind of 3d texture "internally", hence array of 3d doesn't exist
//	 *
//	 * 		OPEN_CL_CONTEXT_TYPE_FLAG, <-- sharing texture arrays with OpenCL is not allowed (afaik)
//	 *
//	 * 		MIPMAP_TEXTURE_FLAG, 	<-- array types "occupy" the logical mip map layers, so mip mapping is impossible
//	 * 		RECTANGLE_TEXTURE_FLAG	<-- Rectangle Tex Arrays seem not be defined in GL
//	 * 		CUBE_MAP_TEXTURE_FLAG	<-- in GL3.3, in GL4, there is a  GL_TEXTURE_CUBE_MAP_ARRAY target
//	 *
//	 * NOT mutual exclusive with:
//	 * 		MULTISAMPLE_TEXTURE_FLAG && TEXTURE_2D_BUFFER_TYPE <--  only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
//	 *
//	 */
//	ARRAY_TEXTURE_FLAG = 1<<1,
//
//	/**
//	 * mutual exclusive with:
//	 * 	TEXTURE_1D_BUFFER_TYPE
//	 *
//	 *	CUBE_MAP_TEXTURE_FLAG,	<--	no defined by GL (afaik)
//	 *	RECTANGLE_TEXTURE_FLAG, <-- no defined by GL (afaik)
//	 *	MIPMAP_TEXTURE_FLAG
//	 *
//	 *	OPEN_CL_CONTEXT_TYPE_FLAG <-- not defined in CL (afaik)
//	 *
//	 * NOT mutual exclusive with:
//	 * 	ARRAY_TEXTURE_FLAG	<--but only valid for GL_TEXTURE_2D_MULTISAMPLE_ARRAY, i.e no 1D or 3D; 1D Multisample don't exist anyway, and 3D Arrays don't exist, see above;
//	 *
//	 */
//	MULTISAMPLE_TEXTURE_FLAG = 1<<2,
//
//
//	/**
//	 * mutual exclusive with:
//	 * everything but:
//	 *
//	 * NOT mutual exclusive with:
//	 * 	TEXTURE_2D_BUFFER_TYPE
//	 * 	MIPMAP_TEXTURE_FLAG
//	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
//	 *
//	 */
//	CUBE_MAP_TEXTURE_FLAG = 1<<3,
//
//
//	/**
//	 * mutual exclusive with:
//	 * everything but:
//	 *
//	 * NOT mutual exclusive with:
//	 * 	TEXTURE_2D_BUFFER_TYPE
//	 * 	OPEN_CL_CONTEXT_TYPE_FLAG
//	 *
//	 */
//	RECTANGLE_TEXTURE_FLAG = 1<<4,
//};

