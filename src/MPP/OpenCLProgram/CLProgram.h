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

//#include "CLKernelArguments.h"

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
	class CLKernelWorkLoadParams
	: 	public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLKernelWorkLoadParams(
			cl_GLuint numTotalWorkItems,
			cl_GLuint targetNumWorkItemsPerWorkGroup //not guaranteed not to be altered by calculateOptimalParameters()
		);
		virtual ~CLKernelWorkLoadParams(){}

	private:
		friend class CLKernel;

		//there may be some __attribute__((reqd_work_group_size(...)))
		//definitions in the kernel; check that this doesn't conflict with the passed values;
		//also check common stuff like that the mNumWorkItemsPerWorkGroup is a power of two;
		void validateAgainst(CLKernel* kernel)throw(SimulatorException);

		cl_GLuint mNumTotalWorkItems;
		cl_GLuint mNumWorkItemsPerWorkGroup;

	};



	class CLKernel
		:public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLKernel(CLProgram* owningProgram, String kernelName, CLKernelWorkLoadParams* kernelWorkLoadParams);
		~CLKernel();
		void validate()throw(BufferException);

		//grab&modify directly
		inline CLKernelWorkLoadParams* getCLKernelWorkLoadParams()const{return mKernelWorkLoadParams;}


		//work group/item dimensions/size etc are taken from CLKernelWorkLoadParams;
		cl::Event run(std::vector<cl::Event>& EventsToWaitFor);

	protected:
		//make sure the kernels' signature fits exactly the type, number and order of values
		//to be passed; Unfortunately, the OpenCL API has no way to query parameter names or types
		//of kernel functions, hence there is a huge danger of malfunction without explicit
		//error generation; be especially careful when implementing this function,
		//ALWAYS keep it in synch with the openCL code!!!11
		//virtual void passParamsToKernel()= 0;

		friend class CLKernelArgumentBase;

		String mKernelName;
		CLKernelWorkLoadParams* mKernelWorkLoadParams;

		cl::Kernel mKernel;
	};



	class CLProgram
	:	public MPP
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLProgram(Path codeDirectory, Path mProgramCodeSubFolderName, CLKernelWorkLoadParams* kernelWorkLoadParams);
		virtual ~CLProgram();

		//inline cl::Program& getCLProgramHandle()const{return mCLProgramHandle;}
	protected:

		//called by constructor
		virtual void build();

		//throw exception if there is a detectable incompatibility between the cl kernel and the args and work load params
		//note: many stuff is not detectable because of the many missing query features
		//(e.g. param name, param type of kernel arguments) so be very careful!!
		CLKernel* createKernel(String name, CLKernelWorkLoadParams* kernelWorkLoadParams, CLKernelArguments* kernelArgs)throw(SimulatorException);

		//setup the context for template rendering:
		virtual void setupTemplateContext(TemplateContextMap& contextMap);

		virtual void validate()throw(SimulatorException);
		void writeToDisk(const String& sourceCode);


		Path mCodeDirectory;
		Path mProgramCodeSubFolderName;

		cl::Program mCLProgram;

		//CLKernelWorkLoadParams* mKernelWorkLoadParams;
		std::map<String, CLKernel*> mKernels;

	};


}
