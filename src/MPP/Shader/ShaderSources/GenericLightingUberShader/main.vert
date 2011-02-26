//Third concept of a vertex shader template, this time in grantlee template engine syntax;

/**
!  IMPORTANT:  I ASSERTED RESP. ASSUME IN MY FRAMEWORK THAT EVERY GEOMETRY IS ONLY TRNASLATED, ROTATED AND UNIFORMLY SCALED!
!              SKEW OR NON-UNIFORM SCALING HAS NO PLACE IN A SIMULATION CONTEXT (IMHO). FURTHER, THIS RESTRICTION ASSURES THAT
!              NO EXTRA NORMAL MATRIX IS NEEDED, AS THE 3X3 COMPONENT OF (((MODELVIEW)⁻1)T) ONLY DIFFERS FROM MODELVIEW BY THE
!              UNIFORM SCALE FACTOR OF THE MODEL MATRIX; AS WE ONLY NEED THE DRIECTION AND NORMALS WILL NEVER BE NORMALIZED AFTER INTERPOLATION,
!              THIS SCALING OF THE NORMAL BY THE MODELVIEW MATRIX HAS NO NEGATIVE EFFECT;
!              THIS IS WHY IN ALL SHADERS, THE NORMALS WILL BE TRANSFORMED BY THE SAME MATRICES AS THE POSITION COORDINATES!
!              THIS SHALL SIMPLYFY SPECIFIC RENDERING TECCHNIQUES (INSTANCED DYNAMIC ENVIRONMENT MAP GENERATION, E.G., 
!              (WOULD NEED 6* NUM_INSTANCES NORMAL MATRICES STORED OR REPEATEDLY COMPUTED IN SHADER !)) BOTH MATHEMATICALLY AND COMPUTATIONALLY. 
**/

{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}

//like in the fragment template no #defines are necessary anymore, because the "preprocessor-masking" 
//is now done much more conveniently by the template engine

//we also need no special data types for fragment/geometry programs

{% comment %} 
  damn template language: read: 
      if no layered rendering,i.e. neither rendering to cubemap nor rendering to texture array, then define default view matrices;
      otherwise, a geometry shader will provide sveral view matrices and the vertex shader only needs to do the model transform;
{% endcomment %}
{% if not RENDER_TARGET_TEXTURE_TYPE_2D_CUBE and not RENDER_TARGET_TEXTURE_TYPE_2D_CUBE_DEPTH %}
  {% if not RENDERING_TECHNIQUE_SHADOWMAP_GENERATION  or not  LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS  %}
    uniform mat4 viewMatrix;
    uniform mat4 viewProjectionMatrix;
    //uniform int uniqueInstanceID = 0; //default zero
  {% endif %}
{% endif %}

//a bit overkill of matrix permutations, but removing unnecessary ones later is easyier than adding missing ones; those matrices makeing no sense in a certein rendering environment will be ignored;
{% if instancedRendering %}

  struct InstanceTransformInfo
  {
    mat4 modelMatrix;  //needed for layered rendering to be combined with the several lightsource matrices
    mat4 modelViewMatrix; //needed in a non-layered context for calculation of view-space values for lighting calculations
    mat4 modelViewProjectionMatrix; //needed in a non-layered context for gl_Position calculation
    
    int uniqueInstanceID; //it is not guearnteed that for each "logic" instance, the gl_InstanceID steys the same for every draw call
                          //e.g. because of culling of "previous" instances, the own gl_InstanceID will get smaller 
    
    //no padding, because the offsets will be queried via GetActiveUniformsiv(...)
  };

  layout(shared) uniform InstanceMatrixBuffer
  {
    InstanceTransformInfo instanceTransformInfo[  {{numMaxInstancesRenderable}} ];
  };
{% else  %}
  uniform mat4 modelMatrix;
  uniform mat4 modelViewMatrix;
  uniform mat4 modelViewProjectionMatrix; 

  //uniform mat4 normalMatrix;
{% endif %}


  uniform vec2 tangensCamFov = vec2 ( {{tangensCamFovHorizontal}}, {{tangensCamFovVertical}}   ); //for position calculation from pure linear depth value;
	uniform vec2 cotangensCamFov= vec2 ( {{cotangensCamFovHorizontal}}, {{cotangensCamFovVertical}}   ); //for texcoord calculation from viewspace position value;/inverso of tanget, pass from outside to save calculations
	uniform float cameraFarClipPlane = {{ cameraFarClipPlane }};
	uniform float invCameraFarClipPlane = {{ invCameraFarClipPlane }};


{% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
          uniform mat4 worldToShadowMapMatrix;  //bias*perspLight*viewLight * (camView)⁻1  for light calcs in view space
    {% if RENDER_TARGET_TEXTURE_TYPE_2D_CUBE or RENDER_TARGET_TEXTURE_TYPE_2D_CUBE_DEPTH %}  
          //haxx: due to the six view matrices of the cube map cams, we are forced to do the shadow map coord shortcut calculation from world space                                         
          uniform mat4  worldToShadowMapMatrixFROMWORLDSPACE;  //bias*perspLight*viewLight                for light calcs in world space
    {% endif %}
{% endif %}



//----------------------------------------------------------------------------------------------------
//shader specific input:
//{%shaderSpecificInput}
//{
layout(location = {{ POSITION_SEMANTICS }}    ) 	in vec4 inVPosition;
layout(location = {{ NORMAL_SEMANTICS }}      ) 	in vec4 inVNormal;
{% if SHADER_FEATURE_NORMAL_MAPPING %}
layout(location = {{ TANGENT_SEMANTICS }}     ) 	in vec4 inVTangent;
{% endif %}
{% if SHADING_FEATURE_DECAL_TEXTURING or SHADING_FEATURE_DETAIL_TEXTURING %}
layout(location = {{ TEXCOORD_SEMANTICS }}    ) 	in vec4 inVTexCoord;
{% endif %}

//future use when doing fluid stuff
{% if VISUAL_MATERIAL_TYPE_LIQUID_RENDERING or VISUAL_MATERIAL_TYPE_GAS_RENDERING %}
layout(location = {{ VELOCITY_SEMANTICS }}    ) 	in vec4 inVVelocity;
layout(location = {{ MASS_SEMANTICS }}        ) 	in vec4 inVMass;
layout(location = {{ DENSITY_SEMANTICS }}     ) 	in vec4 inVDensity;
layout(location = {{ PRESSURE_SEMANTICS }}    ) 	in vec4 invPressure;
layout(location = {{ FORCE_SEMANTICS }}       ) 	in vec4 inVForce;
layout(location = {{ Z_INDEX_SEMANTICS }}     ) 	in vec4 inVZIndex;
layout(location = {{ DECAL_COLOR_SEMANTICS }} )   in vec4 inVDecalColor;
layout(location = {{ CUSTOM_SEMANTICS }}      ) 	in vec4 inVCustomAttribute;
{% endif %}
//}
//----------------------------------------------------------------------------------------------------


//---- shader output -------------------
out InterfaceData
{
  {% include  "09_Generic_InterfaceData.glsl" %}
} output;
  
//--------------------------------------



void main()
{
  {% if SHADER_FEATURE_INSTANCING %}
    //grab the relevant matrices from the buffer
    mat4 modelMatrix =                 instanceTransformInfo[gl_InstanceID].modelMatrix;
    mat4 modelViewMatrix=              instanceTransformInfo[gl_InstanceID].modelViewMatrix;
    mat4 modelViewProjectionMatrix =   instanceTransformInfo[gl_InstanceID].modelViewProjectionMatrix;
    //mat4 normalMatrix =                normalMatrices[gl_InstanceID];
    
    int uniqueInstanceID =             instanceTransformInfo[gl_InstanceID].uniqueInstanceID;
  {% endif %}	
	
	
	{%comment%} ################################# following "coloring" output ################################################################### {%endcomment%}
  {% if RENDERING_TECHNIQUE_DEFAULT_LIGHTING or RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING  or RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL %}
  
      {% if RENDER_TARGET_TEXTURE_TYPE_2D_CUBE or RENDER_TARGET_TEXTURE_TYPE_2D_CUBE_DEPTH %}
            
          //WORLD space transform, as view/viewproj transform is done for every layer in the geom shader
            gl_Position =  modelMatrix * inVPosition; 
            output.position = gl_Position 	; //don't know if necessary; TODO check for optimization when stable
           
            output.normal = 		modelMatrix * inVNormal;    
            {% if SHADING_FEATURE_NORMAL_MAPPING %}
              output.tangent = 	modelMatrix * inVTangent;
            {% endif %}   
            
            {% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
              {%comment%} later TODO test this optimaziation for one shadowmap; this would save one multiplication 
                     of the fragment world position with the biased sm-MVP matrix in the fragment shader         {%endcomment%}
                output.shadowCoord = worldToShadowMapMatrixFROMWORLDSPACE *  output.position;
            {% endif %}         
             
      {% else %}  
      
          //default view space transform
            gl_Position =  modelViewProjectionMatrix  * inVPosition; //default MVP transform;
            output.position =  	modelViewMatrix * inVPosition;
            
            //myAss = transpose(inverse( modelViewMatrix)) * inVNormal;   
            output.normal = 		transpose(inverse( modelViewMatrix)) * inVNormal;  
            
              
            {% if SHADING_FEATURE_NORMAL_MAPPING %}
              output.tangent = 	modelViewMatrix * inVTangent;
            {% endif %} 

            {% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
              {%comment%} later TODO test this optimaziation for one shadowmap; this would save one multiplication 
                     of the fragment world position with the biased sm-MVP matrix in the fragment shader         {%endcomment%}
                output.shadowCoord = worldToShadowMapMatrix *  output.position;
            {% endif %}


      {%endif%}
      
      {% if SHADING_FEATURE_DECAL_TEXTURING or SHADING_FEATURE_DETAIL_TEXTURING	%}
         output.texCoords = inVTexCoord;
      {%endif%}    
      
      {% if instancedRendering %}
        output.uniqueInstanceID= uniqueInstanceID; 
      {% endif %}

    
  {% endif %}  {%comment%} end of "coloring" inputs {%endcomment%}
	
  {%comment%} ################################# following shadow/pos/depth inputs ############################################################## {%endcomment%}
  {% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION or RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION or RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION  %}
   
    {% if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION  %}
       
       {% if LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINT_LIGHT or LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOT_LIGHTS %}
       
          //WORLD space transform, as view/viewproj transform is done for every layer in the geom shader
            output. positionViewSpaceUNSCALED = modelMatrix * inVPosition; //TODO is obsolete in vertex shader! remove when stable
            gl_Position = modelMatrix * inVPosition;
       
       {% else %} {% comment %} can only be LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT {% endcomment  %}
  
          //default MVP transform;  
            gl_Position = modelViewProjectionMatrix * inVPosition;
        
       {% endif %}
   
    {% else %}{% if RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION %}
    
          //VIEW space transform
            //output.depthViewSpaceNORMALIZED = vec4(modelViewMatrix * inVPosition).z * invCameraFarClipPlane;
            //output.depthViewSpaceUNSCALED = vec4(modelViewMatrix * inVPosition).z;
            //output.positionViewSpaceNORMALIZED = (modelViewMatrix * inVPosition) * invCameraFarClipPlane;
            output.positionViewSpaceUNSCALED = modelViewMatrix * inVPosition; //TODO check the optimized data pass variants when stable;
            gl_Position = modelViewProjectionMatrix * inVPosition;
                
    {% else %}{% if RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION %}
       //default view space transform, same as for the default shading case ;(
            output.position =  	modelViewMatrix * inVPosition;
            gl_Position =       modelViewProjectionMatrix * output.position; //default MVP transform;
    {% endif %}{% endif %}{% endif %}
   
  {% endif %}
  
  {%comment%} ################################# following ID inputs ##############################################################{%endcomment%}
  {% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION %}
    output.genericIndices= ivec4(0,0,gl_InstanceID,23); //some funny value in w to check if it is passed anything
  {% endif %}

}



{%comment%} //-----------------------------------------------------------------
    //lecacy instancing uniform buffer content stuff; has been redesigned, see above; TODO delete this when instancing works
  
	  mat4 modelMatrices [ {{numMaxInstancesRenderable}} ];     //needed for layered rendering to be combined with the several lightsource matrices
    mat4 modelViewMatrices [ {{numMaxInstancesRenderable}} ]; //in a non-layered context for calculation of view-space values for lighting calculations
    mat4 modelViewProjectionMatrices[ {{numMaxInstancesRenderable}} ]; //in a non-layered context for gl_Position calculation
    
    int uniqueInstanceID;
    
    //following alignemt stuff, because 
    float pad1;
    float pad2;
    float pad3;
    
  
  	//mat4 normalMatrices[ {{numMaxInstancesRenderable}} ];	    //only precomputable without layered rendering,i.e. less than one view matrix

{%endcomment%}
