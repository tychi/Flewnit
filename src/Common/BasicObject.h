/**
 *
 * Class BasicObject.h
 *
 * This is kind of a debug class for tracking memory leaks and object management; In Release configurations, it should have
 * no functionality.
 *
 * Why i don't use QObject and qt's meta-stuff for this? I don't want qt to penetrate too much of my code, and i fear a performance and memory
 * impact. So I'll implement the neccessary stuff for myself.
 *
 */


#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include <iostream>


namespace Flewnit
{

	class BasicObject
	{
	public:
		BasicObject()
		{

		}

		~BasicObject()
		{

		}

	private:
		std::string mClassName;
		unsigned int mUniqueID;
	};

}
