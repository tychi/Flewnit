{%comment%}
  GLSL Shader Template: Fragment inputs from Vertex/Geom/TessEval shader
  applicable to following stages: fragment     {%endcomment%} 
  
{%comment%} ################################# following "coloring" inputs ################################################################### {%endcomment%}
{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
  in vec4 inFPosition;
  in vec4 inFNormal;
  in vec4 inFTexCoords;
  {% if SHADING_FEATURE_NORMAL_MAPPING %}
    {%comment%} 
      create TBN-matrix in fragment shader due to the several lightsources (we cannot pass a lightsource-to-fragment
      vector in tangent space from vert/geom/tessEvsal shder to fragment shader,
      because we would have to to this for every light, and the data exchange interface between shader stages is not easily extensible);
      hence, we transform instead the normals from tangent space to world space in the fragment shader (via putting the vectors 
      column-wise into a 3x3-matrix: mat3(t,b,n)), and perform the lighting calculations in world space;                                  {%endcomment%}
    in vec4 inFTangent;
   {% endif %}
  {% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
   {%comment%} later TODO test this optimaziation for one shadowmap; this would save one multiplication 
               of the fragment world position with the biased sm-MVP matrix in the fragment shader         {%endcomment%}
    in vec4 inFShadowCoord;
  {% endif %}
{% endif %}  {%comment%} end of "coloring" inputs {%endcomment%}
	
{%comment%} ################################# following shadow/pos/depth inputs ##############################################################{%endcomment%}
{% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION or RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION  %}
  {% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION and LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT %}
    in vec4 inFPosition_LS_norm; //light space linear coords, scaled by inverse farclipplane of lightsource camera ({{inverse_lightSourcesFarClipPlane}})
  {% else %}{% if RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION %}
    in vec4 inFPosition;
  {% else %}{% if RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION %}  
    in float inFDepthView;  //just the linear z value in view space, for usage in a non-deferred AO contexts; 
                            //if this value will be scaled to [0..1] via the farclipplane and written to gl_FragDeapth
                            //or just written unscaled to a single componentent color texture has still TO BE DETERMINED
  {% endif %}{% endif %}{% endif %}
  {%comment%} for default spotlight shadowmap generation, there is no fragment shader necessary at all, hence no input {%endcomment%}
{% endif %}

{%comment%} ################################# following ID inputs ##############################################################{%endcomment%}
{% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION %}
  in ivec4 inFGenericIndices;
{% endif %}

