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
	NAMESPACE_PREFIX vec4 position;
	NAMESPACE_PREFIX vec4 diffuseColor;
	//a dedicated specular color to produce unrealistic but nice effects;	
	NAMESPACE_PREFIX vec4 specularColor;
	//------------------------------------------------------------------------------------------------
	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
	NAMESPACE_PREFIX vec4 direction; //for a pointlight, this is (0,0,-1)
	
	//to indicate a point light, the following values are all zero
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;	//serves also as indicator if the source shall be treated as spot or not (zero= pointlight ;) )
	float outerSpotCutOff_Radians;
	float spotExponent;

	//if is negative, it's an indicator that this source doesn't cast shadows ---------------------------
	float shadowMapLayer;
	
	//aligned to
	//    4 components * 4 byte * 4 member vectors =  64 bytes 
	//  + 1 component  * 4 byte * 4 member scalars =  16 Bytes
	//________________________________________________________
	// total alignment                             =  80 Bytes
	//{%comment%} //tagged as comment for grantlee, so that this c++- specific section is not included in the shader code
		LightSourceShaderStruct(Vector4D position, Vector4D diffuseColor, Vector4D specularColor, Vector4D direction,
			float innerSpotCutOff_Radians, float outerSpotCutOff_Radians , float spotExponent, float shadowMapLayer)
		:
		position(position),diffuseColor(diffuseColor),specularColor(specularColor), direction(specularColor),
		innerSpotCutOff_Radians(innerSpotCutOff_Radians), outerSpotCutOff_Radians(outerSpotCutOff_Radians),
		spotExponent(spotExponent), shadowMapLayer(shadowMapLayer)
		{}
		
		bool isSpotLight() {return innerSpotCutOff_Radians==0.0f;}
	//{%endcomment%}
};
