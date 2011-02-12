{%comment%}
  GLSL Shader Template: Data to be exchanged between Vertex/Geom/TessEval and Fragment shader shader
  applicable to following stages: fragment     {%endcomment%} 
  
struct InterfaceData
{  
  
{%comment%} ################################# following "coloring" inputs ################################################################### {%endcomment%}
{% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
  vec4 position;
  vec4 normal;
  {% if SHADING_FEATURE_NORMAL_MAPPING %}
    {%comment%} 
      create TBN-matrix in fragment shader due to the several lightsources (we cannot pass a lightsource-to-fragment
      vector in tangent space from vert/geom/tessEvsal shder to fragment shader,
      because we would have to to this for every light, and the data exchange interface between shader stages is not easily extensible);
      hence, we transform instead the normals from tangent space to world space in the fragment shader (via putting the vectors 
      column-wise into a 3x3-matrix: mat3(t,b,n)), and perform the lighting calculations in world space;                                  {%endcomment%}
    vec4 tangent;
  {% endif %}
  
  {% if SHADING_FEATURE_DECAL_TEXTURING or SHADING_FEATURE_DETAIL_TEXTURING	%}
    vec4 texCoords;
  {%endif%}
  {% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
   {%comment%} later TODO test this optimaziation for one shadowmap; this would save one multiplication 
               of the fragment world position with the biased sm-MVP matrix in the fragment shader         {%endcomment%}
    vec4 shadowCoord;
  {% endif %}
{% endif %}  {%comment%} end of "coloring" inputs {%endcomment%}
	
{%comment%} ################################# following shadow/pos/depth inputs ############################################################## {%endcomment%}
{% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION or RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION  %}
 
  {%comment%} haxx as the engine cannot combine AND and OR ;):
              read:  if we wanna make a pointlight shadow map or an AO depth map, then ..{%endcomment%} 
  {% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION %} 
   {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION %}
      //goal: writing a linear viewspace depthvalue, scaled to [0..1] to the gl_FragDepth;
      //following variable listing in descending optimazation; i will begin with the non optimized and hence fewest error prone one
      //float depthViewSpaceNORMALIZED;
      //float depthViewSpaceUNSCALED;
      //vec4 positionViewSpaceNORMALIZED; //light space linear coords, scaled by inverse farclipplane of lightsource camera ({{inverse_lightSourcesFarClipPlane}})
      vec4 positionViewSpaceUNSCALED; //light space linear coords, unscaled to test the most simple case before the more error prone optimized one
    {% endif %} 
  {% endif %}    
   
  {% if RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION %}  
    vec4 position;
  {% endif %}
  
  {%comment%} for default spotlight shadowmap generation, there is no fragment shader necessary at all, hence no input variable {%endcomment%}
{% endif %}

{%comment%} ################################# following ID inputs ##############################################################{%endcomment%}
{% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION %}
  ivec4 genericIndices;
{% endif %}
 
 }; //end InterfacerData
