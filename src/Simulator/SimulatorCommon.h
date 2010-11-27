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

#include "Util/Timer.h"


namespace Flewnit
{

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

class Loader;

class MediaLayerInterface;

class GUI;

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
class SPHFluidMechanicsSimulator;
class SoundSimulator;



class SimulationPipeline;
class SimulationData;
class PipelineStageInterface;
class LightingPipelineStage;
class SPHFluidPipelineStage;



///\brief buffer forwards;
///\{
class BufferInterface;
class PingPongBuffer;

class VBO;
class VertexAttributeBuffer;
class VertexIndexBuffer;

class OpenCLBuffer;

class RenderTarget;
class FBO;

class Texture;
class Texture3D;
///\}


///\brief Scene forwards;
///\{
class Scene;
class SceneNode;
class Camera;
class WorldObject;

class SubObject;
class GeometryInterface;
class Material;

class MechanicSubObject;
class VisualSubObject;
class AcusticSubObject;
///\}





}
