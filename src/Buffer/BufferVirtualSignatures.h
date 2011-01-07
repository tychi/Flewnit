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

	virtual void generateGL()		FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void generateCL()		FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void generateCLGL()		FLEWNIT_SIGNATURE_PURENESS_TAG;

	//the two non-symmetrci GL-only routines:
	virtual void bindGL()			FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void allocGL()			FLEWNIT_SIGNATURE_PURENESS_TAG;

	virtual void writeGL(const void* data)FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void writeCL(const void* data)FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void readGL(void* data)		FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void readCL(void* data)		FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void copyGLFrom(GraphicsBufferHandle bufferToCopyContentsFrom)FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void copyCLFrom(ComputeBufferHandle bufferToCopyContentsFrom)FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void freeGL()			FLEWNIT_SIGNATURE_PURENESS_TAG;
	virtual void freeCL()			FLEWNIT_SIGNATURE_PURENESS_TAG;

//	//not needed (yet?)
//	virtual void* mapGLToHost()FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* mapCLToHost()FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* unmapGL()FLEWNIT_SIGNATURE_PURENESS_TAG;
//	virtual void* unmapCL()FLEWNIT_SIGNATURE_PURENESS_TAG;

	//virtual void allocCL()FLEWNIT_SIGNATURE_PURENESS_TAG; <-- bullshaat ;)
	//virtual void bindCL()FLEWNIT_SIGNATURE_PURENESS_TAG; <-- bullshaat ;)
	///\}

#undef FLEWNIT_SIGNATURE_PURENESS_TAG
