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

	class CLKernel
		:public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		~CLKernel();
		void validate()throw(BufferException);

		//grab&modify directly; will be set resp. verified before every kernal launch
		inline CLKernelArguments* getCLKernelArguments(){return mCLKernelArguments;}
		inline CLKernelWorkLoadParams* getCLKernelWorkLoadParams()const{return mKernelWorkLoadParams;}


		//work group/item dimensions/size etc are taken from kernelWorkLoadParams resp. mKernelWorkLoadParams;
		typedef std::vector<cl::Event> EventVector;
		cl::Event run(
				const EventVector& EventsToWaitFor,
				//zero indicates that the kernel shall use its own member work load params;
				CLKernelWorkLoadParams* kernelWorkLoadParams = 0,
				//zero indicates that the kernel shall use its own member kernel args;
				CLKernelArguments* kernelArgs = 0
		) throw(SimulatorException);

	protected:
		friend class CLProgram;
		friend class CLKernelArguments;
		friend class CLKernelArgumentBase;

		//called by CLProgram::createKernel() factory function;
		CLKernel(CLProgram* owningProgram, String kernelName,
				CLKernelWorkLoadParams* kernelWorkLoadParams,
				CLKernelArguments* clKernelArguments);




		String mKernelName;

		CLKernelArguments* mCLKernelArguments;
		CLKernelWorkLoadParams* mKernelWorkLoadParams;

		cl::Kernel mKernel;
	};



	class CLProgram
	:	public MPP
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLProgram(Path codeDirectory, Path mProgramCodeSubFolderName);
		virtual ~CLProgram();

		//inline cl::Program& getCLProgramHandle()const{return mCLProgramHandle;}
	protected:

		//called by constructor
		virtual void build();

		//Factory function; Owns the kernels, i.e. is responsible for their deletion;
		//throw exception if there is a detectable incompatibility between the cl kernel and the args and work load params
		//note: many stuff is not detectable because of the many missing query features
		//(e.g. param name, param type of kernel arguments) so be very careful!!
		CLKernel* createKernel(String name, CLKernelWorkLoadParams* kernelWorkLoadParams, CLKernelArguments* kernelArgs)throw(SimulatorException);

		//setup the context for template rendering:
		//non-pure virtual so that derived programs can replace or complement template info;
		virtual void setupTemplateContext(TemplateContextMap& contextMap);

		virtual void validate()throw(SimulatorException);



		Path mCodeDirectory;
		Path mProgramCodeSubFolderName;

		cl::Program mCLProgram;

		//CLKernelWorkLoadParams* mKernelWorkLoadParams;
		std::map<String, CLKernel*> mKernels;

	};


}
