
{% include  "00_Generic_Common_VersionTag.glsl" %}
{% include  "01_Generic_Common_precisionTag.glsl" %}
//data type definition

//---- interface ---------------------------------------------------------------------------------
//---- application (uniform) input ----
{% if TEXTURE_SHOW_TYPE_TEXTURE_TYPE_2D%}
  uniform sampler2D textureToShow;
{% endif %}
{% if TEXTURE_SHOW_TYPE_TEXTURE_TYPE_2D_RECT%}
  uniform sampler2DRect textureToShow;
{% endif %}
{%comment%} and so on... continue list if needed;{% endcomment %}


//layerToShow relevant for array or 3D- textures: provide layer resp texture coord to show a specific slice;
uniform float layerOrTexcoord = 0.0f;

//---- shader input --------------------



in InterfaceData
{
  vec4 texCoords;
}input;

//---- shader output -------------------
out vec4 outFFinalLuminance;
//----- subroutines ------------------------------------------------------------------------------



void main()
{

  {% if TEXTURE_SHOW_TYPE_TEXTURE_TYPE_2D%}
    outFFinalLuminance=texture(textureToShow,input.texCoords.xy);
   
    outFFinalLuminance.b= 0.8 * input.texCoords.x ;  //test for success TODO remove this line 
  {% endif %}
  
  {% if TEXTURE_SHOW_TYPE_TEXTURE_TYPE_2D_RECT%}
    //scale by texture size; gl_FragCoord only useful if the is no scaling.. //TODO make this shader more customizable: interpolation, scaling etc.
    outFFinalLuminance=texture(textureToShow,input.texCoords.xy * vec2(textureSize(textureToShow,0)) );
  {% endif %}
  
  {%comment%} and so on... continue list if needed;{% endcomment %}
  

} //end main
