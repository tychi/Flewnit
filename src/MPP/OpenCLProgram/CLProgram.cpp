/*
 * Shader.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "CLProgram.h"

#include "CLKernelArguments.h"

#include "MPP/OpenCLProgram/CLProgramManager.h"

#include "Simulator/ParallelComputeManager.h"

#include "Util/Log/Log.h"

#include <grantlee/engine.h>

#include <boost/foreach.hpp>



namespace Flewnit
{

//called by virtual void CLProgram::createKernels();
//those routines build the default arguments list;
CLKernel::CLKernel(CLProgram* owningProgram, String kernelName,
		CLKernelWorkLoadParams* defaultKernelWorkLoadParams,
		CLKernelArguments* kernelArguments) throw(SimulatorException)
:
	mKernelName(kernelName),
	mDefaultKernelWorkLoadParams(defaultKernelWorkLoadParams),
	mCLKernelArguments(kernelArguments)

{
	try
	{
		mKernel = cl::Kernel(
			owningProgram->mCLProgram,
			kernelName.c_str(),
			PARA_COMP_MANAGER->getLastCLErrorPtr()
		);
	}
	catch(cl::Error)
	{

		throw(SimulatorException(String("something went wrong when creating the CL kernel ") + kernelName));
	}

	validate();

	mDefaultKernelWorkLoadParams->validateAgainst(this);
}


CLKernel::~CLKernel()
{
	delete mDefaultKernelWorkLoadParams;
	delete mCLKernelArguments;
}

void CLKernel::validate()throw(SimulatorException)
{
	mDefaultKernelWorkLoadParams->validateAgainst(this);
	mCLKernelArguments->validateAgainst(this);
}

cl::Event CLKernel::run(const EventVector& eventsToWaitFor) throw(SimulatorException)
{
	mCLKernelArguments->passArgsToKernel(this);

	*(PARA_COMP_MANAGER->getLastCLErrorPtr())
		=
		PARA_COMP_MANAGER->getCommandQueue().enqueueNDRangeKernel(
			mKernel,
			cl::NullRange,
			cl::NDRange( (size_t) ( mDefaultKernelWorkLoadParams->mNumTotalWorkItems) ),
			cl::NDRange( (size_t) ( mDefaultKernelWorkLoadParams->mNumWorkItemsPerWorkGroup) ),
			& eventsToWaitFor,
			//PARA_COMP_MANAGER->getLastEventPtr()
			& mEventOfLastKernelExecution
		);

	//assign this event to global last event, if anyone is interested in it (don't know yet ;( );
	*(PARA_COMP_MANAGER->getLastEventPtr()) = mEventOfLastKernelExecution;

	return mEventOfLastKernelExecution;
;
}

//run() routine for kernels with different work loads
//Calls customKernelWorkLoadParams.passArgsToKernel();
cl::Event CLKernel::run(
	const EventVector& eventsToWaitFor,
	const CLKernelWorkLoadParams& customKernelWorkLoadParams
) throw(SimulatorException)
{
	customKernelWorkLoadParams.validateAgainst(this);

	mCLKernelArguments->passArgsToKernel(this);

	*(PARA_COMP_MANAGER->getLastCLErrorPtr())
			=
		PARA_COMP_MANAGER->getCommandQueue().enqueueNDRangeKernel(
			mKernel,
			cl::NullRange,
			cl::NDRange( customKernelWorkLoadParams.mNumTotalWorkItems ),
			cl::NDRange( customKernelWorkLoadParams.mNumWorkItemsPerWorkGroup ),
			& eventsToWaitFor,
			//PARA_COMP_MANAGER->getLastEventPtr()
			& mEventOfLastKernelExecution
		);

	//assign this event to global last event, if anyone is interested in it (don't know yet ;( );
	*(PARA_COMP_MANAGER->getLastEventPtr()) = mEventOfLastKernelExecution;

	return mEventOfLastKernelExecution;
}





//#################################################################################################

CLProgram::CLProgram(
		Path sourceFileName,
		SimulationDomain sd,
		Path codeDirectory,
		Path programCodeSubFolderName
)
: MPP(sourceFileName.string(), sd),
	mCodeDirectory(codeDirectory),
	mProgramCodeSubFolderName(programCodeSubFolderName)
{

	CLProgramManager::getInstance().registerCLProgram(this);
}

CLProgram::~CLProgram()
{
	BOOST_FOREACH( KernelMap::value_type & pair, mKernels)
	{
		delete pair.second;
	}
}



void CLProgram::build()
{
    Grantlee::Engine *templateEngine = new Grantlee::Engine();
    Grantlee::FileSystemTemplateLoader::Ptr loader =
    		Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );

    String programDirectory=	(mCodeDirectory / mProgramCodeSubFolderName).string() ;
    String commonCodeSnippetsDirectory = (mCodeDirectory / Path("common")).string();
    loader->setTemplateDirs( QStringList() << programDirectory.c_str() << commonCodeSnippetsDirectory.c_str());
    templateEngine->addTemplateLoader(loader);

    //setup the context to delegate template rendering according to the shaderFeatures (both local and global):
    TemplateContextMap contextMap;
    setupTemplateContext(contextMap);


    //---------------------
    //generate shader stage source code:
	Grantlee::Template shaderTemplate = templateEngine->loadByName( getName().c_str() );
	Grantlee::Context shaderTemplateContext(contextMap);
    String programSourceCode = shaderTemplate->render(&shaderTemplateContext).toStdString();

	Path generatedProgPath=
		mCodeDirectory  / Path("__generated") /
		Path( String("_GEN_") + getName()	);

	writeToDisk(programSourceCode, generatedProgPath);

	cl::Program::Sources sources = { {programSourceCode.c_str(), (programSourceCode.size())} };

    //create OpenCL Program form generated source code:
	GUARD(
		mCLProgram = cl::Program(
			PARA_COMP_MANAGER->	getCLContext(),
			cl::Program::Sources
				{	//initializer list for VECTOR_CLASS<std::pair<const char*, ::size_t> > .. wtf;
					//so much for the simplicity of the c++ API of CL
					{ programSourceCode.c_str(), programSourceCode.size() }
				},
			PARA_COMP_MANAGER->getLastCLErrorPtr()
		)
	);



	try
	{
		mCLProgram.build(
				// don't specify a specific device, the one associated to program's cl::Context is taken by default
				//^| (cl_device_id*)&devices.front(), <-- pointer to reference to nonexsiting element; this is madness,
				//although it compiles with the current setup! to be sure, rather pass a non-empty list
				{ PARA_COMP_MANAGER->getUsedDevice() }
				//{}
				// define later "-cl-fast-relaxed-math"

		);

		LOG<<INFO_LOG_LEVEL<<getName()<<": sucessfully built!\n";
	}
	catch(cl::Error err)
	{
		validate();
	}


}



void CLProgram::validate()throw(SimulatorException)
{
	cl_build_status buildStatus =
		mCLProgram.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(
				PARA_COMP_MANAGER->getUsedDevice(), PARA_COMP_MANAGER->getLastCLErrorPtr());

	if(buildStatus != CL_BUILD_SUCCESS)
	{
		//GUARD(
		String buildLog = mCLProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(
					PARA_COMP_MANAGER->getUsedDevice(), PARA_COMP_MANAGER->getLastCLErrorPtr());
		//);

		Path errorLogPath=
			mCodeDirectory  / Path("__generated") /
			Path( String("INFO_LOG_") + getName() ) ;

		writeToDisk(buildLog,errorLogPath);

		LOG<<ERROR_LOG_LEVEL<< buildLog<<"\n";

		throw(SimulatorException(String("OpenCL program ")+ getName() + String(": Build Error!")));
	}


}



CLKernel* CLProgram::getKernel(String name)throw(SimulatorException)
{
	if(mKernels.find(name) == mKernels.end())
	{
		throw(SimulatorException(
				String("CLProgram: ") + getName()+
				String(": Kernel with specified name ")+ name + String(" doesn't exist!") ));
	}

	return mKernels[name];
}



}
