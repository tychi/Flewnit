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
#include "Util/Time/Timer.h"

#include <grantlee/engine.h>


#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>




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

}


CLKernel::~CLKernel()
{
	delete mDefaultKernelWorkLoadParams;
	delete mCLKernelArguments;
}

void CLKernel::validate()throw(SimulatorException)
{
	if( (mDefaultKernelWorkLoadParams->mNumTotalWorkItems != 0 )
		&& (mDefaultKernelWorkLoadParams->mNumWorkItemsPerWorkGroup != 0 )
	)
	{
		mDefaultKernelWorkLoadParams->validateAgainst(this);
	}

	mCLKernelArguments->validateAgainst(this);
}

cl::Event CLKernel::run(const EventVector& eventsToWaitFor) throw(SimulatorException)
{
	mCLKernelArguments->passArgsToKernel(this);

	if( (mDefaultKernelWorkLoadParams->mNumTotalWorkItems == 0 )
		|| (mDefaultKernelWorkLoadParams->mNumWorkItemsPerWorkGroup == 0 )
	)
	{
		throw(SimulatorException("mDefaultKernelWorkLoadParams to be used are invalid! provide custom params!"));
	}


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
	mProgramCodeSubFolderName(programCodeSubFolderName),
	mTimer(Timer::create())
{

	CLProgramManager::getInstance().registerCLProgram(this);
}

CLProgram::~CLProgram()
{
	BOOST_FOREACH( KernelMap::value_type & pair, mKernels)
	{
		delete pair.second;
	}

	delete mTimer;
}



void CLProgram::build()
{
	LOG<<INFO_LOG_LEVEL<<"Building program "<<getName()<<" ...\n";
	mTimer->reset();
	mTimer->start();


    String programDirectory=	(mCodeDirectory / mProgramCodeSubFolderName).string() ;
    String commonCodeSnippetsDirectory = (mCodeDirectory / Path("common")).string();

    Path completeSourceFilePath = Path(mCodeDirectory / mProgramCodeSubFolderName / Path(getName()));
    Path completePtxFilePath =
    		mCodeDirectory  / Path("__generated") /
    		Path( String("_GEN_") + getName() + String(".ptx"));

    assert("source must exists" && 	boost::filesystem::exists(	completeSourceFilePath	) );

    //{ time stamp stuff

    struct stat linuxSourceFileStat;
	int statErrorSrc = stat (completeSourceFilePath.string().c_str(), &linuxSourceFileStat);
	//int statErrorSrc = stat ("/bin", &linuxSourceFileStat);
	if (statErrorSrc != 0)
	{ assert(0&& "wtf file must exist! i tested it  with boost! may never end here!");}

	LOG<< INFO_LOG_LEVEL<<"Time of last source modification: "
			<< linuxSourceFileStat.st_mtim.tv_sec
			<<";\n";

//segfaults within clCreateProgramWithBinary for new excplicable reason :(
//hence commented out following source/binary delegation stuff :(

//    bool needToRebuildFromSource= true;
//
//    if(	boost::filesystem::exists(	completePtxFilePath	) )
//    {
//    	struct stat linuxPtxFileStat;
//
//
//    	int statErrorPtx = stat (completePtxFilePath.string().c_str(), &linuxPtxFileStat);
//    	if (statErrorPtx != 0)
//    	{ assert(0&& "wtf file must exist! i tested it  with boost! may never end here!");}
//
//    	LOG<< INFO_LOG_LEVEL<<"Time of last ptx file modification: "
//    			<< linuxPtxFileStat.st_mtim.tv_sec
//    			<<";\n";
//
//    	if(
//    		linuxSourceFileStat.st_mtim.tv_sec < linuxPtxFileStat.st_mtim.tv_sec
//    	)
//    	{
//    		LOG<< INFO_LOG_LEVEL<<"Source file is older than ptx file by"
//    				<< linuxPtxFileStat.st_mtim.tv_sec	- linuxSourceFileStat.st_mtim.tv_sec
//    				<<" seconds! Using hence ptx file!\n";
//
//    		needToRebuildFromSource = false;
//    	}
//    }
//    //} end timestamp stuff
//
//
//segfaults for new excplicable reason :(
//    if(! needToRebuildFromSource)
//    {
//    	buildProgramFromBinaryFromDisk(completePtxFilePath);
//    }
//    else
    {
        Grantlee::Engine *templateEngine = new Grantlee::Engine();
        Grantlee::FileSystemTemplateLoader::Ptr loader =
        		Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );

		loader->setTemplateDirs( QStringList()
				<< programDirectory.c_str()
				<< commonCodeSnippetsDirectory.c_str()
		);
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
					,""//,"-cl-fast-relaxed-math"
					// define later "-cl-fast-relaxed-math"

			);

			writeBinaryToDisk(completePtxFilePath);

			LOG<<INFO_LOG_LEVEL<<getName()<<": sucessfully built!\n";
		}
		catch(cl::Error err)
		{
			validate();
		}
    } //end build form source

	mTimer->stop();
	LOG<<INFO_LOG_LEVEL<<"Finished Building program "<<getName()<<" after "<<
			mTimer->getElapsedTimeInSecondsDouble() <<" seconds;\n";


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


void CLProgram::writeBinaryToDisk(Path completePtxFilePath )
{
	//code taken and adapted from http://forums.nvidia.com/index.php?showtopic=171016

	std::ofstream myfile(completePtxFilePath.string().c_str());

	cl_uint program_num_devices =
			mCLProgram.getInfo<CL_PROGRAM_NUM_DEVICES>( PARA_COMP_MANAGER->getLastCLErrorPtr() );
    if (program_num_devices == 0)
    {
      LOG<<ERROR_LOG_LEVEL<< "CLProgram "<<getName()<<": no valid binary was found;\n";
      assert(0 && "no valid binary was found");
    }

    //size_t binaries_sizes[program_num_devices];

    std::vector<size_t> binarySizes =
    		 mCLProgram.getInfo<CL_PROGRAM_BINARY_SIZES>( PARA_COMP_MANAGER->getLastCLErrorPtr() );

    std::vector<char *> binaries;
    for (size_t i = 0; i < binarySizes.size() ; i++)
          binaries.push_back( new char[ binarySizes[ i ] + 1 ] );

    *(PARA_COMP_MANAGER->getLastCLErrorPtr()) =
    	mCLProgram.getInfo(CL_PROGRAM_BINARIES, &binaries);

    //mCLProgram.getInfo<CL_PROGRAM_BINARIES>( PARA_COMP_MANAGER->getLastCLErrorPtr() );

    if(myfile.is_open())
    {
    	for (size_t i = 0; i < binaries.size(); i++)
    	{
    		myfile << binaries[i];
    	}
    }
    myfile.close();

    for (size_t i = 0; i < program_num_devices; i++)
    {
    	delete [] binaries[i];
    }



    //clGetProgramInfo(cpProgram,     CL_PROGRAM_BINARY_SIZES, program_num_devices*sizeof(size_t), binaries_sizes, NULL);

//        char **binaries = new char*[ciDeviceCount];
//
//        for (size_t i = 0; i < ciDeviceCount; i++)
//                        binaries[i] = new char[binaries_sizes[i]+1];
//
//        clGetProgramInfo(cpProgram, CL_PROGRAM_BINARIES, program_num_devices*sizeof(size_t), binaries, NULL);
//

//
//        for (size_t i = 0; i < program_num_devices; i++)
//                        delete [] binaries[i];
//
//        delete [] binaries;
}


void CLProgram::buildProgramFromBinaryFromDisk(Path completePtxFilePath)
{
	FILE* fp = fopen( completePtxFilePath.string().c_str(), "r");
	        fseek (fp , 0 , SEEK_END);
	        const size_t lSize = ftell(fp);
	        rewind(fp);
	        unsigned char* buffer;
	        buffer = (unsigned char*) malloc (lSize);
	        fread(buffer, 1, lSize, fp);
	        fclose(fp);

	cl::Program::Binaries binaries = {
			//{(const unsigned char**)&buffer, lSize }
			{& buffer, lSize }

	};


    mCLProgram = cl::Program(
    	PARA_COMP_MANAGER->getCLContext(),
  		{ PARA_COMP_MANAGER->getUsedDevice() },
  		binaries,
  		0,
  		0//PARA_COMP_MANAGER->getLastCLErrorPtr()
    );

//    mCLProgram.build(
//    	{ PARA_COMP_MANAGER->getUsedDevice() }
//    );

//	        cl_int status;
//	        cpProgram = clCreateProgramWithBinary(cxGPUContext, 1, (const cl_device_id *)cdDevices,
//	                                &lSize, (const unsigned char**)&buffer,
//	                                &status, &ciErr1);
//
//	    if (ciErr1 != CL_SUCCESS)
//	    {
//	        cout<<"Error in clCreateProgramWithBinary, Line "<<__LINE__<<" in file "<<__FILE__<<" "<<endl;
//	        Cleanup(EXIT_FAILURE);
//	    }
//
//	    ciErr1 = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
}



}
