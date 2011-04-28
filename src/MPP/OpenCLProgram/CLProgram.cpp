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


#include <boost/foreach.hpp>
#include <grantlee/engine.h>
#include "Util/Log/Log.h"

namespace Flewnit
{

//called by virtual void CLProgram::createKernels();
//those routines build the default arguments list;
CLKernel::CLKernel(CLProgram* owningProgram, String kernelName,
		CLKernelWorkLoadParams* defaultKernelWorkLoadParams,
		CLKernelArguments* kernelArguments)
:
	mKernelName(kernelName),
	mDefaultKernelWorkLoadParams(defaultKernelWorkLoadParams),
	mCLKernelArguments(kernelArguments)

{
	GUARD(
		mKernel = cl::Kernel(
			owningProgram->mCLProgram,
			kernelName.c_str(),
			PARA_COMP_MANAGER->getLastCLErrorPtr()
		)
	);
}


CLKernel::~CLKernel()
{
	delete mDefaultKernelWorkLoadParams;
	delete mCLKernelArguments;
}

void CLKernel::validate()throw(BufferException)
{
	//TODO
	assert(0&&"TODO implement");
}

cl::Event CLKernel::run(const EventVector& EventsToWaitFor) throw(SimulatorException)
{
	//TODO
	assert(0&&"TODO implement");
}

//run() routine for kernels with different work loads
//Calls customKernelWorkLoadParams.passArgsToKernel();
cl::Event CLKernel::run(
	const EventVector& EventsToWaitFor,
	const CLKernelWorkLoadParams& customKernelWorkLoadParams
) throw(SimulatorException)
{
	//TODO
	assert(0&&"TODO implement");
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
		Path( getName()	);

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



	GUARD(
		mCLProgram.build(
				// don't specify a specific device, the one associated to program's cl::Context is taken by default
				//^| (cl_device_id*)&devices.front(), <-- pointer to reference to nonexsiting element; this is madness,
				//although it compiles with the current setup! to be sure, rather pass a non-empty list
				{ PARA_COMP_MANAGER->getUsedDevice() }
				//{}
				// define later "-cl-fast-relaxed-math"

		)
	);

    validate();

}



void CLProgram::validate()throw(SimulatorException)
{
	//GUARD( mCLProgram.getBuildInfo(PARA_COMP_MANAGER->getUsedDevice(), PARA_COMP_MANAGER->getLastCLErrorPtr()) );

	GUARD( cl_build_status buildStatus =
		mCLProgram.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(
				PARA_COMP_MANAGER->getUsedDevice(), PARA_COMP_MANAGER->getLastCLErrorPtr())
	);

	if(buildStatus != CL_BUILD_SUCCESS)
	{
		GUARD( String buildLog =
			mCLProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(
					PARA_COMP_MANAGER->getUsedDevice(), PARA_COMP_MANAGER->getLastCLErrorPtr())
		);

		Path errorLogPath=
			mCodeDirectory  / Path("__generated") /
			Path( getName()	) / Path("__PROGRAM_BUILD_INFO_LOG") ;

		writeToDisk(buildLog,errorLogPath);

		LOG<<ERROR_LOG_LEVEL<< buildLog<<"\n";

		throw(SimulatorException(String("OpenCL program ")+ getName() + String(": Build Error!")));
	}
	else
	{
		LOG<<INFO_LOG_LEVEL<<getName()<<": sucessfully built";
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
