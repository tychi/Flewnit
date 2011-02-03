//GLSL Shader Template: persistent defines:

//applicable to following stages: all
//includeable by application code:yes

//{%persistentdefines}
//{

	#define	POSITION_SEMANTICS 	0
	#define	NORMAL_SEMANTICS	1
	#define	TANGENT_SEMANTICS	2
	#define	TEXCOORD_SEMANTICS	3
	//for debug draw and water rendering semantics
	#define VELOCITY_SEMANTICS	4
	#define MASS_SEMANTICS		5
	#define DENSITY_SEMANTICS	6
	#define PRESSURE_SEMANTICS	7
	#define FORCE_SEMANTICS		8
	#define Z_INDEX_SEMANTICS	9

	//the texture-only semantics don't play a role here, as certain textures are bound and identified by a name, not via an array

	//indicators for e.g. a geometry shader to delegate layered rendering 
	//or a fragment shader to know if to wirt to color or to gl_FragDepth;
	
	#define TEXTURE_TYPE_1D						0
	#define TEXTURE_TYPE_1D_ARRAY				1
	#define TEXTURE_TYPE_2D						2
	#define TEXTURE_TYPE_2D_RECT				3
	#define TEXTURE_TYPE_2D_CUBE				4
	#define TEXTURE_TYPE_2D_ARRAY				5
	#define TEXTURE_TYPE_2D_MULTISAMPLE			6
	#define TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE 	7
	
	#define TEXTURE_TYPE_2D_DEPTH				8
	#define TEXTURE_TYPE_2D_RECT_DEPTH			9 
	#define TEXTURE_TYPE_2D_CUBE_DEPTH			10
	#define TEXTURE_TYPE_2D_ARRAY_DEPTH			11
	//#define TEXTURE_TYPE_2D_MULTISAMPLE_DEPTH 		12 //not supported yet
	//#define TEXTURE_TYPE_2D_ARRAY_MULTISAMPLE_DEPTH 	13 //not supported yet
	
	#define TEXTURE_TYPE_3D						14


	//-----rendering technique stuff---------------------------

	/*
	 	render depth values to a depth texture, attached to the framebuffer's
	 	depth attachment:
		- if ShaderFeatures.lightSourcesShadowFeature==
					LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT:
			just a vertex shader, rendering implicitly only the "1/z" gl_FragDepth value
			to 2d-depth texture, no color stuff

		- if ShaderFeatures.lightSourcesShadowFeature==
					LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT:
			vertex + geometry + fragment shader, rendering the z-value in Camera Coordinates
			into a cubic depth texture by writing explicitly to gl_FragDepth;
			The geometry shader generates a primitive for every cubemap face;
		if ShaderFeatures.lightSourcesShadowFeature==
					LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS:
			vertex +geometry shader, rendering implicitly only the "1/z" gl_FragDepth value
			to a 2d-depth texture array, no color stuff.
			The geometry shader generates a primitive for every array layer;
	*/
	#define RENDERING_TECHNIQUE_SHADOWMAP_GENERATION		0
	//same as RENDERING_TECHNIQUE_SHADOWMAP_GENERATION, except we don't render
	//just the camera space z-value to gl_FragDepth, but the whole
	//camera space vec4 to a color texture
	#define RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION 	1
	//setup: same as RENDERING_TECHNIQUE_POSITION_IMAGE_GENERATION,
	//but as memory-footprint-optimization, we write only the z-value
	//to a one-component (GL_RED) texture; the other values will be reconstructed
	//from view frustum and frag coord when needed.
	#define RENDERING_TECHNIQUE_DEPTH_IMAGE_GENERATION		2
	#define RENDERING_TECHNIQUE_DEFAULT_LIGHTING			3
	#define RENDERING_TECHNIQUE_TRANSPARENT_OBJECT_LIGHTING	4
	#define RENDERING_TECHNIQUE_DEFERRED_GBUFFER_FILL		5
	#define RENDERING_TECHNIQUE_DEFERRED_LIGHTING			6
	//value to indicate a special shader
	#define RENDERING_TECHNIQUE_CUSTOM						7
	
	//---------------------------------------------------------
	
	
	#define LIGHT_SOURCES_LIGHTING_FEATURE_NONE						0
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ONE_SPOT_LIGHT			1
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ONE_POINT_LIGHT			2
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_LIGHTS			3
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_SPOT_LIGHTS			4
	#define LIGHT_SOURCES_LIGHTING_FEATURE_ALL_POINT_OR_SPOT_LIGHTS	5
	
	
	#define LIGHT_SOURCES_SHADOW_FEATURE_NONE			0
	#define LIGHT_SOURCES_SHADOW_FEATURE_ONE_SPOTLIGHT	1
	#define LIGHT_SOURCES_SHADOW_FEATURE_ONE_POINTLIGHT	2
	#define LIGHT_SOURCES_SHADOW_FEATURE_ALL_SPOTLIGHTS	3

	#define SHADOW_TECHNIQUE_NONE		0
	#define SHADOW_TECHNIQUE_DEFAULT	1
	#define SHADOW_TECHNIQUE_PCFSS		2
	
	#define SHADING_FEATURE_NONE				0
	#define SHADING_FEATURE_DIRECT_LIGHTING		1<<0
	//global lighting via layered depth images or stuff... just a brainstroming, won't be implemented	
	#define SHADING_FEATURE_GLOBAL_LIGHTING		1<<1
	#define SHADING_FEATURE_DECAL_TEXTURING		1<<2
	#define SHADING_FEATURE_NORMAL_MAPPING		1<<3
	#define SHADING_FEATURE_CUBE_MAPPING		1<<4
	#define SHADING_FEATURE_AMBIENT_OCCLUSION	1<<5
	#define SHADING_FEATURE_TESSELATION			1<<6


//}
