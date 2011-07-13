/*
 * UnitQuad.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: tychi
 */

#include "UnitQuad.h"
#include "Buffer/Buffer.h"
#include "Util/Log/Log.h"
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit
{

UnitQuad::UnitQuad(	String name)
: VertexBasedGeometry(name,	VERTEX_BASED_TRIANGLES)
  {
	BufferInfo bufferi(
			name + String("UnitQuadPositionBuffer"),
			ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
			POSITION_SEMANTICS,
			TYPE_VEC4F,
			6, //2*3 triangle vertices without index
			BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
			VERTEX_ATTRIBUTE_BUFFER_TYPE,
			NO_CONTEXT_TYPE
	);

	setAttributeBuffer(new Buffer(bufferi,false,0));

	bufferi.name = name + String( "BoxGeometryTexCoordBuffer");
	bufferi.bufferSemantics = TEXCOORD_SEMANTICS;
	setAttributeBuffer(new Buffer(bufferi,false,0));


	Vector4D* posBuffer = 	reinterpret_cast<Vector4D*>(getAttributeBuffer(POSITION_SEMANTICS)->getCPUBufferHandle());
	Vector4D* tcBuffer = 	reinterpret_cast<Vector4D*>(getAttributeBuffer(TEXCOORD_SEMANTICS)->getCPUBufferHandle());

	//lower right triangle
	posBuffer[0]= Vector4D(-1.0f,-1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(0.0f,0.0f,0.0f,1.0f);

	posBuffer[1]= Vector4D(1.0f,-1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(1.0f,0.0f,0.0f,1.0f);

	posBuffer[2]= Vector4D(1.0f,1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(1.0f,1.0f,0.0f,1.0f);

	//upper left triangle
	posBuffer[3]= Vector4D(-1.0f,-1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(0.0f,0.0f,0.0f,1.0f);

	posBuffer[4]= Vector4D(1.0f,1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(1.0f,1.0f,0.0f,1.0f);

	posBuffer[5]= Vector4D(-1.0f,1.0f,0.0f,1.0f);
	tcBuffer[0]= Vector4D(0.0f,1.0f,0.0f,1.0f);


	getAttributeBuffer(POSITION_SEMANTICS)->copyFromHostToGPU();
	getAttributeBuffer(TEXCOORD_SEMANTICS)->copyFromHostToGPU();


}

UnitQuad::~UnitQuad()
{

}

}
