/*
 * ShaderManager.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#include "ShaderManager.h"
#include "Util/Log/Log.h"

namespace Flewnit {

ShaderManager::ShaderManager(const ShaderFeaturesGlobal& globalShaderFeatures,
		Path codeDirectory )
{
	// TODO Auto-generated constructor stub
//	ShaderFeaturesLocal s;
//	ShaderFeaturesLocal t(RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL);
//
//	boost::unordered_map<ShaderFeaturesLocal, String> testMap;
//
//	testMap[s]= "ASSNESS";
//	testMap[ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,TEXTURE_TYPE_2D_ARRAY)] = "YUMMYMAMMA";
//
//	LOG<<DEBUG_LOG_LEVEL<<"what comes now?"<<testMap[ShaderFeaturesLocal(RENDERING_TECHNIQUE_CUSTOM,TEXTURE_TYPE_2D_ARRAY)];
//	assert(testMap.find(t) == testMap.end());
//
//	mShaderMap[s]= 0;
}

ShaderManager::~ShaderManager() {
	// TODO Auto-generated destructor stub
}

}
