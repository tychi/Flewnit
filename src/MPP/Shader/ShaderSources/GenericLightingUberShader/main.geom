//A first concept of a vertex shader template

//section 0: Version Tag
//{%versionTag}
//{
#version 330 core
//}

//section 1: precision:
//{%precisionTag}
//{
precision highp float;
//}

//section 2: defines:
//{%defines}
//{
//
#define	POSITION_SEMANTICS 	0
#define	NORMAL_SEMANTICS	1
#define	TANGENT_SEMANTICS	2
#define	BINORMAL_SEMANTICS	3
#define	TEXCOORD_SEMANTICS	4

#define VELOCITY_SEMANTICS	5
#define MASS_SEMANTICS		6
#define DENSITY_SEMANTICS	7
#define PRESSURE_SEMANTICS	8
#define FORCE_SEMANTICS		9
#define Z_INDEX_SEMANTICS	10

//}



//section 3: shader specific input:
//{%geomShaderInput}
//{
layout(triangles) in;
//}

//section 
//{%}
//{

//}
