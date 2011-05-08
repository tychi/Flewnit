/*
 * BoxGeometry.cpp
 *
 *  Created on: Feb 19, 2011
 *      Author: tychi
 */

#include "BoxGeometry.h"
#include "Buffer/Buffer.h"
#include "Util/Log/Log.h"
#include "MPP/Shader/ShaderManager.h"

namespace Flewnit {

BoxGeometry::BoxGeometry(
		String name,
		const Vector3D& halfExtends,
		bool addTangents,
		bool patchRepresentation,
		const Vector4D& texcoordScale)
: VertexBasedGeometry(name,
		(
			patchRepresentation
			&& ShaderManager::getInstance().shadingFeaturesAreEnabled(SHADING_FEATURE_TESSELATION)
		)? VERTEX_BASED_TRIANGLE_PATCHES : VERTEX_BASED_TRIANGLES),
		mHalfExtends(halfExtends)
  {
	BufferInfo bufferi(
			name + String("BoxGeometryPositionBuffer"),
			ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
			POSITION_SEMANTICS,
			TYPE_VEC4F,
			6* 4 , //because of the normals, the vertices cannot be shared ;(
			BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
			VERTEX_ATTRIBUTE_BUFFER_TYPE,
			NO_CONTEXT_TYPE
	);

	setAttributeBuffer(new Buffer(bufferi,false,0));

	bufferi.name = name + String("BoxGeometryNormalBuffer");
	bufferi.bufferSemantics = NORMAL_SEMANTICS;
	setAttributeBuffer(new Buffer(bufferi,false,0));

	if(addTangents)
	{
		bufferi.name = name + String("BoxGeometryTangentBuffer");
		bufferi.bufferSemantics = TANGENT_SEMANTICS;
		setAttributeBuffer( new Buffer( bufferi, false, 0 )	);
	}

	bufferi.name = name + String( "BoxGeometryTexCoordBuffer");
	bufferi.bufferSemantics = TEXCOORD_SEMANTICS;
	setAttributeBuffer(new Buffer(bufferi,false,0));

	setIndexBuffer(
		new Buffer(
			BufferInfo(
				name + String("BoxGeometryIndexBuffer"),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				INDEX_SEMANTICS,
				TYPE_UINT32,
				6 * 6, //6 faces * 2* triangles @ 3 verts
				BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
				VERTEX_INDEX_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			)
		)
	);


	Vector4D* posBuffer = 	reinterpret_cast<Vector4D*>(getAttributeBuffer(POSITION_SEMANTICS)->getCPUBufferHandle());
	Vector4D* normalBuffer =reinterpret_cast<Vector4D*>(getAttributeBuffer(NORMAL_SEMANTICS)->getCPUBufferHandle());
	Vector4D* tangentBuffer = 0;
	if(addTangents)	{
		tangentBuffer = 	reinterpret_cast<Vector4D*>(getAttributeBuffer(TANGENT_SEMANTICS)->getCPUBufferHandle());
	}
	Vector4D* tcBuffer = 	reinterpret_cast<Vector4D*>(getAttributeBuffer(TEXCOORD_SEMANTICS)->getCPUBufferHandle());
	unsigned int* indexBuffer = reinterpret_cast<unsigned int*>(getIndexBuffer()->getCPUBufferHandle());



	//iterate over the six faces:
	for(int axis=0; axis<3; axis++)
	{
		for(int side = 1; side >= -1; side -= 2)
		{
			Vector3D normal = Vector3D(axis==0?side:0,axis==1?side:0, axis==2?side:0);
			Vector3D left = Vector3D(
					((axis==2) || (axis ==1))	 ? 	-side 	:	0,
					0,
					axis==0?side:0
			);

			Vector3D down= glm::cross(normal,left);

			Vector3D tangent = left * (-1.0f);

			int vertexIndexBase = 8* axis + 4*(1-(side+1)/2);
			int indexIndexBase = 12* axis + 6*(1-(side+1)/2);

			//lower left
			posBuffer[vertexIndexBase + 0] 		= Vector4D( (normal + left + down)* halfExtends, 1 );
			normalBuffer[vertexIndexBase + 0]	= Vector4D( normal, 0 );
			if(addTangents){
				tangentBuffer[vertexIndexBase+0]= Vector4D( tangent, 0 );
			}
			tcBuffer[vertexIndexBase + 0]		= Vector4D(0,0,0, 0 ) * texcoordScale;
			//lower right
			posBuffer[vertexIndexBase + 1] 		= Vector4D( (normal - left + down)* halfExtends , 1 );
			normalBuffer[vertexIndexBase + 1]	= Vector4D( normal, 0 );
			if(addTangents){
				tangentBuffer[vertexIndexBase+1]= Vector4D( tangent, 0 );
			}
			tcBuffer[vertexIndexBase + 1]		= Vector4D(1,0,0, 0 )* texcoordScale;
			//upper right
			posBuffer[vertexIndexBase + 2] 		= Vector4D( (normal - left - down)* halfExtends , 1 );
			normalBuffer[vertexIndexBase + 2]	= Vector4D( normal, 0 );
			if(addTangents){
				tangentBuffer[vertexIndexBase+2]= Vector4D( tangent, 0 );
			}
			tcBuffer[vertexIndexBase + 2]		= Vector4D(1,1,0, 0 )* texcoordScale;
			//upper left
			posBuffer[vertexIndexBase + 3] 		= Vector4D( (normal + left - down)* halfExtends , 1 );
			normalBuffer[vertexIndexBase + 3]	= Vector4D( normal, 0 );
			if(addTangents){
				tangentBuffer[vertexIndexBase+3]= Vector4D( tangent, 0 );
			}
			tcBuffer[vertexIndexBase + 3]		= Vector4D(0,1,0, 0 )* texcoordScale;

			indexBuffer[indexIndexBase + 0]=  vertexIndexBase + 0;
			indexBuffer[indexIndexBase + 1]=  vertexIndexBase + 1;
			indexBuffer[indexIndexBase + 2]=  vertexIndexBase + 2;
			indexBuffer[indexIndexBase + 3]=  vertexIndexBase + 0;
			indexBuffer[indexIndexBase + 4]=  vertexIndexBase + 2;
			indexBuffer[indexIndexBase + 5]=  vertexIndexBase + 3;

//			LOG<<DEBUG_LOG_LEVEL<<"VERTICES OF BOX, face "<<
//					((side>0)?"positive":"negative") <<
//					(	(axis==0)
//							?" X"
//							:( (axis==1)
//								?" Y"
//								:" Z"	)    )
//
//					<< ": \n";
//			for(int i=0;i<4;i++)
//			{
//				LOG<<DEBUG_LOG_LEVEL
//						<<"Index of vertex :"<<vertexIndexBase+i
//						<<"; Vertex coords: "<<posBuffer[vertexIndexBase + i]
//						<<"; Normal: "<<normalBuffer[vertexIndexBase + i]
//						<<"; start index for face in index buffer: "<<indexIndexBase<<";\n"
//		 	    ;
//			}

		}
	}

	getAttributeBuffer(POSITION_SEMANTICS)->copyFromHostToGPU();
	getAttributeBuffer(NORMAL_SEMANTICS)->copyFromHostToGPU();
	if(addTangents){getAttributeBuffer(TANGENT_SEMANTICS)->copyFromHostToGPU();}
	getAttributeBuffer(TEXCOORD_SEMANTICS)->copyFromHostToGPU();
	getIndexBuffer()->copyFromHostToGPU();

}

BoxGeometry::~BoxGeometry()
{

}

}
