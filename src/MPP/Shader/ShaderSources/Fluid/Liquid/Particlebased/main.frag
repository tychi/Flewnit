
{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}
//data type definition

//---- interface ---------------------------------------------------------------------------------
//---- application (uniform) input ----

  uniform mat4 inverseModelViewMatrix; //needed for cube map lookup in world space
  
  uniform vec2 viewPortSizes;
  uniform vec2 focalLengths;


  //Have a look at constructor of ParticleLiquidVisualMaterial for explanation of those values;
	uniform float curvatureDisplacementFactorPerStep;
	uniform float silhouetteThreshold;
	
	//uniform float fluidTextureScaleFactor;
	uniform vec2 sceneTextureSampleInterval; // vec2(1,1)/ textureSize(backGroundSceneTexture); precomputed for performance;
  uniform vec2 fluidTexturesSampleInterval; // vec2(1,1)/ textureSize(depthTexture); precomputed for performance;
	
	
	uniform float particleDrawRadius;
	uniform vec4 liquidColor;			
	uniform float thicknessAttenuationFactor;
	uniform float refractionStrengthBias;
	uniform float foamGenerationAccelerationThreshold;
	uniform vec4 foamColor;
	uniform float reflectivity;
	uniform float shininess;
	
	//value for Schlick's Fresnel approximation:
	uniform float r0;
	//we don't need any further values like the transmitted angle resp. vector, as we have to fake via texture lookup anyway ;(

  	

  // - - - - - - - - samplers to come

  //for the beginning to keep it simple, lets just assume that the background scene rendering is a 2D texture
  uniform sampler2D backGroundSceneTexture;
  
  uniform samplerCube cubeMap;
  
  
  {% if sophisticatedRendering or thicknessAndNoiseGeneration or curvatureFlowComputation %}
    uniform sampler2D depthTexture;
  {% endif %}
  
  
  {% if thicknessAndNoiseGeneration or directRendering %}
      //texture to sample from for generation of noiseRenderedTexture;
      uniform sampler2D noiseSourceTexture;
  {% endif %}
  
  {% if sophisticatedRendering %}
    uniform sampler2D thicknessTexture;
    //the texture where the accumulated noise values are encoded in
    uniform sampler2D noiseRenderedTexture;
  {% endif %}
  


  // - - - - - - - - TODO light sources, shadow maps etc. to come
  



//---- shader input --------------------

  {% if sophisticatedRendering or curvatureFlowComputation %}
    //just fullscreenquad, everything else is read from textures and uniforms
    
    in InterfaceData
    {
      vec4 texCoords;
    }input;
    
  {% else %}
    
    in InterfaceData
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
     } input;
     
  {% endif %}

//---- shader output -------------------

{% if directRendering or sophisticatedRendering %}
layout(location = 0 ) out vec4 outFFinalLuminance;
{% endif %}

{% if depthAndAccelGeneration  %}
layout(location = 0 ) out float outFDepthViewSpace;
layout(location = 1 ) out float outFAcceleration;
{% endif %}

{% if curvatureFlowComputation %}
layout(location = 0 ) out float outFSmoothedDepth;
{% endif %}

{% if thicknessAndNoiseGeneration %}
layout(location = 0 ) out float outFThickness;
layout(location = 1 ) out float outFNoise;
{% endif %}


//----- subroutines ------------------------------------------------------------------------------

{% if thicknessAndNoiseGeneration %}
bool fragmentIsOutOfCircle()liquidColor
{
  vec2 n = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
  return ( dot(n,n) > 1.0);
}
{% endif %}


{% if sophisticatedRendering or curvatureFlowComputation %}
  vec3 calculateNormal()
  {

      //compute normal from finite differences of depth texture after [Green08]
      ...

  }
{% endif %}


{% if thicknessAndNoiseGeneration %}
  float calculateThickness()
  {
    ...
  }

  float calculateNoise()
  {
    ...
  }
{% endif %}

//==================================================================================================================

void main()
{

{% if sophisticatedRendering or curvatureFlowComputation %}
  //check if fragment is covered by fluid, else pass background and return resp- discard;
  float depth = texture(depthTexture, input.texCoords.xy);
  if(depth <= 0.0)
  {
    {% if sophisticatedRendering %}
      outFFinalLuminance = texture(backGroundSceneTexture, input.texCoords.xy);
      return;
    {% endif %}
    {% if curvatureFlowComputation %}
      discard;
    {% endif %}
  }
{% endif %}



//=============== normal, fragment position computation and thickness determination ==========

{% if directRendering or depthAndAccelGeneration  %}
  //compute normal from point coord 
  vec3 normalViewSpace;
  normalViewSpace = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
  float magnitude = dot(normalViewSpace.xy, normalViewSpace.xy);
  // kill pixels outside circle
  if (magnitude > 1.0) 
  { 
    discard;  
  }
  normalViewSpace.z = sqrt(1.0-magnitude);
  //now we have a normalized view space normal
  
  vec3 fragPositionViewSpace = input.position.xyz + normalize(normalViewSpace) * particleDrawRadius ;
  
  //hardcode for direct rendering ;(
  float thickness = 0.5;
  
{% endif %}

{% if thicknessAndNoiseGeneration %}
  //no normal calculation and hence no "automatic" discard:
  if(fragmentIsOutOfCircle())
  {
    discard;
  }
{% endif %}


{% if sophisticatedRendering or curvatureFlowComputation %}

  //careful: is normalized for direct rendering methods, but NOT for deferred ones!
  //hence this value is non-normalized!
  vec3 normalViewSpace = calculateNormal();
  
  //normalize:
  normalViewSpace= normalize(normalViewSpace);
  
  //calculate view space fragment position from depth texture
  vec3 fragPositionViewSpace = ...
  
  
  float thickness = texture(thicknessTexture, input.texCoords.xy);

{% endif %}

//============== shading stuff =========================



{% if directRendering or sophisticatedRendering %}
  
  //fresnel term calculation: - - - - - - - - - - -
  //{
  vec3 reflectedCamToFragViewSpaceNormalized = reflect ( normalize(fragPositionViewSpace.xyz), normalViewSpace.xyz);
  
  //Schlick's approximation:
  float reflectivity = r0 + (1.0-r0) * pow( dot(reflectedCamToFragViewSpaceNormalized.xyz, normalViewSpace.xyz), 5.0);
  
  float refractivity = 1.0 - reflectivity;
  //}
  
  
  vec4 reflectedColor =  texture(
    cubeMap, 
    //transform from viewspace back to worldspace via inverse rotational part of the view matrix:
    //note : the correct lookup would be with positive direction reflected; but due to strange layout
    //of loadable cube map textures (	{"_RT", "_LF", positive y= down, negative y=up, "_FR", "_BK"}),
    //we have to lookup in the contrary direction,
    //fragment position is in view space, hence corresponds to unnormalized direction :)
    //(mat3(inverseModelViewMatrix)) * reflect( - position.xyz , normalViewSpace.xyz )
    (mat3(inverseModelViewMatrix)) * (-reflectedCamToFragViewSpaceNormalized)
  );
{% endif %} 

{% if directRendering  %}
  //no thickness information, "real" refraction (i suggest) is too costly, will try later, so just half of both:
  vec4 refractedColor = 
    0.5 * liquidColor +
    0.5 * texture(backGroundSceneTexture, input.texCoords +  normalViewSpace.xy * thickness * refractionStrengthBias),                
{% endif %}                 

{% if sophisticatedRendering %}
  vec4 refractedColor = mix(
    liquidColor,
    texture(backGroundSceneTexture, input.texCoords +  refractionStrengthBias * normalViewSpace.xy ),
    exp( -thicknessAttenuationFactor * thickness )
  );
{% endif %}

{% if directRendering or sophisticatedRendering %}


 
  outFFinalLuminance.rgb= 
      reflectivity * reflectedColor.xyz  
    + refractivity * refractedColor-xyz
    ;

    //TODO add specular hightlight stuff


  //HAXX TEST: first, just pass the normal for a test:
  outFFinalLuminance.rgb= normalViewSpace;
  outFFinalLuminance.a= 1.0;


{% endif %}

//========== non-shading stuff =========================

{% if depthAndAccelGeneration  %}
  outFDepthViewSpace = input.position.z + normalViewSpace.z * particleDrawRadius ;
  outFAcceleration= input.acceleration;
{% endif %}

{% if thicknessAndNoiseGeneration %}
  outFNoise = calculateNoise();
  outFThickness = calculateThickness();
{% endif %}

{% if curvatureFlowComputation %}
  //variable names oriented to [Green08]
  
  float D = normalViewSpace.x * normalViewSpace.x + normalViewSpace.y * normalViewSpace.y + normalViewSpace.z * normalViewSpace.z;
  //normalize:
  normalViewSpace = normalViewSpace / sqrt(D);
  
  ...

  outFSmoothedDepth = ...;
{% endif %}

  

} //end main
