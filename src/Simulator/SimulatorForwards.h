/*
 * SimulatorCommon.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 *  \brief Some Forward declarations, enums and defines used by many Simulation related files.
 *
 *  \note Some forwards may be overkill here, but regard this as a "class brainstorming" at the moment ;).
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"



namespace Flewnit
{


class Timer;
class FPSCounter;
class InputInterpreter;

class AmendedTransform;



class Profiler;
class Log;


class Config;
class ConfigStructNode;
template <typename T> class ConfigValueNode;

class Loader;

class WindowManager;
class GUI;
class InputManager;


//{
class RadixSorter;
//}

///\brief Converter forwards;
///\{
class GeometryTransformer;

class MeshVoxelizer;
class VoxelParticelizer;
class DistanceFieldRasterizer;
class TriangleIndexBufferGenerator;
///\}



class SimulatorInterface;

class LightingSimulator;
class MechanicsSimulator;
class SoundSimulator;

class SimulationResourceManager;
class IntermediateResultBuffersManager;


class ParallelComputeManager;


//class SimulationPipeline;
class SimulationPipelineStage;

	class LightingSimStageBase;
		class ShadowMapGenerator;
		class DefaultLightingStage;
		//to be continued..

	class ParticleMechanicsStage;
	//to be continued...

//class LightingPipelineStage;
//class SPHFluidPipelineStage;
//required features a pipeline stage needs fulfilled from the rest of the simulators/pipelines to work properly
//class SimulationPipelineRequirements;
//features the pipeline stage provides itself;
//class SimulationPipelineFeatures;



///\brief buffer forwards;
///\{


class BufferInfo;
class TextureInfo;

//abstract base class for all Buffers;
class BufferInterface;
//kind of wrapper for two buffers to be toggled for read/write access;
class PingPongBuffer;
//class for all buffers but textures and render buffers;
class Buffer;

class UniformBufferMetaInfo;

//abstract base class for the several concrete-dimensioned textures;
class Texture;

class Texture1D;
class Texture1DArray;

class Texture2D;
class Texture2DDepth;

class Texture2DCube;


class Texture2DArray;

class Texture2DMultiSample;
class Texture2DMultiSampleArray;


class Texture3D;
//the special class for offscreen rendering: kinda texture without filtering stuff for depth and stencil operations;
class RenderBuffer;



//class VBO;
//class VertexAttributeBuffer;
//class VertexIndexBuffer;

//class OpenCLBuffer;

class RenderTarget;
//class Screen;
//class FBO;
///\}


///\brief Scene/WorldObject forwards;
///\{
class SceneRepresentation;
	class SceneGraph;
		class SceneNode;
	class ParticleSceneRepresentation;
	class 	ParticleAttributeBuffers;
	class VoxelSceneRepresentation;
	class PrimitiveSceneRepresentation;

//WorldObject is derived from SceneNode
class WorldObject;
	class PureVisualObject; //pure visual object without mechanical interference
	class 	SkyDome;


///\{ following: concrete WorldObjects with different purpose:
class Camera;

//
class LightSourceManager;
	class LightSource;
		class 	PointLight;
		//SpotLight is a Specialization of PointLight;
		class	SpotLight;


class ParticleFluid; //the most important WorldObject in this thesis :);
class VoxelFluid; //irrelevant in this thesis

class RigidBody; //configurable or derivable to TriangleRB or ParticleizedRB ..t.b.d...
class 	ParticleRigidBody;

class 	PrimitiveRigidBody; //Convex hull, capsule etc.. won't be thought about during this thesis,
							//even though this may lead to more design flaws...
class 	TriangleRigidBody;	 //see above, no further thought about this

class StaticObject;	//triangles specially sorted and accessed in the mechanical domain for efficient
					//collision detection via OpenCL

class AccelerationStructure;
class  	UniformGrid; //in "virtual reality" logic, not a WorldObject;
					//but because of its buffers and debug draw stuff,
					//it fits the structure of a WorldObject;
	class UniformGridBufferSet;
class 	KDTree;  //won't be thought about during this thesis...
class 	BVH;    //won't be thought about during this thesis...
///\}


class InstanceManager; //maintainer for a subset for instanced geometry

//abstract generic interface classes for all simulation domains to derive from them spcifically
class SubObject;

class Geometry;
class 	InstancedGeometry; //derived from Geometry, pointing to owning InstnaceManager

class 	VertexBasedGeometry;	//
//class 	PointCloudGeometry;
//class		TriangleMeshGeometry;

//abstract base class for all voxel stuff:
class 	VoxelGridGeometry;
//As In OpenCL (in contrast to CUDA), it is not possible to "reinterpret cast" a Generic Buffer
//to a Texture or Vice versa, we have to implement two seperate classes :(.
//The 3D Texture Voxel representation is great for gas Simulation AND its rendering;
class		Texture3DVoxelGridGeometry;

//procedural types:
class BoxGeometry;
class UnitQuad;

/*
The Generic Buffer is needed as a storage for a Uniform Grid acceleration structure
storing
	0.: The Amount of the containing particles
	1.: index to the first of the containing Particles
	2.: The Amount of the containing static triangles
	3.: index to the first of the containing static Triangles

It COULD be realized a a 3D Integer Texture, bu I don't like this concept (yet)
*/
class		GenericBufferVoxelGridGeometry;
//the single purpose for this Geometry type is to realize debug drawing of the Uniform Grid
//via instanced drawing of only two vertices;
class		UniformGridImplicitVoxelGridGeometry;

class Material;
class   VisualMaterialFlags;
class	VisualMaterial;
class   	SkyDomeMaterial;
class 		DebugDrawVisualMaterial;
class   	ParticleLiquidVisualMaterial;
class MechanicalMaterial;
class 	ParticleRigidBodyMechMat;
class	ParticleFluidMechMat;


//to be continued;

//following obsolete
//class MechanicSubObject;
//class VisualSubObject;
//class AcusticSubObject;

///\} //end Scene Forwards

//the "Massively Parallel Program"; Abstraction for CUDA, OpenCL, OpenGL, DirectCompute etc pp;
class MPP;

	class ShaderManager;
	class 	Shader;
	class 	TextureShowShader;
	class 	ParticleLiquidShader;

	class CLProgramManager;
	class 	CLProgram;
	class 	 	BasicCLProgram;
	class	 		RadixSortProgram;
	class 	  		ReorderParticleAttributesProgram;

	class 			UniformGridRelatedProgram;
	class 				UpdateUniformGridProgram;
	class				SplitAndCompactUniformGridProgram;
	class 	  			ParticleSimulationProgram;
	class 					Initial_UpdateForce_Integrate_CalcZIndex_Program;
	class					UpdateDensityProgram;
	class					UpdateForce_Integrate_CalcZIndex_Program;
	class 					UpdateRigidBodiesProgram;

	class CLKernel;
	class CLKernelWorkLoadParams;
	class CLKernelArguments;
	class CLKernelArgumentBase;
	template<typename T> class CLValueKernelArgument;
	class 	CLBufferKernelArgument;

	//following data types actually defined in OpenCL source code, but also included by the c++ app;
	//Code compatibility ensured by conditional compilation and careful alignment by hand via
	//padding members;
	//edit: doesn't work; typedef struct {...} StructureName; seems for the compiler not to be the same as
	// struct StructureName {...};
	//	namespace CLShare
	//	{
	//		struct ObjectGenericFeatures;
	//		struct UserForceControlPoint;
	//		struct ParticleRigidBody;
	//		struct SimulationParameters;
	//	}



}
