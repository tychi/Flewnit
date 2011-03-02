{%comment%}
  GLSL Shader Template: uniforms and uniform buffers:
  applicable to following stages: fragment     {%endcomment%} 
  
  uniform vec2 tangensCamFov = vec2 ( {{tangensCamFovHorizontal}}, {{tangensCamFovVertical}}   ); //for position calculation from pure linear depth value;
	uniform vec2 cotangensCamFov= vec2 ( {{cotangensCamFovHorizontal}}, {{cotangensCamFovVertical}}   ); //for texcoord calculation from viewspace position value;/inverso of tanget, pass from outside to save calculations
	uniform float cameraFarClipPlane = {{ cameraFarClipPlane }};
	uniform float invCameraFarClipPlane = {{ invCameraFarClipPlane }};

{% if not SHADING_FEATURE_NONE %}
  
  {% if SHADING_FEATURE_AMBIENT_OCCLUSION %}
    uniform float AOinfluenceRadius;
    uniform float AOattenuation;
	  {%comment%} TODO if really doing AO, we have to determine the texture type, the coordinate system etc. pp.; 
	             this is just a conceptional stub for AO                                                             {%endcomment%} 	
	{%endif%}

  //following some non-dependently generated material relevant uniforms; even if they aren't used by some shader permutations,
  //masking them according to shader features would be overkill; defined and unsued may is better than undefined and written to 
  //by the app
  //{
  uniform int numCurrentlyActiveLightSources =  {{ numMaxLightSources }} ;
  uniform float invNumCurrentlyActiveLightSources = {{ invNumMaxLightSources }} ;
  
  //number of lightsources casting shadows being currently active
  uniform int numCurrentlyActiveShadowCasters = {{ numMaxShadowCasters }} ;
  uniform float invNumCurrentlyActiveShadowShadowCasters =  {{ invNumMaxShadowCasters }} ;

  {% if not RENDERING_TECHNIQUE_DEFERRED_LIGHTING  %}  
  uniform float shininess = 100.0; 
  uniform float reflectivity = 0.25;
  {%comment%} in the deferred case, it is coded into the decal textures' alpha channel; We have to mask this uniform, because the name will
              be re-used in the deferred case {%endcomment%}
  {% endif %}
  //factor of attenuation outside the outer light cone
  uniform float spotLightAmbientFactor = 0.03;
  
  uniform vec4 eyePositionW; //world space eye position, needed e.g. for skydome rendering
  uniform mat4 viewMatrix; //the transpose (=inverse) of the rotational component of the view matrix is needed to transform "envmap lookup vector" from view space back to world space



  //clamp the attenuation due to shadowmapping to [minimalshadowAttenuation, 1.0]
  uniform float minimalshadowAttenuation = 0.2;
  //} end non-dependently generated material relevant uniforms;

  //----- begin lightsource/shadowmap interface ------------------------------------
  {% if LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT or LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT  %}  
    uniform LightSource lightSource;
  {% else %}
    {% if LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS or  LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS %}
      layout(shared) uniform LightSourceBuffer
      {
        LightSource lightSources[ {{ numMaxLightSources }} ];
      };
    {% endif %}  
  {% endif %}
  
  {% if not LIGHT_SOURCES_SHADOW_FEATURE_NONE  %}  
    //begin shadowmap matrix stuff
    {% if LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS %}
    {%comment%}  any hint we need more than one shadowmap transformation matrix?
                  (pointlight cube shadow map counts as one and needs no matrix for lookup)? 
                  (assert also that multiple-lightsource-LIGHTING is enabled)                  {% endcomment %}
    {% if LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS or LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS %}
        //multiple light source shadow map lookup parameters
        layout(shared) uniform WorldToShadowMapMatrixBuffer
        {
          //bias*perspLight*viewLight                  for light calcs in world space and
          //bias*perspLight*viewLight * (camView)⁻1    for light calcs in view space <-- THIS IS WHAT I TOOK!
          mat4 worldToShadowMapMatrices[  {{ numMaxShadowCasters }} ]; 
                                                                      
        };
    {% endif %}    
    {% else %}//single light source shadow map lookup parameters following:
        {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}//only one spot light source for shadowing:
          uniform mat4 worldToShadowMapMatrix; //bias*perspLight*viewLight                for light calcs in world space and
                                               //bias*perspLight*viewLight * (camView)⁻1  for light calcs in view space
        {% endif %}
        {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT %}
          //in case of one point light shadow mapping, the lookup is in "non-1/z" camera space, 
          //i.e. needs no additional transformation matrix when calculating in world space; 
          
          //BUT BEWARE: when calculating in view spcace, we need a matrix:    
          uniform mat4 viewToPointLightShadowMapMatrix; // (inverse lightSource FarClipPlane) * inversepointLightTranslation * (camView)⁻1
          //but because the depth buffer is always clamped to [0..1], the depth value
          //must be scaled by 1/farclipPlane of lightSource camera ({{invCameraFarClipPlane}});
          
          //bias for lookup, in case the glPolygonOffset doesn't apply when writing the gl_FragDepth manually:
          uniform float shadowMapComparisonOffset= -0.01;
        {% endif %}  
    {% endif %}
    //end shadowmap matrix stuff
  {% endif %}
  //-----end lightsource/shadowmap interface -----------------------------------
{% endif %} {%comment%} end if !SHADING_FEATURE_NONE {%endcomment%}
