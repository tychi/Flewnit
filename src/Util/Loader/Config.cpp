/*
 * Config.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Config.h"

#include <boost/foreach.hpp>

namespace Flewnit
{



ConfigStructNode::~ConfigStructNode()
{
	typedef Map<String, ConfigNodeInterface*> mapDummyType;
	BOOST_FOREACH( mapDummyType::value_type & node, mChildren )
		{
			delete node.second;
		}
}

}
