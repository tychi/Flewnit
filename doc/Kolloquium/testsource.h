vec4 fragmentColor =  	
	{% if SHADING_FEATURE_DIFFUSE_TEXTURING %}  
		texture(decalTexture,input.texCoords.xy);
	{% else %} color;
	{% endif %}  

#define NUM_BITS_PER_KEY_TO_SORT ( {{ numBitsPerKey }} )


{% block performSPHCalculations %}
	{% comment %}
	the core of the physics simulation:
	accumulate all relevant values
	(density, pressure force, viscosity force etc ...)
	{% endcomment %}
{% endblock performSPHCalculations %}

{% block performSPHCalculations %}
	if( BELONGS_TO_FLUID(
			GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID  ) )
	{
		ownDensity +=
			cObjectGenericFeatures [ GET_CURRENT_NEIGHBOUR_PARTICLE_OBJECT_ID  ].massPerParticle
			* poly6( ownPosition -  GET_CURRENT_NEIGHBOUR_POS , cSimParams );
	}
{% endblock performSPHCalculations %}


gl_TessLevelOuter[gl_InvocationID] =
 clamp(
      edgeLengthViewSpace
    	* numScreenPixels
    	* tessQualityFactor
    	* distanceFactor,
    minTessLevel, maxTessLevel );
barrier(); //------------------------------------------
if (gl_InvocationID == 0)
{
  gl_TessLevelInner[0] = max(max(gl_TessLevelOuter[0],gl_TessLevelOuter[1]),gl_TessLevelOuter[2]) ;
}


output.position +=
  normalize(output.normal) *
  displacementVal * //looked up from displ. map via interpolated tex.Coord.
  displacementIntensity * //user-defined tweak-value
  weightedDistanceDependentDisplacementFactor //dist.-dependent morphing-value for smooth transitions
  ;
