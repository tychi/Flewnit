/*
 * Box.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#include "Box.h"
#include "Simulator/SimulationResourceManager.h"
#include "Geometry/Geometry.h"
#include "Geometry/VertexBasedGeometry.h"
#include "Material/VisualMaterial.h"
#include "SubObject.h"
#include "Buffer/Buffer.h"
#include "Util/Log/Log.h"

#include "URE.h"
#include "Util/Loader/Loader.h"


namespace Flewnit {

Box::Box(String name, const AmendedTransform& localTransform, const Vector3D& halfextends)
: PureVisualObject(name, localTransform)
{
	Geometry* geo = SimulationResourceManager::getInstance().getGeometry("BoxGeometry");
	if(! geo)
	{
		VertexBasedGeometry* vertGeo = new VertexBasedGeometry("BoxGeometry",VERTEX_BASED_TRIANGLES);
		geo= vertGeo;

		BufferInfo bufferi(
				"BoxGeometryPositionBuffer",
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				POSITION_SEMANTICS,
				TYPE_VEC4F,
				24, //because of the normals, the vertices cannot be shared ;(
				VERTEX_ATTRIBUTE_BUFFER_TYPE,
				NO_CONTEXT_TYPE
		);

		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		bufferi.name = "BoxGeometryNormalBuffer";
		bufferi.bufferSemantics = NORMAL_SEMANTICS;
		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		bufferi.name = "BoxGeometryTexCoordBuffer";
		bufferi.bufferSemantics = TEXCOORD_SEMANTICS;
		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		vertGeo->setIndexBuffer(
			new Buffer(
				BufferInfo(
					"BoxGeometryIndexBuffer",
					ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
					INDEX_SEMANTICS,
					TYPE_UINT32,
					36,
					VERTEX_INDEX_BUFFER_TYPE,
					NO_CONTEXT_TYPE
				)
			)
		);


		Vector4D* posBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(POSITION_SEMANTICS)->getCPUBufferHandle());
		Vector4D* normalBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(NORMAL_SEMANTICS)->getCPUBufferHandle());
		Vector4D* tcBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(TEXCOORD_SEMANTICS)->getCPUBufferHandle());

		int* indexBuffer = reinterpret_cast<int*>(vertGeo->getIndexBuffer()->getCPUBufferHandle());




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

				int vertexIndexBase = 8* axis + 4*(1-(side+1)/2);
				int indexIndexBase = 12* axis + 6*(1-(side+1)/2);

				//lower left
				posBuffer[vertexIndexBase + 0] = Vector4D( (normal + left + down)* halfextends, 1 );
				normalBuffer[vertexIndexBase + 0]=Vector4D( normal, 0 );
				tcBuffer[vertexIndexBase + 0]=Vector4D(0,0,0, 0 );
				//lower right
				posBuffer[vertexIndexBase + 1] =Vector4D( (normal - left + down)* halfextends , 1 );
				normalBuffer[vertexIndexBase + 1]=Vector4D( normal, 0 );
				tcBuffer[vertexIndexBase + 1]=Vector4D(1,0,0, 0 );
				//upper right
				posBuffer[vertexIndexBase + 2] =Vector4D( (normal - left - down)* halfextends , 1 );
				normalBuffer[vertexIndexBase + 2]=Vector4D( normal, 0 );
				tcBuffer[vertexIndexBase + 2]=Vector4D(1,1,0, 0 );
				//upper left
				posBuffer[vertexIndexBase + 3] =Vector4D( (normal + left - down)* halfextends , 1 );
				normalBuffer[vertexIndexBase + 3]=Vector4D( normal, 0 );
				tcBuffer[vertexIndexBase + 3]=Vector4D(0,1,0, 0 );

				indexBuffer[indexIndexBase + 0]=  vertexIndexBase + 0;
				indexBuffer[indexIndexBase + 1]=  vertexIndexBase + 1;
				indexBuffer[indexIndexBase + 2]=  vertexIndexBase + 2;
				indexBuffer[indexIndexBase + 3]=  vertexIndexBase + 0;
				indexBuffer[indexIndexBase + 4]=  vertexIndexBase + 2;
				indexBuffer[indexIndexBase + 5]=  vertexIndexBase + 3;

				LOG<<DEBUG_LOG_LEVEL<<"VERTICES OF BOX, face "<<
						((side>0)?"positive":"negative") <<
						(	(axis==0)
								?" X"
								:( (axis==1)
									?" Y"
									:" Z"	)    )

						<< ": \n";
				for(int i=0;i<4;i++)
				{
					LOG<<DEBUG_LOG_LEVEL
							<<"Index of vertex :"<<vertexIndexBase+i
							<<"; Vertex coords: "<<posBuffer[vertexIndexBase + i]
							<<"; Normal: "<<normalBuffer[vertexIndexBase + i]
							<<"; start index for face in index buffer: "<<indexIndexBase<<";\n"
			 	    ;
				}

			}
		}

		vertGeo->getAttributeBuffer(POSITION_SEMANTICS)->copyFromHostToGPU();
		vertGeo->getAttributeBuffer(NORMAL_SEMANTICS)->copyFromHostToGPU();
		vertGeo->getAttributeBuffer(TEXCOORD_SEMANTICS)->copyFromHostToGPU();
		vertGeo->getIndexBuffer()->copyFromHostToGPU();

		//TODO ADD BUFFERS;
	}

	Material* mat = SimulationResourceManager::getInstance().getMaterial("MinimalLightingMaterial");
	if(! mat)
	{
//		Texture* tex= URE_INSTANCE->getLoader()->loadTexture(
//				String("bunnyTex"),
//				DECAL_COLOR_SEMANTICS,
//				Path("./assets/textures/bunny.png"),
//				TexelInfo(4,GPU_DATA_TYPE_UINT,8,true),
//				true,
//				false,
//				false
//		);
//
//		std::map<BufferSemantics,Texture*> myMap;
//		myMap[DECAL_COLOR_SEMANTICS] = tex;

		mat = new VisualMaterial("MinimalLightingMaterial",
				//VISUAL_MATERIAL_TYPE_DEBUG_DRAW_ONLY, SHADING_FEATURE_NONE,
				VISUAL_MATERIAL_TYPE_DEFAULT_LIGHTING,
				SHADING_FEATURE_DIRECT_LIGHTING,
				//ShadingFeatures( SHADING_FEATURE_DIRECT_LIGHTING | SHADING_FEATURE_DECAL_TEXTURING),
				//myMap,
				std::map<BufferSemantics,Texture*>(),
				VisualMaterialFlags(true,false,true,true,false,false));

	}

	addSubObject(
		new SubObject(
				"BoxSubObject",
				VISUAL_SIM_DOMAIN,
				geo,
				mat
		)
	);

}

Box::~Box()
{
	// TODO Auto-generated destructor stub
}

}



/*
 		BufferInfo bufferi(
				"BoxGeometryPositionBuffer",
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
				POSITION_SEMANTICS,
				TYPE_VEC4F,
				24, //because of the normals, the vertices cannot be shared ;(
				VERTEX_ATTRIBUTE_BUFFER_TYPE,
				NO_CONTEXT_TYPE
		);

		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		bufferi.name = "BoxGeometryNormalBuffer";
		bufferi.bufferSemantics = NORMAL_SEMANTICS;
		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		bufferi.name = "BoxGeometryTexCoordBuffer";
		bufferi.bufferSemantics = TEXCOORD_SEMANTICS;
		vertGeo ->setAttributeBuffer(
			new Buffer(
				bufferi,
				false,
				0
			)
		);

		vertGeo->setIndexBuffer(
			new Buffer(
				BufferInfo(
					"BoxGeometryIndexBuffer",
					ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG),
					INDEX_SEMANTICS,
					TYPE_UINT32,
					36,
					VERTEX_INDEX_BUFFER_TYPE,
					NO_CONTEXT_TYPE
				)
			)
		);


		Vector4D* posBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(POSITION_SEMANTICS)->getCPUBufferHandle());
		Vector4D* normalBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(NORMAL_SEMANTICS)->getCPUBufferHandle());
		Vector4D* tcBuffer = reinterpret_cast<Vector4D*>
			(vertGeo->getAttributeBuffer(TEXCOORD_SEMANTICS)->getCPUBufferHandle());

		int* indexBuffer = reinterpret_cast<int*>(vertGeo->getIndexBuffer()->getCPUBufferHandle());




//		for(int axis=0; axis<3; axis++)
//		{
//			for(int side = 1; side >= -1; side -= 2)
//			{
//				Vector3D normal = Vector3D(axis==0?side:0,axis==1?side:0, axis==2?side:0);
//				Vector3D left = Vector3D(
//						((axis==2) || (axis ==1))	 ? 	-side 	:	0,
//						0,
//						axis==0?side:0
//				);
//				Vector3D down= glm::cross(normal,left);
//
//				int vertexIndexBase = 8* axis + 4*(1-(side+1)/2);
//				int indexIndexBase = 12* axis + 6*(1-(side+1)/2);
//
//				//lower left
//				posBuffer[vertexIndexBase + 0] = Vector4D( (normal + left + down)* halfextends, 1 );
//				normalBuffer[vertexIndexBase + 0]=Vector4D( normal, 0 );
//				tcBuffer[vertexIndexBase + 0]=Vector4D(0,0,0, 0 );
//				//lower right
//				posBuffer[vertexIndexBase + 1] =Vector4D( (normal - left + down)* halfextends , 1 );
//				normalBuffer[vertexIndexBase + 1]=Vector4D( normal, 0 );
//				tcBuffer[vertexIndexBase + 1]=Vector4D(1,0,0, 0 );
//				//upper right
//				posBuffer[vertexIndexBase + 2] =Vector4D( (normal - left - down)* halfextends , 1 );
//				normalBuffer[vertexIndexBase + 2]=Vector4D( normal, 0 );
//				tcBuffer[vertexIndexBase + 2]=Vector4D(1,1,0, 0 );
//				//upper left
//				posBuffer[vertexIndexBase + 3] =Vector4D( (normal + left - down)* halfextends , 1 );
//				normalBuffer[vertexIndexBase + 3]=Vector4D( normal, 0 );
//				tcBuffer[vertexIndexBase + 3]=Vector4D(0,1,0, 0 );
//
//				indexBuffer[indexIndexBase + 0]=  vertexIndexBase + 0;
//				indexBuffer[indexIndexBase + 1]=  vertexIndexBase + 1;
//				indexBuffer[indexIndexBase + 2]=  vertexIndexBase + 2;
//				indexBuffer[indexIndexBase + 3]=  vertexIndexBase + 0;
//				indexBuffer[indexIndexBase + 4]=  vertexIndexBase + 2;
//				indexBuffer[indexIndexBase + 5]=  vertexIndexBase + 3;
//
//				LOG<<DEBUG_LOG_LEVEL<<"VERTICES OF BOX, face "<<
//						((side>0)?"positive":"negative") <<
//						(	(axis==0)
//								?" X"
//								:( (axis==1)
//									?" Y"
//									:" Z"	)    )
//
//						<< ": \n";
//				for(int i=0;i<4;i++)
//				{
//					LOG<<DEBUG_LOG_LEVEL
//							<<"Index of vertex :"<<vertexIndexBase+i
//							<<"; Vertex coords: "<<posBuffer[vertexIndexBase + i]
//							<<"; Normal: "<<normalBuffer[vertexIndexBase + i]
//							<<"; start index for face in index buffer: "<<indexIndexBase<<";\n"
//			 	    ;
//				}
//
//			}
//		}

 * */
