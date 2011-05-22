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

#include <typeinfo>


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
			Texture1D		(bool genMipmaps);	//ocl binding not supported :@

			//attention! rectangle and mip mapping mutual exclusive!
			Texture2D		(bool genMipmaps, bool oclbinding, bool rectangle);
			Texture2DCube	(bool genMipmaps);  // multisample forbidden,
												// ocl binding possible, but not supported by this framework
												//array forbidden in GL 3.3, allowed in GL4.1 <-- not supported by this framework

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
			no MultiSample:
			{
				Texture2DDepthArray(int numLayers)
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
				Texture2DDepth	( bool oclbinding, bool rectangle )
				Texture2DDepthCube() // ocl binding possible, but not supported by this framework
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
	explicit Texture(const TextureInfo& texi);
	virtual ~Texture();
	//must be implemented by concrete textures;
	virtual bool operator==(const BufferInterface& rhs) const =0;
	//must be re-implemented by concrete textures;
	//virtual bool operator==(const BufferInterface& rhs) const;

	//virtual void loadFromFile(Path filename) throw(BufferException){throw(BufferException("concete class has no loader functionality"));}
	inline const TextureInfo& getTextureInfo()const{return *mTextureInfoCastPtr;}
	//shortcut acessor:
	inline TextureType getTextureType()const{return mTextureInfoCastPtr->textureType;}


	//called initially by he concrete constructors (in super constructor, the relevant CL/GL state
	//is not available, hence the boiler plate calls);
	//for special purposes, use a self-created sampler object or set the gl params unwrapped
	virtual void setupDefaultSamplerParameters()=0;
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
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
	virtual void writeCL(const void* data)throw(BufferException);

	//non-pure, as glGetTexImage is quite generic :); at least one generic GL function ;(
	//BUT:
	//must be overidden with an exception-throw-implementation for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable
	//must also be overridden by CubeMap class, as the read call must happen six times;
	virtual void readGL(void* data)throw(BufferException);
	//can be non-pure, as clEnqueueReadImage is quite generic;
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
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
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
	virtual void copyCLFrom(ComputeBufferHandle & bufferToCopyContentsFrom)throw(BufferException);

	//non-pure, as glDeleteTextures() applies to every texture type :)
	virtual void freeGL()throw(BufferException);
	//non-pure, as c++-cl-object manages deletion for itself;
	//lets "implement" it directly
	virtual void freeCL()throw(BufferException){}

	//internal function to set the compare func etc;
	//void setupDepthTextureParameters();

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
	explicit Texture1D(String name, BufferSemantics bufferSemantics,
			int width, const BufferElementInfo& texeli,
			 bool allocHostMemory, const void* data =0,  bool genMipmaps = false);

	virtual ~Texture1D();

	virtual bool operator==(const BufferInterface& rhs) const;

	virtual void setupDefaultSamplerParameters();
protected:

#include "TextureNonCLInteropImplementations.h"


	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);
};


/**
 * 	2D Texture;
 *
 * 	Features:
 * 		-	MipMapping if not Rectangle;
 * 		-	CL interOp;
 * 		- 	Option to be a Rectangle Texture;
 */
class Texture2D: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2D(String name, BufferSemantics bufferSemantics,
			int width, int height, const BufferElementInfo& texeli,
			bool allocHostMemory, bool clInterOp,
			bool makeRectangleTex=false, const void* data =0,  bool genMipmaps = false);

	virtual ~Texture2D();

	virtual bool operator==(const BufferInterface& rhs) const;

	virtual void setupDefaultSamplerParameters();
protected:


	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);


	//protected constructor to be called by Texture2DDepth:
	explicit Texture2D(String name,
				int width, int height,
				bool rectangle,
				bool allocHostMemory, bool clInterOp);

	//protected constructor to be called by Texture1dArray()
	explicit Texture2D(const TextureInfo& texi, const void* data= 0);
};

/**
 * 	2D Depth Texture; Usable as shadow map or other depth-buffer related algorithms
 *
 * 	Features:
 * 		-	CL interOp;
 */
class Texture2DDepth: public  Texture2D
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DDepth(String name,
				int width, int height,
				bool rectangle,
				bool allocHostMemory, bool clInterOp);
	virtual ~Texture2DDepth();

	virtual void setupDefaultSamplerParameters();

	virtual bool operator==(const BufferInterface& rhs) const;

	//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
	virtual void allocGL()throw(BufferException);
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
	explicit Texture3D(String name, BufferSemantics bufferSemantics,
			int width, int height, int depth, const BufferElementInfo& texeli,
			 bool allocHostMemory, bool clInterOp, const void* data =0,  bool genMipmaps = false);
	//explicit Texture3D(Path fileName, bool clInterOp, bool genMipmaps = false);
	virtual ~Texture3D();

	virtual void setupDefaultSamplerParameters();

	virtual bool operator==(const BufferInterface& rhs) const;
protected:
//#	include "BufferVirtualSignatures.h"

	virtual void generateCLGL()throw(BufferException);
	virtual void allocGL()throw(BufferException);
	virtual void writeGL(const void* data)throw(BufferException);

	//protected constructor to be called by Texture2dArray()
	explicit Texture3D(const TextureInfo& texi, const void* data= 0);
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
	explicit Texture2DCube(String name,
			int quadraticSize, const BufferElementInfo& texeli,
			 bool allocHostMemory,
			//an array containing all six images in the following order:
			// +x,-x,+y,-y,+z,-z
			const void* data =0,
			bool genMipmaps = false);

	virtual ~Texture2DCube();

	virtual bool operator==(const BufferInterface& rhs) const;

	virtual void setupDefaultSamplerParameters();
protected:


	//protected constructor to be called by Texture2DDepth:
	explicit Texture2DCube(String name,	int quadraticSize, bool allocHostMemory);

#	include "TextureNonCLInteropImplementations.h"


	virtual void allocGL()throw(BufferException);

	virtual void writeGL(const void* data)throw(BufferException);

	//must also be overridden by CubeMap class, as the read call must happen six times instead of one;
	//be sure that the data buffer is 6 times bigger than one face;
	virtual void readGL(void* data)throw(BufferException);

	//there are several ways to bind a cube map to an FBO
	//TODO find at least a working one, better a short/efficient one
	//must also be overridden by CubeMap, as there are six color attachments instead of one
	//glFrameBufferTextureFace... we'll see
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException);

};

/**
 * 	2D Cube Depth Texture; Usable as point-light shadow map or other depth-buffer related algorithms
 *
 */
class Texture2DDepthCube: public  Texture2DCube
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DDepthCube(String name,
				int quadraticSize,
				bool allocHostMemory);
	virtual ~Texture2DDepthCube();

	virtual bool operator==(const BufferInterface& rhs) const;

	virtual void setupDefaultSamplerParameters();

	//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
	virtual void allocGL()throw(BufferException);
};




/**
 * 	1D Texture Array;
 *
 * 	Features:
 * 		-	MipMapping;
* 	Drawbacks:
* 		-	no CL interop possible (afaik)
 */
class Texture1DArray: public Texture2D
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture1DArray(String name, BufferSemantics bufferSemantics,
			int width, int numLayers,  const BufferElementInfo& texeli,
			 bool allocHostMemory, const void* data =0,  bool genMipmaps = false);

	virtual ~Texture1DArray();
public:
	virtual bool operator==(const BufferInterface& rhs) const;
protected:

#include "TextureNonCLInteropImplementations.h"


};


/**
 * 	2D Texture Array;
 *
 * 	Features:
 * 		-	MipMapping;
* 	Drawbacks:
* 		-	no CL interop possible (afaik)
 */
class Texture2DArray: public Texture3D
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DArray(String name, BufferSemantics bufferSemantics,
			int width, int height, int numLayers,  const BufferElementInfo& texeli,
			 bool allocHostMemory, const void* data =0,  bool genMipmaps = false);

	virtual ~Texture2DArray();

	virtual void setupDefaultSamplerParameters();

	virtual bool operator==(const BufferInterface& rhs) const;
protected:

	//protected constructor to be called by Texture2DDepthArray:
	explicit Texture2DArray(String name,
				int width, int height, int numLayers,
				bool allocHostMemory);

#include "TextureNonCLInteropImplementations.h"

};


/**
 * 	2D Depth Array Texture; Usable as bunch of shadow maps or other
 * 	depth-buffer related algorithms to be filled by layered rendering;
 *
 */
class Texture2DDepthArray: public  Texture2DArray
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DDepthArray(String name,
				int width, int height, int numLayers,
				bool allocHostMemory);
	virtual ~Texture2DDepthArray();

	virtual bool operator==(const BufferInterface& rhs) const;

	virtual void setupDefaultSamplerParameters();

	//override to set compare func etc; Probably it's possible to be  bypassed via sampler objects, but for now..
	virtual void allocGL()throw(BufferException);
};


/**
 * 	2D Multisample Texture;
 *
 *	Features:
 *		- antialiasing possible with an MS Texture bound to an FBO
 *		- stencil routed k-buffering possible for efficient depth peeling
 *
 * 	Drawbacks:
 * 		-	no CL interop possible (afaik)
 * 		-	no filtering, so no mipmapping
 * 		-   no explicit data transfer from CPU memory known to me ;(
 * 		- 	I don't know if a readback and/or copying between MS textures is possible;
 * 			It won't be supported; The primaray Use of an
 * 			MS Texture is as Render Target for Antialiasing and efficient depth peeling
 */
class Texture2DMultiSample
: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DMultiSample(String name, BufferSemantics bufferSemantics,
			int width, int height, int numMultiSamples,  const BufferElementInfo& texeli);

	virtual ~Texture2DMultiSample();

	virtual void setupDefaultSamplerParameters();

	virtual bool operator==(const BufferInterface& rhs) const;

protected:

	virtual void allocGL()throw(BufferException);

#	include "TextureNonCLInteropImplementations.h"

	//must be overidden with an exception-throw-implementation for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable
	virtual void writeGL(const void* data)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}
	virtual void readGL(void* data)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}

};




/**
 * 	2D Multisample Array Texture;
 *
 *	Features:
 *		- layered multisampled rendering; Don't know if this framework has any use of it, but
 *		  every technique shall be ready to use, if there is not TOO much implementation overhead du to too much special cases;
 *		- antialiasing possible with an MS Texture bound to an FBO
 *		- stencil routed k-buffering possible for efficient depth peeling
 *
 * 	Drawbacks:
 * 		-	no CL interop possible (afaik)
 * 		-	no filtering, so no mipmapping
 * 		-   no explicit data transfer from and to CPU memory known to me ;(
 * 		- 	I don't know if a readback and/or copying between MS textures is possible;
 * 			It won't be supported; The primary use of an
 * 			MS Texture is as RenderTarget for anti aliasing and efficient depth peeling
 */
class Texture2DArrayMultiSample: public Texture
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	explicit Texture2DArrayMultiSample(String name, BufferSemantics bufferSemantics,
			int width, int height, int numLayers, int numMultiSamples,  const BufferElementInfo& texeli);

	virtual ~Texture2DArrayMultiSample();

	virtual void setupDefaultSamplerParameters();

	virtual bool operator==(const BufferInterface& rhs) const;

protected:

	virtual void allocGL()throw(BufferException);

#	include "TextureNonCLInteropImplementations.h"

	//must be overidden with an exception-throw-implementation for certain concrete Texture classes
	//(namely multisample textures), which don't seem to be writable, copyable ar readable
	virtual void writeGL(const void* data)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}
	virtual void readGL(void* data)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)
	{throw(BufferException("writeGL: Multisample Texture types  don't seem to be writable, readable or copyable directly;"));}

};



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

