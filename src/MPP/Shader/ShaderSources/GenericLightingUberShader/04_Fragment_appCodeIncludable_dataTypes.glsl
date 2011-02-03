//GLSL Shader Template: Data type definitions:

//applicable to following stages: fragment

//make some mods so that the c++-code can use this definition, too
#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#	define NAMESPACE_PREFIX glm::
#else
#	define NAMESPACE_PREFIX	
#endif


//{%typeDefinitions}
//{

#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
struct LightSourceShaderStruct
#else
struct LightSource
#endif
{
	NAMESPACE_PREFIX vec3 position;
	NAMESPACE_PREFIX vec3 diffuseColor;
	//a dedicated specular color to produce unrealistic but nice effects;	
	NAMESPACE_PREFIX vec3 specularColor;

	//----------------------------------------------------------------------
	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
	//to indicate a point light, those valus are all zero
	NAMESPACE_PREFIX vec3 direction;
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;	//serves also as indicator if the source shall be treated as spot or not (zero= pointlight ;) )
	float outerSpotCutOff_Radians;
	float spotExponent;
	//alignment is everything :P
	
	float shadowMapLayer;


	//aligned to 64 bytes;
	
	#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
	
	LightSourceShaderStruct(Vector3D position, Vector3D diffuseColor, Vector3D specularColor, Vector3D direction,
			float innerSpotCutOff_Radians, float outerSpotCutOff_Radians , float spotExponent, float shadowMapLayer)
	:
	position(position),diffuseColor(diffuseColor),specularColor(specularColor), direction(specularColor),
	innerSpotCutOff_Radians(innerSpotCutOff_Radians), outerSpotCutOff_Radians(outerSpotCutOff_Radians),
	spotExponent(spotExponent), shadowMapLayer(shadowMapLayer)
	{}
	
	/*
	bool operator==(const LightSourceShaderStruct& rhs)
	{
		return
				(position == rhs.position ) &&
				(diffuseColor == rhs.diffuseColor ) &&
				( specularColor== rhs.specularColor ) &&
				(direction == rhs. direction) &&
				( innerSpotCutOff_Radians== rhs.innerSpotCutOff_Radians ) &&
				(outerSpotCutOff_Radians == rhs.outerSpotCutOff_Radians ) &&
				(spotExponent == rhs.spotExponent ) &&
				( shadowMapLayer== rhs.shadowMapLayer );
	}
	*/
	
	bool isSpotLight()
	{
		return innerSpotCutOff_Radians==0.0f;
	}
#endif
};
//}
