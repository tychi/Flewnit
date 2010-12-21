/*
 * LoaderHelper.h
 *
 *  Created on: Dec 9, 2010
 *      Author: tychi
 *
 * private Header with helper functions
 */

#pragma once


#include "Common/FlewnitSharedDefinitions.h"

#include "Common/Math.h"

#include "Config.h"
#include "Util/Log/Log.h"

#include <exception>

//#include <boost/filesystem/path.hpp>
//#include <boost/lexical_cast.hpp>
//
//#include <tinyxml.h>

namespace Flewnit
{

class ConfigCastException : public std::exception
{
	String mDescription;
 public:
	ConfigCastException(String description = "unspecified config-cast error") throw()
	: mDescription(description)
	{ }

	virtual ~ConfigCastException() throw(){}

	virtual const char* what() const throw()
	{
	    return mDescription.c_str();
	}
};



class ConfigCaster
{
public:
	template<typename T>
	static T cast(ConfigStructNode& csn) throw(ConfigCastException)
	{
		ConfigValueNode<T>* cvn = dynamic_cast<ConfigValueNode<T>* > ( & csn);

		if(cvn)
		{
			return cvn->value();
		}
		else
		{
			throw(	ConfigCastException()	);
			//assert("Bad cast of ConfigStructNode!" && 0);
			//maybe should reaqlize type-prettyprinting... but later...
		}
	}

};


}
