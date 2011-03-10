/*
 * SkyDome.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#include "SkyDome.h"
#include "Util/Loader/Loader.h"
#include "Simulator/SimulationResourceManager.h"
#include "WorldObject/SubObject.h"
#include "Material/VisualMaterial.h"
#include "Geometry/Procedural/BoxGeometry.h"
#include "URE.h"
#include "Buffer/Texture.h"

namespace Flewnit {

SkyDome::SkyDome( Path cubeMapFilePath )//Path cubeMapDirectory, String cubeMapFilename, String fileEnding)
: PureVisualObject(String("SkyDome") + Path(cubeMapFilePath.filename()).stem())
{
	BoxGeometry* boxGeo= new BoxGeometry(getName(),Vector3D(50,50,50),false,false);
	String cubeMapFilename = Path(cubeMapFilePath.filename()).stem();
	Texture2DCube* cubeTex = dynamic_cast<Texture2DCube*>(
			SimulationResourceManager::getInstance().getTexture(cubeMapFilename));
	if(!cubeTex)
	{
		cubeTex = URE_INSTANCE->getLoader()->loadCubeTexture(
				cubeMapFilePath,
				ENVMAP_SEMANTICS,
				BufferElementInfo(4,GPU_DATA_TYPE_UINT,8,true),
				true,
				false
		);
	}
	SkyDomeMaterial* skyDomeMat = new SkyDomeMaterial(getName(),cubeTex);
	SubObject* so = new SubObject(getName(),VISUAL_SIM_DOMAIN,boxGeo,skyDomeMat);

	addSubObject(so);

	SimulationResourceManager::getInstance().registerSkydome(this);
}

SkyDome::~SkyDome()
{
	// TODO Auto-generated destructor stub
}

Texture* SkyDome::getCubeMap()
{
	return reinterpret_cast<VisualMaterial*>
		(getSubObjects(VISUAL_SIM_DOMAIN)[0]->getMaterial())->getTexture(ENVMAP_SEMANTICS);
}

}
