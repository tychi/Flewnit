/*
 * LightSourceManager.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: tychi
 */

#include "LightSourceManager.h"

namespace Flewnit
{

LightSourceManager::LightSourceManager(
		LightSourcesLightingFeature lightSourcesLightingFeature,
		LightSourcesShadowFeature lightSourcesShadowFeature,
		int maxLightSources)
{

}


LightSourceManager::~LightSourceManager()
{

}


//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
PointLight* LightSourceManager::createPointLight(
		const Vector3D& position,
		bool castsShadows,
		const Vector3D& diffuseColor,
		const Vector3D& specularColor
) throw(SimulatorException)
{

}


//throws exception if mNumMaxLightSources lightsources already exists or if the
//lighting feature contradicts pointlights;
//if castsShadows contradicts the shadowing feature, a warning is issues,
//and the compatible value is set, i.e. there is o guarantee that the user's
//wish is fulfilled
SpotLight* LightSourceManager::createSpotLight(
		const Vector3D& position,
		const Vector3D& direction,
		bool castsShadows,
		float innerSpotCutOff_Degrees,
		float outerSpotCutOff_Degrees,
		float spotExponent,
		const Vector3D& diffuseColor,
		const Vector3D& specularColor
) throw(SimulatorException)
{

}



//FrustumCulling wont't be implemented too soon ;(
void LightSourceManager::renderShadowMaps(float maxDistanceToMainCam, bool doFrustumCulling)
{

}

//fill buffers with recent values
void LightSourceManager::setupBuffersForShading(float maxDistanceToMainCam)
{

}




//important to omit f***up: when the LS manager is destroying ls'es ITSELF, it
//has to remove the list-entry before actually call the destructor on the LS;
void LightSourceManager::unregisterLightSource(LightSource* ls)
{

}



}
