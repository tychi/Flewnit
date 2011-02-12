//Third concept of a vertex shader template, this time in grantlee template engine syntax;

{% comment %} 
  when do we need a geometry shader? 
    - for layered rendering, i.e.:
        - cube shadow map generation
        - dynamic cubemap rendering
        - multiple spotlight shadowmap generation
    - for god rays (won't be implemented too soon ;( )
    - we'll see ;)
{% endcomment %}

{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}

//like in the fragment template no #defines are necessary anymore, because the "preprocessor-masking" 
//is now done much more conveniently by the template engine
//we also need no special data types for fragment/geometry programs


{% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION and LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS %}
  layout(shared) uniform LightSourceMatrixBuffer
  {
    //we need only the view projection matrices, as no pure view space calculations are performed during shadow map generation
    uniform mat4 shadowCameraviewProjectionMatrices[ {{  numMaxShadowCasters }} ];
  };
{% else %}{% if RENDER_TARGET_TEXTURE_TYPE_2D_CUBE or RENDER_TARGET_TEXTURE_TYPE_2D_CUBE_DEPTH %}

    uniform mat4 cubeMapCameraViewMatrices[ 6 ];
    uniform mat4 cubeMapCameraProjectionMatrices[ 6 ];

{% endif %}{% endif %}

//input from vertex shader:

//output to vertex shader

