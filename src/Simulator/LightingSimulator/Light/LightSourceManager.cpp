/*
 * LightSourceManager.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSourceManager.h"


#include "Buffer/BufferHelperUtils.h"
#include "Buffer/Buffer.h"

#include "LightSource.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"

#include "Simulator/LightingSimulator/RenderTarget/RenderTarget.h"

#include "MPP/Shader/ShaderManager.h"

#include <boost/foreach.hpp>
#include <sstream>
#include "MPP/Shader/Shader.h"
#include "Util/HelperFunctions.h"
#include "Simulator/OpenCL_Manager.h"
#include "Util/Log/Log.h"

#include <initializer_list>



namespace Flewnit
{

LightSourceManager::LightSourceManager()
	//mNumCurrentActiveLightingLightSources(0),
	//mNumCurrentActiveShadowingLightSources(0)
:
 mLightSourceProjectionMatrixNearClipPlane(0.1f),
 mLightSourceProjectionMatrixFarClipPlane(1000.0f),
 mLightSourcesUniformBuffer(0),
 mLightSourceBufferMetaInfo(0),
 mShadowMapMatricesUniformBuffer(0),
 mShadowMapMatrixBufferMetaInfo(0),
 mNumMaxLightSources(ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources)
 //mLightSourceBufferOffsets(0),
 //mRequiredLightSourceBufferSize(0)
{
	//all creation stuff done in init();
}

void LightSourceManager::init()
{
	if(
		(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
				== LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
	 ||	(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature
			 == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )
	)
	{
		std::vector<String> memberStrings=
			{
			  "position","diffuseColor","specularColor","direction",
			  "innerSpotCutOff_Radians","outerSpotCutOff_Radians","spotExponent","shadowMapLayer"
			};

		mLightSourceBufferMetaInfo = new UniformBufferMetaInfo(
			mNumMaxLightSources,
			String("LightSourceBuffer"), String("lightSources"),
			memberStrings,
			ShaderManager::getInstance().getUniformBufferOffsetQueryShader());

		//queryLightSourceBufferSizeAndOffsets();

		assert( (mLightSourceBufferMetaInfo->mRequiredBufferSize  % BufferHelper::elementSize(TYPE_FLOAT))== 0);
		mLightSourcesUniformBuffer = new Buffer(
			BufferInfo(
				String("LightSourceUniformBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				LIGHT_SOURCE_BUFFER_SEMANTICS,
				TYPE_FLOAT,
				mLightSourceBufferMetaInfo->mRequiredBufferSize / BufferHelper::elementSize(TYPE_FLOAT),
				BufferElementInfo(true),
				UNIFORM_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			//yes, the contents are mostly modded when moving lightsources are involved
			true,
			//set no data yet
			0
		);
	}



	if(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature
		 == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)
	{
		//TODO refactor the same way as lightSource uniform buffer;

		const cl_GLuint expectedBiggestAlignment = 128;
		//how many mat4's fit into the alignemt stride?
		//(we know that 128/(4bytePerFloat*4ElementsPerVec*4columnsPerMatrix)=2), but lets keep it general,
		//in case we wanna use ather than 32 bit base types once:
		assert(sizeof(Matrix4x4) <= expectedBiggestAlignment);
		int matricesPerAlignmentStride = expectedBiggestAlignment / sizeof(Matrix4x4);


		mShadowMapMatricesUniformBuffer = new Buffer(
			BufferInfo(
				String("ShadowMapMatricesUniformBuffer"),
				ContextTypeFlags( HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				TRANSFORMATION_MATRICES_SEMANTICS,
				TYPE_MATRIX44F,
				ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources
					//design decision: array elements must be possible to be aligned to expectedBiggestAlignment;
					//better alloc double size than provoke buffer overflow and undefined shader behaviour
					* matricesPerAlignmentStride ,
				BufferElementInfo(true),
				UNIFORM_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			//yes, the contents are mostly modded when moving lightsources are involved
			true,
			//set no data yet
			0
		);
	}
}





LightSourceManager::~LightSourceManager()
{
	for(unsigned int i=0; i< mLightSources.size();i++)
	{
		//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
		//has to remove the list-entry before actually call the destructor on the LS;
		//as calls to erase() make problems during itteration, we just set the entry to zero
		LightSource* tmp = mLightSources[i];
		mLightSources[i] = 0;
		delete tmp;
	}

//	if(mLightSourceBufferOffsets)
//	{
//		for(int lightSourceRunner=0; lightSourceRunner< mNumMaxLightSources; lightSourceRunner++)
//		{
//			delete[] mLightSourceBufferOffsets[lightSourceRunner];
//		}
//		delete[] mLightSourceBufferOffsets;
//	}

	if(mLightSourceBufferMetaInfo) delete mLightSourceBufferMetaInfo;
	if(mShadowMapMatrixBufferMetaInfo) delete mShadowMapMatrixBufferMetaInfo;

}



void LightSourceManager::queryLightSourceBufferSizeAndOffsets()
{
//	GLuint shaderGLProgramHandle = ShaderManager::getInstance().getUniformBufferOffsetQueryShader()->getGLProgramHandle();
//	GLuint lsUniBlockIndex = GUARD( glGetUniformBlockIndex(shaderGLProgramHandle, "LightSourceBuffer") );
//	//query needed buffer size
//	GUARD(
//		glGetActiveUniformBlockiv(
//			shaderGLProgramHandle,
//			lsUniBlockIndex,
//			GL_UNIFORM_BLOCK_DATA_SIZE,
//			& mRequiredLightSourceBufferSize
//		)
//	);
//
//	//--------------------------------------------------------------------------------
//
//	mLightSourceBufferOffsets = new GLint*[mNumMaxLightSources];
//
//	const String memberStrings[] =
//		{
//		  "position","diffuseColor","specularColor","direction",
//		  "innerSpotCutOff_Radians","outerSpotCutOff_Radians","spotExponent","shadowMapLayer"
//		};
//
//	const char* indexQuery_C_StringArray[FLEWNIT_NUM_LIGHTSOURCE_MEMBERS];
//	String indexQueryStringArray[FLEWNIT_NUM_LIGHTSOURCE_MEMBERS];
//	GLuint currentUniformIndices[FLEWNIT_NUM_LIGHTSOURCE_MEMBERS];
//	for(int lightSourceRunner=0; lightSourceRunner< mNumMaxLightSources; lightSourceRunner++)
//	{
//		String baseString =
//			//String("LightSourceBuffer.lightSources[")
//			String("lightSources[")
//			+ HelperFunctions::toString(lightSourceRunner)
//			+ String("].") ;
//
//
//			mLightSourceBufferOffsets[lightSourceRunner]= new GLint[FLEWNIT_NUM_LIGHTSOURCE_MEMBERS];
//			for(int memberRunner=0; memberRunner< FLEWNIT_NUM_LIGHTSOURCE_MEMBERS; memberRunner++)
//			{
//				indexQueryStringArray[memberRunner]= String(baseString+memberStrings[memberRunner]);
//				indexQuery_C_StringArray[memberRunner]= indexQueryStringArray[memberRunner].c_str();
//			}
//			//first, get indices of current lightsource members:
//			GUARD(
//				glGetUniformIndices(
//					shaderGLProgramHandle,
//					FLEWNIT_NUM_LIGHTSOURCE_MEMBERS,
//					indexQuery_C_StringArray,
//					currentUniformIndices
//				)
//			);
//
//			//second, get offset in buffer for those members, indentified by the queried indices:
//			GUARD(
//				glGetActiveUniformsiv(
//					shaderGLProgramHandle,
//					FLEWNIT_NUM_LIGHTSOURCE_MEMBERS,
//					currentUniformIndices,
//					GL_UNIFORM_OFFSET,
//					mLightSourceBufferOffsets[lightSourceRunner]
//				)
//			);
//
//
//			for(int memberRunner=0; memberRunner< FLEWNIT_NUM_LIGHTSOURCE_MEMBERS; memberRunner++)
//			{
//				LOG<<DEBUG_LOG_LEVEL << String(indexQuery_C_StringArray[memberRunner])<<" ;\n";
//				LOG<<DEBUG_LOG_LEVEL <<"uniform index: "<<  currentUniformIndices[memberRunner] <<" ;\n";
//				LOG<<DEBUG_LOG_LEVEL <<"uniform offset: "<<  mLightSourceBufferOffsets[lightSourceRunner][memberRunner] <<" ;\n";
//			}
//	}

}









//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
PointLight* LightSourceManager::createPointLight(
		const Vector4D& position,
		bool castsShadows,
		const Vector4D& diffuseColor,
		const Vector4D& specularColor
) throw(SimulatorException)
{
	LightSourcesLightingFeature lslf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature;
	assert(
			( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT)
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS )
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )) ;

	if(( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT))
	{
		assert("only one lightsource allowed by global shading features;"
				&& (mLightSources.size()==0));
	}

	assert("maximum of lightsources not reached" &&
			(getNumTotalLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources) );

	if(castsShadows)
	{
		LightSourcesShadowFeature lssf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature;
		assert( "pointlight as shadowcaster allowed" && (lssf = LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT));
		assert( "for point lights, only one shadow caster is valid" && (getNumTotalShadowingLightSources() ==0));
	}

	std::stringstream s; s<< mLightSources.size();

	mLightSources.push_back(
		new PointLight(
				String("PointLight")+ s.str(),
				castsShadows,
				true,
				LightSourceShaderStruct(
					position,diffuseColor,specularColor,
					//some ought-to be unused (besides their function as indicator that ist NOT a spot light)
					//default values
					Vector4D(0.0f,0.0f,-1.0f,0.0f),0.0f,0.0f,0.0f,0.0f)
		)
	);

	return reinterpret_cast<PointLight*> (mLightSources.back());
}


//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
SpotLight* LightSourceManager::createSpotLight(
		const Vector4D& position,
		const Vector4D& direction,
		bool castsShadows,
		float innerSpotCutOff_Degrees,
		float outerSpotCutOff_Degrees,
		float spotExponent,
		const Vector4D& diffuseColor,
		const Vector4D& specularColor
) throw(SimulatorException)
{
	LightSourcesLightingFeature lslf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesLightingFeature;
	assert(
			( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT)
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS )
		||	( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS )) ;

	if(( lslf == LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT))
	{
		assert("only one lightsource allowed by global shading features;"
				&& (mLightSources.size()==0));
	}

	assert("maximum of lightsources not reached" &&
			(getNumTotalLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxLightSources) );

	if(castsShadows)
	{
		LightSourcesShadowFeature lssf = ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature;
		assert( "spotlight as shadowcaster allowed" &&
			(	(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT) ||
				(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS)	 )
		);
		if(lssf == LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
		{
			assert( "only one shadow caster is valid" && (getNumTotalShadowingLightSources() ==0));
		}
		else
		{
			assert(getNumTotalShadowingLightSources() < ShaderManager::getInstance().getGlobalShaderFeatures().numMaxShadowCasters);
		}

	}

	std::stringstream s; s<< mLightSources.size();

	mLightSources.push_back(
		new SpotLight(
				String("SpotLight")+ s.str(),
				castsShadows,
				true,
				LightSourceShaderStruct(
					position,diffuseColor,specularColor,
					direction,
					glm::radians(innerSpotCutOff_Degrees),
					glm::radians(outerSpotCutOff_Degrees),
					spotExponent,
					//the layer is not fixed but dependent on the number of currently active shadow casters
					//on a per-frame basis;
					//but again, non-initialized members make the coder shit into his pants ;)
					static_cast<float>(getNumTotalShadowingLightSources())
				)
		)
	);

	return reinterpret_cast<SpotLight*> (mLightSources.back());
}

int LightSourceManager::getNumCurrentlyActiveLightingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if(ls->isEnabled()){cnt++;}
	}
	return cnt;
}

int LightSourceManager::getNumCurrentlyActiveShadowingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if(ls->isEnabled() && ls->castsShadows() ){cnt++;}
	}
	return cnt;
}

int LightSourceManager::getNumTotalShadowingLightSources()const
{
	int cnt=0;
	BOOST_FOREACH(LightSource* ls, mLightSources)
	{
		if( ls->castsShadows() ){cnt++;}
	}
	return cnt;
}



//fill buffers with recent values
void LightSourceManager::updateLightSourcesUniformBuffer(Camera *mainCam)
{
	if(mLightSourcesUniformBuffer)
	{
		//assuming that there is a CPU component!
		//NOTE: maybe on could just access the std::vector data and transfer it to the gpu;
		//but i'm concerned about stuff like alignment, this-pointer and other c++-meta data
		//which could corrupt a tightly-packed assumption; EDIT: yes, my concern was legitimate ;(
		unsigned char* bufferToFill = reinterpret_cast<unsigned char*>(mLightSourcesUniformBuffer->getCPUBufferHandle());
		//set everything to zero in order to omit wrong rendering in hardcoeded for-loops
		memset(bufferToFill,mLightSourcesUniformBuffer->getBufferInfo().bufferSizeInByte,0);

		unsigned int currentLightSourceUniformBufferIndex=0;
		for(unsigned int currentLightSourceHostIndex =0; currentLightSourceHostIndex < mLightSources.size(); currentLightSourceHostIndex++ )
		{
			if(mLightSources[currentLightSourceHostIndex]->isEnabled())
			{
				const LightSourceShaderStruct& lsss = mLightSources[currentLightSourceHostIndex]->getdata();

			    Vector4D lightPosViewSpace =
			    		mainCam->getGlobalTransform().getLookAtMatrix()
			    		* Vector4D( mLightSources[currentLightSourceHostIndex]->getGlobalTransform().getPosition(), 1.0f);
			    Vector4D lightDirViewSpace =
			    		mainCam->getGlobalTransform().getLookAtMatrix()
			    		* Vector4D( mLightSources[currentLightSourceHostIndex]->getGlobalTransform().getDirection(), 0.0f);


#define CURRENT_VEC4_VALUE(index) \
		reinterpret_cast<Vector4D&>( \
				bufferToFill[ \
				     mLightSourceBufferMetaInfo->mBufferOffsets[currentLightSourceUniformBufferIndex][index] \
				] \
		)
#define CURRENT_FLOAT_VALUE(index) \
		reinterpret_cast<float&>( \
				bufferToFill[ \
				     mLightSourceBufferMetaInfo->mBufferOffsets[currentLightSourceUniformBufferIndex][index] \
				] \
		)

			    CURRENT_VEC4_VALUE(0) = lightPosViewSpace;
			    CURRENT_VEC4_VALUE(1) = lsss.diffuseColor;
			    CURRENT_VEC4_VALUE(2) = lsss.specularColor;
			    CURRENT_VEC4_VALUE(3) = lightDirViewSpace;

			    CURRENT_FLOAT_VALUE(4)   =  lsss.innerSpotCutOff_Radians;
			    CURRENT_FLOAT_VALUE(5)   =  lsss.outerSpotCutOff_Radians;
			    CURRENT_FLOAT_VALUE(6)   =  lsss.spotExponent;
			    CURRENT_FLOAT_VALUE(7)   =  lsss.shadowMapLayer;

#undef CURRENT_FLOAT_VALUE
#undef CURRENT_VEC4_VALUE

				currentLightSourceUniformBufferIndex++;
			}
		} //endfor

		mLightSourcesUniformBuffer->copyFromHostToGPU();

	}
}

void LightSourceManager::updateShadowMapMatricesUniformBuffer(Camera *mainCam)
{
	return;
	//TODO
	assert(0&&"//TODO");
}




//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
//has to remove the list-entry before actually call the destructor on the LS;
void LightSourceManager::unregisterLightSource(LightSource* ls)
{
	//BOOST_FOREACH(LightSource* currentLs, mLightSources)
	for(unsigned int i=0; i< mLightSources.size();i++)
	{
		if(ls == mLightSources[i])
		{
			//this is an indicator that the manager has NOT deleted the lightsource itself;
			//otherwise, the entry where the ls was stored before would now be 0;
			mLightSources.erase( mLightSources.begin() + i);
			break;
		}
	}
}


}


/*

  lecacy stuff in init: not deleted for doc purposes:

  		//to know exactly the rquired buffer size, we would need all the compiled and linked shaders
		//using the buffer to query the buffer size; due to alignment and rearrangement
		//for memory access optimization we have to assume that the buffer object
		//consumes more memory than it would do if it were tightly packed;
		//n.b.: on the development laptop with a Geforce GT 435M with the 270.18 linux beta driver,
		//the buffer elements ARE tighly packed light in the std140 manner, but we cannot rely on
		//this for any machine and driver release;
		//at this moment, we just have to guess a size and throw an exception if the actual
		//shader requirement exceed the allocated amount;
		//Why not allocation the biggest amount posibly needed? Think about the current worst case,
		//that every single value is alinge to 16 byte for the fermi architecture;
		//for later releases, this padding for alignment might get even worse, hence there
		//is never a guarantee that the buffer is big enough; we just have to guess
		//if we don't want to precompile any possible shader permutation;
		//we assume a 128 byte alignment of array elements in the buffer, as fermi maximum coalesced reads
		//from global memory are of that size
//		const cl_GLuint expectedBiggestAlignment = 128;
//		cl_GLuint maxExpectedAlignedBufferElementSize =
//				//integer rounding to next lower multiple of expectedBiggestAlignment:
//				( sizeof(LightSourceShaderStruct) / expectedBiggestAlignment) * expectedBiggestAlignment;
//		//add one expectedBiggestAlignment if the structure is not initially a multiple
//		if( (sizeof(LightSourceShaderStruct) % expectedBiggestAlignment) > 0 )
//		{
//			//if structure size does not correspond to expected alignment stride,
//			//add one stride
//			maxExpectedAlignedBufferElementSize += expectedBiggestAlignment;
//		}

 //				mNumMaxLightSources
//				//max. expected number of floats inside a LightSourceShaderStruct:
//					* maxExpectedAlignedBufferElementSize
//					/ BufferHelper::elementSize(TYPE_FLOAT),
 *
 */
