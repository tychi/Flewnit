/*
 * BufferInterfaceSignatures.h
 *
 *  Created on: Nov 28, 2010
 *      Author: tychi
 */


//this "outsourcing" of signatures may be not such a good idea; Im' doing it the usual way ;(

//NO pragma once, as it has to be included in any class definition derived from BufferInterface;
//#pragma once


//
//#ifdef FLEWNIT_BUFFERINTERFACE_BASE_CLASS_DEFINITION
//#	define FLEWNIT_SIGNATURE_ABSTRACTNESS_TAG =0
//#else
////	define this tag to zero, as the function is really going to be implemented by the including class
//#	define FLEWNIT_SIGNATURE_ABSTRACTNESS_TAG
//#endif
//
//
//virtual bool isAllocated(ContextType contextType) FLEWNIT_SIGNATURE_ABSTRACTNESS_TAG;
