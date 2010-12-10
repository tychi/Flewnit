/*
 * Config.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Config.h"

#include "Common/Math.h"
#include "Util/Log/Log.h"

#include <boost/foreach.hpp>


#include <typeinfo>

namespace Flewnit
{



ConfigStructNode::~ConfigStructNode()
{
	LOG<<DEBUG_LOG_LEVEL<<"destroying ConfigStructNode; hope to kill all children of this:P ;\n";
	for(Map<String, ConfigStructNode*>::iterator it = mChildren.begin();
			it!= mChildren.end();
			it++)
	{
		LOG<<DEBUG_LOG_LEVEL<<"destroying Child named \""<< it->first <<"\" of ConfigStructNode;\n";
		delete it->second;
	}


	//wtf this alway omits the first element!:
//	typedef Map<String, ConfigStructNode*> mapDummyType;
//	BOOST_FOREACH( mapDummyType::value_type & node, mChildren )
//		{
//			delete node.second;
//		}
}

//ConfigValueNode::~ConfigValueNode()
//{
//	LOG<<DEBUG_LOG_LEVEL<<"detrayoing configvaluenode; hope to kill all children of this:P ;\n";
//}


ConfigStructNode& ConfigStructNode::operator[](String name)
{

	if(mChildren.find(name) ==  mChildren.end())
	{
		LOG<< ERROR_LOG_LEVEL<<"For usage of this operator, the element must Exist, otherwise you'd dereference an uninitialized pointer;\n";
		assert(0);
	}

	return *(mChildren[name]);
}



}
