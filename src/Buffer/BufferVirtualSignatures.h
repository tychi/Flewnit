/*
 * BufferVirtualSignatures.h
 *
 *  Created on: Jan 7, 2011
 *      Author: tychi
 *
 *   A file to omit boilerplate, as those signatures must be repeated by all buffers;
 *
 */

	//wrapper functions to GL and CL calls without any error checking,
	//i.e. semantic checks/flag delegation/verifiction must be done before those calls;
	//those routines are introduced to reduce boilerplate code;
	///\{

#ifdef FLEWNIT_PURE_VIRTUAL
#	define FLEWNIT_SIGNATURE_PURENESS_TAG =0
#else
#	define FLEWNIT_SIGNATURE_PURENESS_TAG
#endif

	virtual void generateGL()	throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void generateCL()		throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void generateCLGL()		throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;

	//the two non-symmetrci GL-only routines:
	virtual void bindGL()			throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void allocGL()			throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;

	virtual void writeGL(const void* data)throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void writeCL(const void* data)throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void readGL(void* data)		throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void readCL(void* data)		throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void copyCLFrom(const ComputeBufferHandle & bufferToCopyContentsFrom)throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void freeGL()			throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void freeCL()			throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;

//	//not needed (yet?)
//	virtual void* mapGLToHost()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* mapCLToHost()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* unmapGL()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* unmapCL()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG;

	//virtual void allocCL()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG; <-- bullshaat ;)
	//virtual void bindCL()throw(BufferException)	FLEWNIT_SIGNATURE_PURENESS_TAG; <-- bullshaat ;)
	///\}

#undef FLEWNIT_SIGNATURE_PURENESS_TAG
