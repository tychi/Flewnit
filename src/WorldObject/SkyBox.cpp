/*
 * SkyBox.cpp
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#include "SkyBox.h"

namespace Flewnit {

SkyBox::SkyBox(Path cubeMapDirectory, String cubeMapFilename, String fileEnding,const AmendedTransform& localtransform)
: PureVisualObject(cubeMapFilename,localtransform)
{
	assert(0 && "SkyBox::SkyBox: implement me");
	// TODO Auto-generated constructor stub

}

SkyBox::~SkyBox()
{
	// TODO Auto-generated destructor stub
}

}
