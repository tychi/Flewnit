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


}


Texture* Loader::loadTexture(String name,  BufferSemantics bufferSemantics, Path fileName,
		const TexelInfo& texelPreferredLayout,
		bool allocHostMemory, bool shareWithOpenCL,  bool genMipmaps
) throw(BufferException)
{
	 fipImage * image = new fipImage();
	 Texture* returnTex = 0;

	 texelPreferredLayout.validate();

	 image->load(fileName.string().c_str());

	  LOG<<INFO_LOG_LEVEL<< "Loading image with path "
	          << fileName.string()
	          << "; Bits Per Pixel: "
	          << image->getBitsPerPixel()
	          << "; width: " << image->getWidth()
	          << "; height" << image->getHeight()<<";\n";


	  int dimentionality = (image->getHeight() !=1) ? 2 : 1;

//	  TextureInfo texInfo(
//				BufferInfo,
//				cl_GLuint dimensionality,
//				Vector3Dui dimensionExtends,
//				const TexelInfo& texelInfo,
//				GLenum textureTarget,);

	  //TODO
//
//	  image->getImageType();
//	  //source code study seems to promise that to 24bit- RGB will be added an 8bit alpha channel with values 0xFF;
//	  image->convertTo32Bits();
//	  image->convertToType(FIT_RGBAF); //FIT_RGBA16
//	  image->getBitsPerPixel();
//
//	  image->getColorType(); // hope to be FIC_RGBALPHA or  FIC_RGB
//	  image->getImageSize(); //in bytes
//	  image->getChannel();
//	  image->accessPixels();
//
//
//	    if (image->getBitsPerPixel() == 32)
//	    {
//	      *glChannelOrder = GL_RGBA;
//	      *texChannelOrder = GL_BGRA;
//	    } else if (image->getBitsPerPixel() == 24) {
//	      *glChannelOrder = GL_RGB;
//	      *texChannelOrder = GL_BGR;
//	    } else {
//	      *glChannelOrder = GL_RGB;
//	      *texChannelOrder = GL_BGR;
//	      Logger::Instance().log("WARNING",
//	              "Texture", "Converting "+ path+ " to 24bits.");
//	      if (image->convertTo24Bits()) {
//	        Logger::Instance().log("WARNING", "Texture", "SUCESS!");
//	      } else {
//	        Logger::Instance().log("ERROR",
//	                "Texture", "Converting "+ path+ " to 24bit failed.");
//	      }
//	    }

	    delete image;

	    return returnTex;

}


Texture2DCube* Loader::loadCubeTexture(
		String name,  BufferSemantics bufferSemantics, Path fileName,
					const TexelInfo& texelPreferredLayout,
					bool allocHostMemory,  bool genMipmaps
)throw(BufferException)
{
	const String suffixes[] = {"_RT", "_LF", "_DN", "_UP", "_FR", "_BK"};
	//TODO
}




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



