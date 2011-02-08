//{%comment%}
//	GLSL Shader Template: Data type definitions (also includable by c++ code to guarantee compatiblity):
//	applicable to following stages: fragment
//{%endcomment%}
//make some mods so that the c++-code can use this definition, too
#ifdef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#	define NAMESPACE_PREFIX glm::
#else
#	define NAMESPACE_PREFIX	
#endif

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
	//------------------------------------------------------------------------------------------------
	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
	//to indicate a point light, those valus are all zero
	NAMESPACE_PREFIX vec3 direction;
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;	//serves also as indicator if the source shall be treated as spot or not (zero= pointlight ;) )
	float outerSpotCutOff_Radians;
	float spotExponent;

	//if is negative, it's an indicator that this source doesn't cast shadows ---------------------------
	float shadowMapLayer;
	//aligned to 64 bytes;
	//{%comment%} //tagged as comment for grantlee, so that this c++- specific section is not included in the shader code
		LightSourceShaderStruct(Vector3D position, Vector3D diffuseColor, Vector3D specularColor, Vector3D direction,
			float innerSpotCutOff_Radians, float outerSpotCutOff_Radians , float spotExponent, float shadowMapLayer)
		:
		position(position),diffuseColor(diffuseColor),specularColor(specularColor), direction(specularColor),
		innerSpotCutOff_Radians(innerSpotCutOff_Radians), outerSpotCutOff_Radians(outerSpotCutOff_Radians),
		spotExponent(spotExponent), shadowMapLayer(shadowMapLayer)
		{}
		
		bool isSpotLight() {return innerSpotCutOff_Radians==0.0f;}
	//{%endcomment%}
};

