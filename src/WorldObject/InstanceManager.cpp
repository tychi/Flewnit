/*
 * InstanceManager.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: tychi
 */

#include "InstanceManager.h"

#include "WorldObject/SubObject.h"
#include "Simulator/SimulationResourceManager.h"
#include "Buffer/BufferHelperUtils.h"
#include "Material/VisualMaterial.h"
#include "MPP/Shader/Shader.h"
#include "Buffer/Buffer.h"
#include "Util/HelperFunctions.h"
#include "Geometry/Geometry.h"
#include "Geometry/InstancedGeometry.h"

#include <boost/foreach.hpp>
#include "URE.h"
#include "MPP/Shader/ShaderManager.h"
#include "Simulator/LightingSimulator/Light/LightSourceManager.h"
#include "Simulator/LightingSimulator/Camera/Camera.h"



namespace Flewnit
{

bool InstanceManager::sInstancedRenderingIsCurrentlyActive = false;

InstanceManager::InstanceManager(String name, GLuint numMaxInstances,
		SubObject* drawableSubObject )
:
	SimulationObject(name, VISUAL_SIM_DOMAIN),
	mMaxManagedInstances(numMaxInstances),
	mCreatedManagedInstances(0),
	mInstanceTransformUniformBuffer(0),
	mInstanceTransformUniformBufferMetaInfo(0),
	mDrawableSubObject(drawableSubObject)
{
	SimulationResourceManager::getInstance().registerInstanceManager(this);

	std::vector<String> memberStrings=
		{
			"modelMatrix",
			"modelViewMatrix",
			"modelViewProjectionMatrix",
			"uniqueInstanceID"
		};
	assert( "draw-geometry may not be instanced!" && (mDrawableSubObject->getGeometry()->getGeometryRepresentation() != INSTANCED_GEOMETRY_REPESENTATION) );
	VisualMaterial* visMat = dynamic_cast<VisualMaterial*>(mDrawableSubObject->getMaterial());
	assert(visMat && visMat->isInstanced());
	Shader* uniformBufferOffsetQueryShader= visMat->getCurrentlyUsedShader();
	assert(uniformBufferOffsetQueryShader && uniformBufferOffsetQueryShader->getLocalShaderFeatures().instancedRendering);

	mInstanceTransformUniformBufferMetaInfo = new UniformBufferMetaInfo(
		mMaxManagedInstances,
		String("InstanceTransformBuffer"),
		String("instanceTransforms"),
		memberStrings,
		uniformBufferOffsetQueryShader
	);
	assert( (mInstanceTransformUniformBufferMetaInfo->mRequiredBufferSize  % BufferHelper::elementSize(TYPE_FLOAT))== 0);
	assert( (mInstanceTransformUniformBufferMetaInfo->mRequiredBufferSize  % BufferHelper::elementSize(TYPE_INT32))== 0);
	//we have 3* mat4 + 1* int and have to expect alignment padding form the GL;
	//allocate generously to be sure not to provoke a buffer overflow;
	//the constructor params of the Buffer class aren't optimal for
	//"non-built-in" element types like the "InstanceTransform" struct,
	//but I won't add new constructors and meta info
	//calculation and maintainment logic just because of this minor design flaw ;(
	int actualNumMatrixElements = std::max(
		(int)(  mInstanceTransformUniformBufferMetaInfo->mRequiredBufferSize
				/ (4 * BufferHelper::elementSize(TYPE_MATRIX44F)) ) ,
		(int)(4*mMaxManagedInstances)
	);
	mInstanceTransformUniformBuffer = new Buffer(
		BufferInfo(
			name + String("InstanceTransformUniformBuffer"),
			ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
			TRANSFORMATION_MATRICES_SEMANTICS,
			TYPE_MATRIX44F,
			actualNumMatrixElements,
			BufferElementInfo(true),
			UNIFORM_BUFFER_TYPE,
			NO_CONTEXT_TYPE
		),
		//yes, the contents are mostly modded as moving instances are involved
		true,
		//set no data yet
		0
	);

}

InstanceManager::~InstanceManager()
{
	delete mDrawableSubObject;
	delete mInstanceTransformUniformBufferMetaInfo;
}

//creates a new SubObject, containing a new InstancedGeometry and a pointer to mAssociatedMaterial
//throws exception if more instances than numMaxInstances would be created;
SubObject* InstanceManager::createInstance()throw(SimulatorException)
{
	if(mCreatedManagedInstances >= mMaxManagedInstances)
	{
		throw(SimulatorException("max number of instances for this instance manager already reached"));
	}

	InstancedGeometry* instGeo = new InstancedGeometry(
			getName() + String("GeometryInstance") + HelperFunctions::toString(mCreatedManagedInstances),
			this,
			mCreatedManagedInstances
	);

	SubObject* returnSO= new SubObject(
		getName() + String("SubObjectInstance") + HelperFunctions::toString(mCreatedManagedInstances),
		VISUAL_SIM_DOMAIN, //maybe this class will be later relevant also for mechanical sim domain, but i can't foresee this, as my knowledge of physics simulation is not that deep :(
		instGeo,
		mDrawableSubObject->getMaterial()
	);
	instGeo->setOwningSubObject(returnSO);

	mCreatedManagedInstances++;

	return returnSO;
}

//called by InstancedGeometry::draw(); the owning WorldObject is backtracked, its relevant matrices extracted
//and the uniform buffer entry filled with the relevant information;
void InstanceManager::registerInstanceForNextDrawing(SubObject* so)throw(SimulatorException)
{
	assert(mCurrentlyRegisteredInstancesForNextDrawing.size() < mMaxManagedInstances);
	assert("geometry is instanced" && dynamic_cast<InstancedGeometry*>(so->getGeometry()));
	assert("instance belongs to this manager" && (reinterpret_cast<InstancedGeometry*>(so->getGeometry())->getInstanceManager() == this));
	BOOST_FOREACH(SubObject* currentSO, mCurrentlyRegisteredInstancesForNextDrawing)
	{
		if(
			reinterpret_cast<InstancedGeometry*>(currentSO->getGeometry())->getUniqueID() ==
			reinterpret_cast<InstancedGeometry*>(so->getGeometry())->getUniqueID()
		)
		{
			throw(SimulatorException("instance with this ID is already registered"));
		}
	}
	mCurrentlyRegisteredInstancesForNextDrawing.push_back(so);
}


void InstanceManager::updateTransformBuffer()
{
	//disinguish transform matrix calc:
	// - SM gen: view/proj from spotlight
	// - default lighting: view/proj from main cam

	assert(mInstanceTransformUniformBuffer->hasBufferInContext(HOST_CONTEXT_TYPE));
	unsigned char* bufferToFill = reinterpret_cast<unsigned char*>(mInstanceTransformUniformBuffer->getCPUBufferHandle());

	//disinguish transform matrix calc:
	// - SM gen && shadow technique != one spot light: view/proj unused, keep identity matrix
	// - SM gen && shadow technique == one spot light: view/proj from spotlight
	// - else: view/proj from main cam
	//get info from shader manager:
	//{begin boilerplate ()
	Matrix4x4 viewMatrix(1.0f);
	Matrix4x4 viewProjMatrix(1.0f);
	if(dynamic_cast<VisualMaterial*>(mDrawableSubObject->getMaterial())->getCurrentlyUsedShader()->getLocalShaderFeatures().renderingTechnique
			== RENDERING_TECHNIQUE_SHADOWMAP_GENERATION)
	{
		if(ShaderManager::getInstance().getGlobalShaderFeatures().lightSourcesShadowFeature ==
				LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT)
		{
			SpotLight* spot = dynamic_cast<SpotLight*> (LightSourceManager::getInstance().getFirstShadowCaster());
			assert("in this scenario, a shadow caster must be a spotlight" && spot);
			viewMatrix = spot->getViewMatrix();
			viewProjMatrix = spot->getViewProjectionMatrix();
		}
	}
	else
	{
		Camera* cam = URE_INSTANCE->getCurrentlyActiveCamera();
		viewMatrix = cam->getViewMatrix();
		viewProjMatrix = cam->getProjectionMatrix() * viewMatrix ;
	}

	for(unsigned int currentTransformElementIndex =0; currentTransformElementIndex < mCurrentlyRegisteredInstancesForNextDrawing.size(); currentTransformElementIndex++ )
	{
#define CURRENT_MAT4_VALUE(index) \
	reinterpret_cast<Matrix4x4&>( \
		bufferToFill[ \
		  mInstanceTransformUniformBufferMetaInfo->mBufferOffsets[ currentTransformElementIndex ][index] \
	    ] \
	)
#define CURRENT_INT_VALUE \
	reinterpret_cast<int&>( \
		bufferToFill[ \
		  mInstanceTransformUniformBufferMetaInfo->mBufferOffsets[currentTransformElementIndex][3] \
		] \
	)

	const Matrix4x4& modelMatrix=
			mCurrentlyRegisteredInstancesForNextDrawing[currentTransformElementIndex]->
			getOwningWorldObject()->getGlobalTransform().getTotalTransform();

	//modelMatrix
	CURRENT_MAT4_VALUE(0)= modelMatrix;
	//modelViewMatrix
	CURRENT_MAT4_VALUE(1)= viewMatrix * modelMatrix;
	//modelViewProjectionMatrix
	CURRENT_MAT4_VALUE(2)= viewProjMatrix * modelMatrix;
	//uniqueInstanceID
	CURRENT_INT_VALUE =  reinterpret_cast<InstancedGeometry*>(
			mCurrentlyRegisteredInstancesForNextDrawing[currentTransformElementIndex]->getGeometry())->getUniqueID();

#undef CURRENT_MAT4_VALUE
#undef CURRENT_INT_VALUE
	} //endfor

	mInstanceTransformUniformBuffer->copyFromHostToGPU();
}

void InstanceManager::drawRegisteredInstances(SimulationPipelineStage* currentStage)
{
	if(mCurrentlyRegisteredInstancesForNextDrawing.size()>0)
	{
		sInstancedRenderingIsCurrentlyActive = true;
		mDrawableSubObject->getMaterial()->activate(currentStage, mCurrentlyRegisteredInstancesForNextDrawing[0]);

		mDrawableSubObject->getGeometry()->draw(mCurrentlyRegisteredInstancesForNextDrawing.size());

		mDrawableSubObject->getMaterial()->deactivate(currentStage, mDrawableSubObject);

		//reset instance registration:
		mCurrentlyRegisteredInstancesForNextDrawing.clear();
		sInstancedRenderingIsCurrentlyActive = false;
	}
}


}
