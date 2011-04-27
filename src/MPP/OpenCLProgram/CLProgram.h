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
		CLProgram(
				Path sourceFileName,
				Path mProgramCodeSubFolderName = Path(String("")),
				Path codeDirectory = Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH ) );
		virtual ~CLProgram();

		//inline cl::Program& getCLProgramHandle()const{return mCLProgramHandle;}

		//note: in contrst to Shader class, you have to call build() by yoursefl, as otherwise
		//when calling setupTemplateContext() from build() called from base class constructor,
		//setupTemplateContext() of the derived classes will not be called correctly ;(
		virtual void build();

	protected:


		//setup the context for template rendering:
		//pure virtual because template contexts can differ considerably, and even if there is a common subset in this framework,
		//it should not count as "default" template setup; in this case, a little boilerplate is preferred to the generality
		//of this OpenCL Program class
		virtual void setupTemplateContext(TemplateContextMap& contextMap)=0;


		//Factory function; Owns the kernels, i.e. is responsible for their deletion;
		//throw exception if there is a detectable incompatibility between the cl kernel and the args and work load params
		//note: many stuff is not detectable because of the many missing query features
		//(e.g. param name, param type of kernel arguments) so be very careful!!
		CLKernel* createKernel(String name, CLKernelWorkLoadParams* kernelWorkLoadParams, CLKernelArguments* kernelArgs)throw(SimulatorException);


		virtual void validate()throw(SimulatorException);



		Path mCodeDirectory;
		Path mProgramCodeSubFolderName;

		cl::Program mCLProgram;

		//CLKernelWorkLoadParams* mKernelWorkLoadParams;
		std::map<String, CLKernel*> mKernels;

	};


}
