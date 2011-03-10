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


{% if not layeredRendering %}
   //seemingly unneded in this stage:
   // uniform mat4 viewMatrix;
   // uniform mat4 projectionMatrix;
   // uniform mat4 viewProjectionMatrix;
{% endif %}

{%if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION and SHADING_FEATURE_TESSELATION %} 
    //as every user varyings are in world space, we have to write view space pos OF THE SPECTOTOR CAM to gl_Position
    //in case both tess and layered rendering is active, so that the tess control shader can perform its 
    //view space dynamic LOD calculations; Even for shadow map generation, the tesslevels should be performed in cam space
    //in order to omit artifacts due to different generation and comparison-geometry; 
     uniform mat4 spectatorCamViewMatrix;
{% endif %} 

//a bit overkill of matrix permutations, but removing unnecessary ones later is easyier than adding missing ones; those matrices makeing no sense in a certein rendering environment will be ignored;
{% if instancedRendering %}

  struct InstanceTransform
  {
    mat4 modelMatrix;  //needed for layered rendering to be combined with the several lightsource matrices
    mat4 modelViewMatrix; //needed in a non-layered context for calculation of view-space values for lighting calculations
    mat4 modelViewProjectionMatrix; //needed in a non-layered context for gl_Position calculation
    
    int uniqueInstanceID; //it is not guearnteed that for each "logic" instance, the gl_InstanceID steys the same for every draw call
                          //e.g. because of culling of "previous" instances, the own gl_InstanceID will get smaller 
    
    //no padding, because the offsets will be queried via GetActiveUniformsiv(...)
  };

  layout(shared) uniform InstanceTransformBuffer
  {
    InstanceTransform instanceTransforms[  {{numMaxInstancesRenderable}} ];
  };
{% else  %}
  uniform mat4 modelMatrix;
  uniform mat4 modelViewMatrix;
  uniform mat4 modelViewProjectionMatrix; 
{% endif %}

  uniform vec3 eyePositionW;
  uniform vec2 tangensCamFov = vec2 ( {{tangensCamFovHorizontal}}, {{tangensCamFovVertical}}   ); //for position calculation from pure linear depth value;
	uniform vec2 cotangensCamFov= vec2 ( {{cotangensCamFovHorizontal}}, {{cotangensCamFovVertical}}   ); //for texcoord calculation from viewspace position value;/inverso of tanget, pass from outside to save calculations
	uniform float cameraFarClipPlane = {{ cameraFarClipPlane }};
	uniform float invCameraFarClipPlane = {{ invCameraFarClipPlane }};


{% if SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
  uniform mat4 shadowMapLookupMatrix;  //bias*perspLight*viewLight * (camView)⁻1  for light calcs in view space
{% endif %}



//----------------------------------------------------------------------------------------------------
//shader specific input:
//{%shaderSpecificInput}
//{
layout(location = {{ POSITION_SEMANTICS }}    ) 	in vec4 inVPosition;
layout(location = {{ NORMAL_SEMANTICS }}      ) 	in vec4 inVNormal;
{% if SHADING_FEATURE_NORMAL_MAPPING %}
layout(location = {{ TANGENT_SEMANTICS }}     ) 	in vec4 inVTangent;
{% endif %}
{% if texCoordsNeeded %}
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
layout(location = {{ DIFFUSE_COLOR_SEMANTICS }} )   in vec4 inVDecalColor;
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
  //-------------------- grab the appropriate transformation matrices ------------------------------

  {% if instancedRendering %}
    //grab the relevant matrices from the buffer
    {%if worldSpaceTransform %}
      mat4 shadeSpaceTransform = instanceTransforms[gl_InstanceID].modelMatrix;
    {% else %}
      mat4 shadeSpaceTransform = instanceTransforms[gl_InstanceID].modelViewMatrix;
    {% endif %}
    mat4 modelViewProjectionMatrix =    instanceTransforms[gl_InstanceID].modelViewProjectionMatrix;   
    mat4 modelViewMatrix =              instanceTransforms[gl_InstanceID].modelViewMatrix;   
    int uniqueInstanceID =              instanceTransforms[gl_InstanceID].uniqueInstanceID;
  {% else %}
    {%if worldSpaceTransform %}
      mat4 shadeSpaceTransform = modelMatrix;
    {% else %}
      mat4 shadeSpaceTransform = modelViewMatrix;
    {% endif %}
  {% endif %}	
  //----------------------------------------------------------------------------------------------
  
  
  {%if not layeredRendering and not SHADING_FEATURE_TESSELATION %} 
    //we need projected stuff for rasterization because no tessCtrl/TessEval/geom shader follows,
    gl_Position =  modelViewProjectionMatrix  * inVPosition; /*default MVP transform*/   
  {% endif %} 
  
   
	{% if shadeSpacePositionNeeded %}
		{% if VISUAL_MATERIAL_TYPE_SKYDOME_RENDERING %}
      //special case: neither world nor view space transform: use the vertex data directly as world spce direction vectors
      output.position = inVPosition;
    {% else %}
      output.position =  shadeSpaceTransform * inVPosition; 
    {% endif %}       
  {% endif %}  
	
	 {%if worldSpaceTransform and SHADING_FEATURE_TESSELATION %} 
    //as every user varyings are in world space, we have to write view space pos OF THE SPECTOTOR CAM to gl_Position
    //in case both tess and layered rendering is active, so that the tess control shader can perform its 
    //view space dynamic LOD calculations; Even for shadow map generation, the tesslevels should be performed in cam space
    //in order to omit artifacts due to different generation and comparison-geometry; 
      {%if RENDERING_TECHNIQUE_SHADOWMAP_GENERATION %} 
        gl_Position =  spectatorCamViewMatrix * output.position;
      {% else %} 
        gl_Position =  modelViewMatrix * inVPosition;
      {% endif %} 
  {% endif %} 
	
	
  {% if SHADING_FEATURE_TESSELATION or shaderPerformsColorCalculations %}              
      output.normal = 		shadeSpaceTransform * inVNormal; //output.normal =     transpose(inverse( modelViewMatrix)) * inVNormal;           
      {% if SHADING_FEATURE_NORMAL_MAPPING %}
        output.tangent = 	shadeSpaceTransform * inVTangent;
      {% endif %} 
      {% if texCoordsNeeded %}
         output.texCoords = inVTexCoord;
      {%endif%}   
  {% endif %}  {%comment%} end of "tess and/or color" inputs {%endcomment%}   
    
      
  {% if not SHADING_FEATURE_TESSELATION and SHADOW_FEATURE_EXPERIMENTAL_SHADOWCOORD_CALC_IN_FRAGMENT_SHADER and LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOT_LIGHT %}
    {%comment%} later TODO test this optimaziation for one shadowmap; this would save one multiplication 
                of the fragment world position with the biased sm-MVP matrix in the fragment shader         {%endcomment%}
    output.shadowCoord = shadowMapLookupMatrix *  output.position;
  {% endif %}
  {% if depthButNotSpotLight and not shadeSpacePositionNeeded %}
     //write out optimized viewspace value for depth image if we have not to write out the whole position for tess and/or geom shader:
     //output.depthViewSpaceNORMALIZED = vec4(shadeSpaceTransform * inVPosition).z * invCameraFarClipPlane;
     //output.depthViewSpaceUNSCALED = vec4(shadeSpaceTransform * inVPosition).z;
     //output.positionViewSpaceNORMALIZED = (shadeSpaceTransform * inVPosition) * invCameraFarClipPlane;
     output.positionViewSpaceUNSCALED = shadeSpaceTransform * inVPosition; //TODO check the optimized data pass variants when stable;               
  {% endif %}
	
  
  {%comment%} ################################# following ID inputs ##############################################################{%endcomment%}
  {% if RENDERING_TECHNIQUE_PRIMITIVE_ID_RASTERIZATION or instancedRendering %}
    {% if instancedRendering %}
      output.genericIndices= ivec4(0,gl_InstanceID,uniqueInstanceID,23); //some funny value in w to check if it is passed anything
    {% else %}
      output.genericIndices= ivec4(0,gl_InstanceID,52,23); //some funny value in w to check if it is passed anything
    {% endif %}
  {% endif %}

}


