
{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}


  uniform mat4 modelViewProjectionMatrix; //just an orthographic projection matrix in [-1,+1,-1,+1,-1,+1]



//----------------------------------------------------------------------------------------------------
//shader specific input:

layout(location = {{ POSITION_SEMANTICS }}    ) 	in vec4 inVPosition;
layout(location = {{ TEXCOORD_SEMANTICS }}    ) 	in vec4 inVTexCoord;



//----------------------------------------------------------------------------------------------------


//---- shader output -------------------
  out InterfaceData
  {
    vec4 texCoords;
  } output;
 
//--------------------------------------



void main()
{

  gl_Position =  modelViewProjectionMatrix  * inVPosition;
  output.texCoords = inVTexCoord;
 
}


