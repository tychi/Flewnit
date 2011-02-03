//GLSL Shader Template: distance attenuation calculations:

//applicable to following stages: fragment


//{%subroutine_getDistanceAttenuation}
//{
	float 	getDistanceAttenuation(vec3 lightToFragW) 			
	{
		/*	
		distanceAttenuation formula hopefully more performant than the following:
		distanceAttenuation = 1.0 / (length(fragToLight)* length(fragToLight));
		(first take a sqrt, than take the reciprocal of its sqare... this is too much overhead :P)	
		*/
		return  1.0 / ( (lightToFragW.x * lightToFragW.x) + (lightToFragW.y * lightToFragW.y) + (lightToFragW.z * lightToFragW.z) );
	}
//}

