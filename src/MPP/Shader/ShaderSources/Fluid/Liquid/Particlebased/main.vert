{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}


  //precomputed value for efficient point size calculation
  //gl_PointSize = particleDrawRadius* max(viewPortWith,viewPortHeight) / (2* tan(FoVAngle)) / length(positionViewSpace)
  //everything can be precomputed but length(positionViewSpace):
  //gl_PointSize = particlePointSizePrecomputedFactor / length(positionViewSpace)
  uniform float particlePointSizePrecomputedFactor;

  uniform mat4 modelViewMatrix;
  uniform mat4 modelViewProjectionMatrix;

//----------------------------------------------------------------------------------------------------
//shader specific input:

layout(location = {{ POSITION_SEMANTICS }}    ) 	in vec4 inVPosition;
//if we use predicted or corrected velicity, plays virtually no role; afaik unneeded anyway
//layout(location = {{ VELOCITY_SEMANTICS }}    ) 	in vec4 inVVelocity;
layout(location = {{ DENSITY_SEMANTICS }}     ) 	in vec4 inVDensity;
layout(location = {{ ACCELERATION_SEMANTICS }}       ) 	in vec4 inVAcceleration;
//maybe once needed for visualization.. but as it works anyway... ;(
//layout(location = {{ Z_INDEX_SEMANTICS }}     ) 	in uint inVZIndex;
//needed for consistent noise texture sampling for a certain pattern to be advected with the particle
layout(location = {{ PRIMITIVE_ID_SEMANTICS }}     ) 	in uint inVObjectInfo;


//----------------------------------------------------------------------------------------------------


//---- shader output -------------------
  {% if sophisticatedRendering or curvatureFlowComputation %}
    //just fullscreenquad, everything else is read from textures and uniforms
    
    out InterfaceData
    {
      vec4 texCoords;
    }output;
    
  {% else %}
    
    out InterfaceData
    {
      vec4 position;
      //vec4 velocity; afaik unneeded
      //float density; only needed later for spray- or inside-volume-culling (via discard in fragment shader or via geom shader.. we'll see)
        
      {% if directRendering or depthAndAccelGeneration %}
        float acceleration;
      {% endif %}
      
      {% if directRendering or thicknessAndNoiseGeneration %}
        uint objectInfo;
      {% endif %} 
     } output;
     
  {% endif %}
//--------------------------------------



void main()
{

  gl_Position =  modelViewProjectionMatrix  * inVPosition;
  
  
  {% if sophisticatedRendering or curvatureFlowComputation %}
  
    output.texCoords = inVTexCoord;
    
  {% else %}
  
        output.position = modelViewMatrix  * inVPosition;
        gl_PointSize = particlePointSizePrecomputedFactor / length( output.position);
        //gl_PointSize = 500.0 / length( output.position);
       
        //no velocity or density passing yet
  /*      
        {% if directRendering or depthAndAccelGeneration %}
          output.acceleration= length(inVAcceleration);
        {% endif %}
        
        {% if directRendering or thicknessAndNoiseGeneration %}
          output.objectInfo= inVObjectInfo;
        {% endif %} 
    */    
  {% endif %}
}


