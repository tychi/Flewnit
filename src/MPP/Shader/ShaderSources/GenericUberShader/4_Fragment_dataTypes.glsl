//GLSL Shader Template: Data type definitions:

//applicable to following stages: fragment


//{%typeDefinitions}
//{
struct LightSource
{
	vec3 position;
	vec3 diffuseColor;
	//a dedicated specular color to produce unrealistic but nice effects;	
	vec3 specularColor;

	//----------------------------------------------------------------------
	//following spotlight stuff, but set it anyway for alignment reasons, even if it won't be used in the shader!
	//to indicate a point light, those valus are all zero
	vec3 direction;
	//value beyond with will be no lighting, to produce a nice light circle and to 
	//hide the rectangular shape of the shadowmap ;)
	float innerSpotCutOff_Radians;	//serves also as indicator if the source shall be treated as spot or not (zero= pointlight ;) )
	float outerSpotCutOff_Radians;
	float spotExponent;
	//alignment is everything :P
	
	//became obsolte
	//int shadowMapLayer;

	float pad;
	
	//float lightSourceAmbientFactor;

	//aligned to 64 bytes;
};
//}
