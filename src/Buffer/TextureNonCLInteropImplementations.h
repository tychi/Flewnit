/*
 * TextureNonCLInteropImplementations.h
 *
 *  Created on: Jan 11, 2011
 *      Author: tychi
 */



	//throw exception due to non-interoperability
	virtual void generateCLGL()throw(BufferException)
	{
		throw(BufferException("Thie Texture type has no interop possibility with OpenCL yet,"
					"although for some types it is possible in CUDA! This is frustrating, but we have to accept it ;(."));
	}
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
	virtual void writeCL(const void* data)throw(BufferException)
	{
		throw(BufferException("Thie Texture type has no interop possibility with OpenCL yet,"
					"although for some types it is possible in CUDA! This is frustrating, but we have to accept it ;(."));
	}
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
	virtual void readCL(void* data)throw(BufferException)
	{
		throw(BufferException("Thie Texture type has no interop possibility with OpenCL yet,"
					"although for some types it is possible in CUDA! This is frustrating, but we have to accept it ;(."));
	}
	//must be implemented with  an exception-throw for certain concrete Texture classes
	//which don't have interop features
	virtual void copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)throw(BufferException)
	{
		throw(BufferException("Thie Texture type has no interop possibility with OpenCL yet,"
					"although for some types it is possible in CUDA! This is frustrating, but we have to accept it ;(."));
	}
