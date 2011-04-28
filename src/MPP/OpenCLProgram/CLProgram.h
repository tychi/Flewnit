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

#include "Simulator/SimulatorMetaInfo.h"


//#include "CLKernelArguments.h"

namespace Flewnit
{

	typedef std::vector<cl::Event> EventVector;


	class CLKernel
		:public BasicObject
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		~CLKernel();

		void validate()throw(SimulatorException);

		//grab&modify directly; will be set resp. verified before every kernal launch
		inline CLKernelArguments* getCLKernelArguments(){return mCLKernelArguments;}
		inline CLKernelWorkLoadParams* getCLKernelWorkLoadParams()const{return mDefaultKernelWorkLoadParams;}


		//{ run routines
			//work group/item dimensions/size etc are taken from kernelWorkLoadParams resp. mKernelWorkLoadParams;
			//run() routine for kernels with always the same work load (radix sort, uniform grid updated,
			//stream compaction etc.);
			//Calls mCLKernelArguments->passArgsToKernel();
			cl::Event run(const EventVector* eventsToWaitFor) throw(SimulatorException);
			//run() routine for kernels with different work loads
			//Uses customKernelWorkLoadParams insead of its member to define work group sizes etc;
			cl::Event run(
					const EventVector* eventsToWaitFor,
					const CLKernelWorkLoadParams& customKernelWorkLoadParams
			) throw(SimulatorException);
		//}


		//called by virtual void CLProgram::createKernels();
		//those routines build the default arguments list;
		CLKernel(CLProgram* owningProgram, String kernelName,
				CLKernelWorkLoadParams* defaultKernelWorkLoadParams,
				CLKernelArguments* kernelArguments);

	protected:
		friend class CLProgram;
		friend class CLKernelArgumentBase;
		friend class CLKernelArguments;
		friend class CLKernelWorkLoadParams;


		//called by CLProgram::virtual void createKernels() factory functions;
		//virtual void createKernelArguments()=0;
		//called by run() routines to potentially update arguments (toggle ping pong buffers etc)
		//virtual void setupKernelArguments()=0;

		String mKernelName;

		CLKernelWorkLoadParams* mDefaultKernelWorkLoadParams;
		CLKernelArguments* mCLKernelArguments;

		cl::Kernel mKernel;
	};



	class CLProgram
	:	public MPP
	{
		FLEWNIT_BASIC_OBJECT_DECLARATIONS;
	public:
		CLProgram(
				Path sourceFileName,
				SimulationDomain sd = GENERIC_SIM_DOMAIN,
				Path codeDirectory = Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH ),
				Path programCodeSubFolderName = Path(String(""))
		);
		virtual ~CLProgram();

		//inline cl::Program& getCLProgramHandle()const{return mCLProgramHandle;}

		//note: in contrast to Shader class, you have to call build() by yoursefl, as otherwise
		//when calling setupTemplateContext() from build() called from base class constructor,
		//setupTemplateContext() of the derived classes will not be called correctly ;(
		virtual void build();


		CLKernel* getKernel(String name)throw(SimulatorException);

		//issue the several createKernel() calls with initial argument list etc;
		virtual void createKernels()=0;

	protected:
		friend class CLKernel;

		void validate()throw(SimulatorException);

		//setup the context for template rendering:
		//pure virtual because template contexts can differ considerably, and even if there is a common subset in this framework,
		//it should not count as "default" template setup; in this case, a little boilerplate is preferred to the generality
		//of this OpenCL Program class
		virtual void setupTemplateContext(TemplateContextMap& contextMap)=0;


		Path mCodeDirectory;
		Path mProgramCodeSubFolderName;

		cl::Program mCLProgram;


		typedef std::map<String, CLKernel*> KernelMap;
		KernelMap mKernels;

	private:


	};


}



//legacy TODO delete

//Factory function;
//called byconstructors of derived classes;
//Owns the kernels, i.e. is responsible for their deletion;
//throw exception if there is a detectable incompatibility between the cl kernel and the args and work load params
//note: many stuff is not detectable because of the many missing query features
//(e.g. param name, param type of kernel arguments) so be very careful!!
//CLKernel* createKernel(String name, CLKernelWorkLoadParams* kernelWorkLoadParams, CLKernelArguments* kernelArgs)throw(SimulatorException);
