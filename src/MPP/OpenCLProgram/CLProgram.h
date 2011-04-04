/*
 * CLProgram.h
 *
 *  Created on: April 3, 2011
 *      Author: tychi
 *
 *  OpenCL specialization of the MPP
 */

#pragma once


#include "MPP/MPP.h"

#include "Common/FlewnitSharedDefinitions.h"
#include "Simulator/SimulatorForwards.h"



namespace Flewnit
{


	/*
	 * Base class for values relevant for OpenCL
	 * 	-	program generation (by grantlee)
	 * 	-	program compilation (by OpenCL driver)
	 * 	-	kernel invocation
	 *
	 * It is up to the derived classes, if the value is used at compile time
	 * (passed to grantlee or as compiler option string),
	 * runtime (via clSetKernelArg()) or completely ignored.
	 *
	 *
	 * Derive one Param class per Kernel class and be sure to keep it in synch with the kernel code,
	 * especially the param list;
	 */
	class CLParams
	: 	public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLParams(
				cl_GLuint numElements,
				cl_GLuint targetNumWorkItemsPerWorkGroup //not guaranteed not to be altered by calculateOptimalParameters()
		);
		virtual ~CLParams(){}

		cl_GLuint numTotalElements;
		cl_GLuint numElementsPerWorkItem; 	//"serialization amount" in order to
											//	- reduce memory needs or
											//	- do more work per item for very small kernels
											//	  to compensate kernel invocation/management overhead
		cl_GLuint numWorkGroups;
		cl_GLuint numWorkItemsPerWorkGroup;

		cl_GLuint neededLocalMemoryPerWorkItem;

		//to be overridden by special kernels if necessary
		virtual void calculateOptimalParameters();

	};

	class CLKernel
		:public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLKernel(CLProgram* owningProgram, String kernelName);
		~CLKernel();
		void validate()throw(BufferException);

		//grab&modify directly
		inline CLParams* getCLParams()const{return mParams;}


		//work group/item dimensions/size etc are taken from CLParams;
		void run();

	protected:
		//make sure the kernels' signature fits exactly the type, number and order of values
		//to be passed; Unfortunately, the OpenCL API has no way to query parameter names or types
		//of kernel functions, hence there is a huge danger of malfunction without explicit
		//error generation; be especially careful when implementing this function,
		//ALWAYS keep it in synch with the openCL code!!!11
		virtual void passParamsToKernel()=0;

		String mKernelName;
		CLParams* mParams;

		cl::Kernel mKernel;
	};



	class CLProgram
	:	public MPP
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLProgram(Path codeDirectory, Path mProgramCodeSubFolderName, CLParams* params);
		virtual ~CLProgram();

		//inline cl::Program& getCLProgramHandle()const{return mCLProgramHandle;}
	protected:

		//called by constructor
		virtual void build();

		CLKernel* createKernel(String name, CLParams* params);
		//setup the context for template rendering:
		virtual void setupTemplateContext(TemplateContextMap& contextMap);

		virtual void validate()throw(SimulatorException);
		void writeToDisk(const String& sourceCode);


		Path mCodeDirectory;
		Path mProgramCodeSubFolderName;

		cl::Program mCLProgram;

		CLParams* mParams;
		std::map<String, CLKernel*> mKernels;

	};


}
