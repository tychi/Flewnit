/*
 * SimulatorCommon.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 *  \brief Some Forward declarations, enums and defines used by many Simulation related files.
 *
 *  \note Some forwards may be overkill here, but regard this as a "class brainstorming at the moment ;).
 */

#pragma once

#include "Util/Time/Timer.h"


namespace Flewnit
{

class FPSCounter;
class InputInterpreter;

enum SimulationDomain
{
	MECHANICAL_SIM_DOMAIN 	= 0,
	VISUAL_SIM_DOMAIN 		= 1,
	ACUSTIC_SIM_DOMAIN		= 2,
	__NUM_SIM_DOMAINS__		= 3,
	__NO_SIM_DOMAIN__		= 4
};

class SimulationDomainIdentifier
{
public:
	virtual SimulationDomain getSimDomain()const = 0;

//ad this macro to every class definition deriving from this class;
#define DECLARE_SIM_DOMAIN(simDomain) virtual SimulationDomain getSimDomain()const {return simDomain;}

};

//settings to configure a simulation step:
//debug draw, time interval, etc;
class SimStepSettings;

enum SimulationKindFlags
{
	LIGHTING_SIMULATION 	= 1<<0,
	FLUID_SIMULATION		= 1<<1,
	RIGID_BODY_SIMULATION	= 1<<2,
	SOFT_BODY_SIMULATION	= 1<<3,
	CLOTH_SIMULATION		= 1<<4,
	HAIR_SIMULATION			= 1<<5
};

class Profiler;
class Log;


class Config;
class Loader;

class WindowManager;
class GUI;
class InputManager;

///\brief Converter forwards;
///\{
class GeometryConverter;
class MeshVoxelizer;
class VoxelParticelizer;
class DistanceFiledRasterizer;
class TriangleIndexBufferGenerator;
///\}



class SimulatorInterface;

class LightingSimulator;
//class FluidMechanicsSimulator;
class MechanicsSimulator;
//class SPHFluidMechanicsSimulator;
class SoundSimulator;

class SimulationResourceManager;
class OpenCL_Manager;


class SimulationPipeline;
class SimulationPipelineStage;
//class LightingPipelineStage;
//class SPHFluidPipelineStage;



///\brief buffer forwards;
///\{
class BufferInterface;
class PingPongBuffer;

class VBO;
class VertexAttributeBuffer;
class VertexIndexBuffer;

//class OpenCLBuffer;

class RenderTarget;
class Screen;
class FBO;

class Texture;
class Texture3D;
///\}


///\brief Scene forwards;
///\{
class SceneGraph;
class SceneNode;
class WorldObject;
class Camera;

//abstract generic intercae classes for all simulation domains to derive from them spcifically
class SubObject;
class Geometry;
class Material;

//the "Massively Parallel Program"; Abstraction for CUDA, OpenCL, OpenGL, DirectCompute etc pp;
class MPP;
class ShaderProgram;
class OpenCLProgram;


class MechanicSubObject;
class VisualSubObject;
class AcusticSubObject;
///\}





}
