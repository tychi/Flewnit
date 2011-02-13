/*
 * Loader.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Loader.h"

#include "LoaderHelper.h"

#include <tinyxml.h>

#include "Buffer/Texture.h"
#include <FreeImagePlus.h>


#include <boost/lexical_cast.hpp>
#include "Buffer/BufferHelperUtils.h"
#include "WorldObject/Box.h"
#include "Simulator/SimulationResourceManager.h"
#include "Scene/Scene.h"



namespace Flewnit
{

Loader::Loader()
{
	// TODO Auto-generated constructor stub

}

Loader::~Loader()
{
	// TODO Auto-generated destructor stub
}

void Loader::loadScene()
{
	createHardCodedSceneStuff();

}


//DEBUG stuff:
void Loader::createHardCodedSceneStuff()
{
	//create the first rendering, to see anything and to test the camera, the buffers, the shares and to overall architectural frame:
	//TODO
//	SimulationResourceManager::getInstance().getScene()->root().addChild(
//		new Box("MyBox",AmendedTransform(Vector3D(0,0,-5)),Vector3D(3,2,1))
//	);


}



void convertBGRAtoRGBA(BYTE* vec4uInt8Image, int numElements)
{
	BYTE tmp;
	for(int i = 0 ; i< numElements; i++ )
	{
		//exchange component 0 and 2 (R and B)
		tmp = vec4uInt8Image[4*i];
		vec4uInt8Image[4*i]= vec4uInt8Image[4*i + 2];
		vec4uInt8Image[4*i + 2] = tmp;
	}
}

void shiftUnsignedByteToSignedByteForNormalMapping(BYTE* unsignedArray, int sizeInByte)
{
	signed char* signedPtrToUnsignedArray = reinterpret_cast<signed char*>(unsignedArray);

	for(int i = 0 ; i< sizeInByte; i++ )
	{
		//haxx: work on the same array with different type-interpreters
		//cast unsigned byte to signed int, because we need "place" for the sign, subtract 128, AND it with an 8-bit-flag(redundant, but to be sure..),
		//then cast back the int to signed byte, as now, the value is in the valid range [-128..127]
		signedPtrToUnsignedArray[i] =  static_cast<signed char>( (static_cast<int>(unsignedArray[i]) - 128) & 0xFFFF );
	}
}

void  addAlphaChannelToVec3FImage(const Vector3D* vec3Image, Vector4D* newVec4Image, int numElements)
{
	for(int i = 0 ; i< numElements; i++ )
	{
		newVec4Image[i] = Vector4D( vec3Image[i], 1.0f);
	}
}

Texture* Loader::loadTexture(String name,  BufferSemantics bufferSemantics, Path fileName,
		TexelInfo texelPreferredLayout,
		bool allocHostMemory, bool shareWithOpenCL,  bool genMipmaps
) throw(BufferException)
{
	 fipImage * image = new fipImage();
	 image->load(fileName.string().c_str());
	 LOG<<INFO_LOG_LEVEL<< "Loading image with path "
	          << fileName.string()
	          << "; Bits Per Pixel: "
	          << image->getBitsPerPixel()
	          << "; width: " << image->getWidth()
	          << "; height" << image->getHeight()<<";\n";



	 assert(sizeof(Vector4D) == 4* sizeof(float) && "Vector types must be tightly packed");
	 //alloc buffer of maximum size; maybe only a fourth will be needed, but that does'nt matter,
	 //as it's only a temorary buffer
	 void* buffer = malloc( sizeof(Vector4D) * image->getWidth()*image->getHeight());


	 //fill buffer according to a rater sophisticated conversion:
	 transformPixelData(bufferSemantics,buffer,texelPreferredLayout,image);

	  int dimensionality = (image->getHeight() >1) ? 2 : 1;

	  Texture* returnTex = 0;

	  if(dimensionality == 1)
	  {
		  returnTex = new Texture1D(
				  name,
				  bufferSemantics,
				  image->getWidth(),
				  texelPreferredLayout,
				  allocHostMemory,
				  buffer,
				  	// buffer
				  	//  ?	reinterpret_cast<void*>(buffer)
					//  : reinterpret_cast<void*>(image->accessPixels()),
				  genMipmaps);

	  }
	  else
	  {
		  returnTex = new Texture2D(
				  name,
				  bufferSemantics,
				  image->getWidth(),
				  image->getHeight(),
				  texelPreferredLayout,
				  allocHostMemory,
				  shareWithOpenCL,
				  //dont't make rectangle; Rectangle is good for deferred rendering, not for decal textureing
				  false,
				  buffer,
				  	// buffer
				  	//  ?	reinterpret_cast<void*>(buffer)
					//  : reinterpret_cast<void*>(image->accessPixels())
				  genMipmaps);
	  }


		//	  if(buffer)
		//	  {
		//		  delete buffer;
		//	  }
	  free(buffer);
	  delete image;

	  return returnTex;
}


Texture2DCube* Loader::loadCubeTexture(
		String name,  BufferSemantics bufferSemantics, Path fileName, String fileEndingWithoutDot,
					TexelInfo texelPreferredLayout,
					bool allocHostMemory,  bool genMipmaps
)throw(BufferException)//may be changed by the loading routine!
{
	const String suffixes[] = {"_RT", "_LF", "_DN", "_UP", "_FR", "_BK"};


	fipImage * image = new fipImage();


	assert(sizeof(Vector4D) == 4* sizeof(float) && "Vector types must be tightly packed");
	//alloc buffer of maximum size; maybe only a fourth will be needed, but that does'nt matter,
	//as it's only a temorary buffer
	void* buffer = 0;


	unsigned int oldDimensions;
	for(int runner=0;runner<6;runner++)
	{
		String currentFileName = fileName.string();
		currentFileName.append(suffixes[runner]);
		currentFileName.append(".");
		currentFileName.append(fileEndingWithoutDot);

		image->load(currentFileName.c_str());
		 LOG<<INFO_LOG_LEVEL<< "Loading image with path "
		          << currentFileName
		          << "; Bits Per Pixel: "
		          << image->getBitsPerPixel()
		          << "; width: " << image->getWidth()
		          << "; height" << image->getHeight()<<";\n";

		int bufferOffset;

		if(runner==0)
		{
			buffer = malloc( 6*  sizeof(Vector4D) * image->getWidth()*image->getHeight());
			bufferOffset =0;
			oldDimensions = image->getHeight();
		}
		else
		{
			//texelPreferredLayoutis only valid unti transformPixelData() has been called once;
			//luckily, its values aren't needed before
			bufferOffset =
					runner *
					texelPreferredLayout.bitsPerChannel / sizeof(BYTE) *
					texelPreferredLayout.numChannels *
					image->getWidth()*image->getHeight();
		}


		//fill buffer according to a rater sophisticated conversion:
		transformPixelData(bufferSemantics,
				//get adress of buffer at the current offset
				&(reinterpret_cast<BYTE*>(buffer)[bufferOffset]),
				texelPreferredLayout,image);

		if(runner==0)
		{oldDimensions = image->getHeight();}


		if(
			(image->getHeight() != image->getWidth())
			||
			! BufferHelper::isPowerOfTwo(image->getHeight())
			|| (image->getHeight() != oldDimensions)
		)
		{
			throw(BufferException("cubetex should be power of two,squared and all images should have the same size!"));
		}
		image->clear();
	}

	Texture2DCube* returnTex =
			new Texture2DCube(
					name, image->getHeight(), texelPreferredLayout,allocHostMemory,
					buffer,genMipmaps
			);

	delete image;
	free(buffer);

	return returnTex;
}



void Loader::transformPixelData(BufferSemantics bufferSemantics,
		//inout buffers
		void* buffer, TexelInfo& texelLayout, fipImage* image)
{
	texelLayout.validate();

	  //Vector4D* buffer = 0;
	  //assert(sizeof(Vector4D) == 4* sizeof(float) && "Vector types must be tightly packed");

	  switch(image->getImageType())
	  {
	  case FIT_BITMAP:

		  if(image->getBitsPerPixel() != 32)
		  {
			  if ( ! (image->convertTo32Bits()) )
			  {
				  throw(BufferException("conversion of image to 32 bit per texel failed"));
			  }
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
			  convertBGRAtoRGBA(image->accessPixels(), image->getWidth()*image->getHeight());
#endif

			  if(bufferSemantics == DISPLACEMENT_SEMANTICS)
			  {
				  LOG<<WARNING_LOG_LEVEL<<"Trying the shiftUnsignedByteToSignedByteForNormalMapping; "
						 << "This is experimental; in case of bugs, check  Loader::loadTexture()";
				  //normalmapping adoption :)
				  shiftUnsignedByteToSignedByteForNormalMapping(image->accessPixels(), image->getImageSize());
				  texelLayout = TexelInfo(4,GPU_DATA_TYPE_INT,8,true);
			  }
			  else
			  {
				  //override to default as i don't have time for sophisticated adoption atm
				  texelLayout = TexelInfo(4,GPU_DATA_TYPE_UINT,8,true);
			  }

			  //copy altered image contents to the designated buffer
			  memcpy( buffer,
					  reinterpret_cast<void*>(image->accessPixels()),
					  texelLayout.numChannels * sizeof(BYTE) * image->getWidth()* image->getHeight()
			  );
		  }
		  break;
	  case FIT_RGBAF:
		  texelLayout = TexelInfo(4,GPU_DATA_TYPE_FLOAT,32,false);

		  //copy altered image contents to the designated buffer
		  memcpy( buffer,
					  reinterpret_cast<void*>(image->accessPixels()),
					  texelLayout.numChannels * sizeof(float) * image->getWidth()* image->getHeight()
		  );
		  break;
	  case FIT_RGBF:
		  	  //add alpha channel for alignment purposes; freeimange doesn't support this conversion,
		  	  //so let's hack it for ourselves:
		  	  //buffer	= new Vector4D[image->getWidth()*image->getHeight()];

		  	  assert((image->getBitsPerPixel()==96) && "it is really 32bit three component image" );

		  	  addAlphaChannelToVec3FImage(
		  			reinterpret_cast<Vector3D*>(image->accessPixels()),
		  			reinterpret_cast<Vector4D*> (buffer),
		  			image->getWidth()*image->getHeight());

		  	  texelLayout = TexelInfo(4,GPU_DATA_TYPE_FLOAT,32,false);

			  //copy altered image contents to the designated buffer
			  memcpy( buffer,
						  reinterpret_cast<void*>(image->accessPixels()),
						  texelLayout.numChannels * sizeof(float) * image->getWidth()* image->getHeight()
			  );
		  break;
	  default:
		  throw(BufferException("sorry, there is no other image type but "
				  "floating point or Bitmap(i.e. 8 bit unsigned normalized int) RGB(A) supported yet "));
	  }
}


//--------------------------------------------------------------------------------



void Loader::loadGlobalConfig(Config& config)
{
	Path dummy(FLEWNIT_DEFAULT_CONFIG_PATH);
	loadGlobalConfig(config, dummy);
}

void Loader::loadGlobalConfig(Config& config, const Path & pathToGlobalConfigFile)
{
	LOG<<INFO_LOG_LEVEL<< "Loading global Config;\n";

	TiXmlDocument XMLDoc ("flewnitGlobalConfig");

	try {
		if(! XMLDoc.LoadFile(pathToGlobalConfigFile.string())
			|| XMLDoc.Error())
		{
			throw std::exception();
		}

		LOG<<INFO_LOG_LEVEL<<"Config file has root node named "<<
				XMLDoc.RootElement()->ValueStr()<<" ;\n";


		//config.rootPtr() = new ConfigStructNode();
		//config.root() = parseElement();

		//check for children:
		TiXmlNode* child = 0;
		TiXmlNode* rootXmlNode =  XMLDoc.RootElement();
		while( ( child = rootXmlNode->IterateChildren( child ) ) != NULL)
		{
			//parse only "elements":
			TiXmlElement* childElement = child->ToElement();
			if(childElement)
			{
				//returnNode->get(child->ValueStr())= parseElement(childElement);
				config.root()[child->ValueStr()].push_back( parseElement(childElement) );
			}
		}


	} catch (...) {
		LOG<<ERROR_LOG_LEVEL<< "[Loader::loadGlobalConfig] Error loading file \""<< pathToGlobalConfigFile.string() <<"\";"
				<< XMLDoc.ErrorDesc()
				<<"; No default config initialization yet exists;\n";
		assert(0);
	}
}




ConfigStructNode* Loader::parseElement(TiXmlElement* xmlElementNode)
{
	ConfigStructNode* returnNode = 0;

	const String* typeOfNode = xmlElementNode->Attribute(String("type"));
	const String nodeName = xmlElementNode->ValueStr();


	if(typeOfNode)
	{
		//ok, this is a "value" node which could be interesting for GUI manipulation:
		GUIParams guiParams;
		getGUIParams(xmlElementNode,guiParams);

		if( *typeOfNode == String("STRING") )
		{
			returnNode = new ConfigValueNode<String>(
					nodeName,
					//dereference String pointer
					*( xmlElementNode->Attribute(String("value")) )
					, guiParams);
		}

		if( *typeOfNode == String("BOOL") )
		{
			returnNode = new ConfigValueNode<bool>(
					nodeName,
					 *(xmlElementNode->Attribute(String("value"))) == String("true")
					 ? true : false
					,guiParams);
		}

		if( *typeOfNode == String("INT") )
		{
			returnNode = new ConfigValueNode<int>(
					nodeName,
					 boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("value") )) )
					,guiParams);
		}

		if( *typeOfNode == String("FLOAT") )
		{
			returnNode = new ConfigValueNode<float>(
					nodeName,
					boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("value") )) )
					,guiParams);
		}




		if( *typeOfNode == String("VEC2I") )
		{
			returnNode = new ConfigValueNode<Vector2Di>(
					nodeName,
					Vector2Di(
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("y") )) )
					)
					,guiParams);
		}

		if( *typeOfNode == String("VEC3I") )
		{
			returnNode = new ConfigValueNode<Vector3Di>(
					nodeName,
					Vector3Di(
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("y") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("z") )) )
					)
					,guiParams);
		}


		if( *typeOfNode == String("VEC4I") )
		{
			returnNode = new ConfigValueNode<Vector4Di>(
					nodeName,
					Vector4Di(
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("y") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("z") )) ),
							boost::lexical_cast<int>( *(xmlElementNode->Attribute( String("w") )) )
					)
					,guiParams);
		}




		if( *typeOfNode == String("VEC2") )
		{
			returnNode = new ConfigValueNode<Vector2D>(
					nodeName,
					Vector2D(
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("y") )) )
					)
					,guiParams);
		}

		if( *typeOfNode == String("VEC3") )
		{
			returnNode = new ConfigValueNode<Vector3D>(
					nodeName,
					Vector3D(
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("y") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("z") )) )
					)
					,guiParams);
		}


		if( *typeOfNode == String("VEC4") )
		{
			returnNode = new ConfigValueNode<Vector4D>(
					nodeName,
					Vector4D(
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("x") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("y") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("z") )) ),
							boost::lexical_cast<float>( *(xmlElementNode->Attribute( String("w") )) )
					)
					,guiParams);
		}

		//TODO rest
		if(! returnNode)
		{
			LOG<<ERROR_LOG_LEVEL<<"Loader::parseElement: unknown \"type\" string in XML config element: "<<
					(*typeOfNode) <<";\n";

		}
	}
	else
	{
		//it's no "value" node but a container node:
		returnNode = new ConfigStructNode(nodeName);
	}

	//check for children:
	TiXmlNode* child = 0;
	while( ( child = xmlElementNode->IterateChildren( child ) ) != NULL )
	{
		//parse only "elements":
		TiXmlElement* childElement = child->ToElement();
		if(childElement)
		{
			(*returnNode) [ child->ValueStr() ] . push_back( parseElement(childElement) );
		}
	}


	return returnNode;
}

void Loader::getGUIParams(TiXmlElement* xmlElementNode, GUIParams& guiParams)
{
	const String* guiVisibilityOfNode = xmlElementNode->Attribute(String("GUIVisibility"));

	if(guiVisibilityOfNode)
	{
		if(*(guiVisibilityOfNode) == String("read"))
		{
			guiParams.setGUIVisibility( ACCESS_READ );
		}
		else
		{
			if(*(guiVisibilityOfNode) == String("read/write"))
			{
				guiParams.setGUIVisibility( ACCESS_READWRITE );
			}
			else
			{
				if(*(guiVisibilityOfNode) == String("none"))
				{
					guiParams.setGUIVisibility( ACCESS_NONE );
				}
				else
				{
					LOG<<ERROR_LOG_LEVEL<<"GUIVisibility String \""<< *guiVisibilityOfNode << "\" invalid; \n";
				}
			}
		}

		//get the tweakConfigString
		const String* tweakConfigString = xmlElementNode->Attribute(String("tweakConfigString"));
		if(tweakConfigString)
		{
			guiParams.setGUIPropertyString(*tweakConfigString);
		}
	}
	//else do nothing, as the defaul constructor of GUIParams allready has initialized its values;


}



}



