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

	BOOST_FOREACH( ConfigMap::value_type & mapNode, mChildren )
	{
		BOOST_FOREACH(ConfigStructNode* vecNode, mapNode.second )
		{
			//delete mapNode.second[runner];
			delete vecNode;
		}
	}

	//tryout stuff: obsolte
	//	LOG<<DEBUG_LOG_LEVEL<<"destroying ConfigStructNode; hope to kill all children of this:P ;\n";
	//	for(Map<String, ConfigStructNode*>::iterator it = mChildren.begin();
	//			it!= mChildren.end();
	//			it++)
	//	{
	//		LOG<<DEBUG_LOG_LEVEL<<"destroying Child named \""<< it->first <<"\" of ConfigStructNode;\n";
	//		delete it->second;
	//	}
}




std::vector<ConfigStructNode*>& ConfigStructNode::operator[](String name)
{

//	if(mChildren.find(name) ==  mChildren.end())
//	{
//		LOG<< ERROR_LOG_LEVEL<<"For usage of this operator, the element must Exist, otherwise you'd dereference an uninitialized pointer;\n";
//		assert(0);
//	}

	return mChildren[name];
}

std::vector<ConfigStructNode*>& ConfigStructNode::get(String name)
{
	assert("requested config entry exists" && childExists(name,0));
	return mChildren[name];
}

ConfigStructNode& ConfigStructNode::get(String name, int index)
{
	assert("requested config entry exists" && childExists(name,index));
	return *(mChildren[name][index]);
}

bool ConfigStructNode::childExists(String name, int index)
{
	return 	//map entry exists?
			(mChildren.find(name) != mChildren.end() ) &&
			//index range valid?
			//TODO check if c++ really does early evaluation cancellation;
			//if not, this would create an invalid entry!
			( (unsigned int) (index) < mChildren[name].size()) ;
}



}
