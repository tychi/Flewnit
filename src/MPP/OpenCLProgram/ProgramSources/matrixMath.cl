   
  
#ifndef FLEWNIT_CL_PROGRAMS_MATRIX_MATH_GUARD
#define FLEWNIT_CL_PROGRAMS_MATRIX_MATH_GUARD
   
   /*
     matrixMath.cl
   
     Like in OpenGL, we assume column major matrices, i.e. matrixAsFloat16.lo.lo is the first column;
     
   */
   
    
    //grab the fourth column of a matrix coded as float16
    #define TRANSLATION_VEC_OF_MATRIX( float16Matrix ) ( ( float16Matrix ).hi.hi )
    //do nothing, just sure to have thought about it and be ready for future implememtation changes:
    #define ROTATIONAL_PART( float16Matrix ) (  float16Matrix )
    
    //float16Matrix.even selects row 0 and 2, float16Matrix.even.even selects row 0
    /*
    Column selection  ||                       |                      |                      |                      |
                -->   ||  .lo.lo               | .lo.hi               | .hi.lo               | .hi.hi               |
    | Row selection   ||                       |                      |                      |                      |
    V                 ||                       |                      |                      |
     =================||==============================================================================================
                      || .lo.lo.lo.lo          | .lo.hi.lo.lo         | .hi.lo.lo.lo         | .hi.hi.lo.lo         |
      .even.even      || .even.even.even.even  | .even.even.odd.even  | .even.even.even.odd  | .even.even.odd.odd   |
                      || .s0                   | .s4                  | .s8                  | .sc                  |
     _________________||_______________________|______________________|______________________|______________________|
                      || .lo.lo.lo.hi          | .lo.hi.lo.hi         | .hi.lo.lo.hi         | .hi.hi.lo.hi         |
      .odd.even       || .odd.even.even.even   | .odd.even.odd.even   | .odd.even.even.odd   | .odd.even.odd.odd    |
                      || .s1                   | .s5                  | .s9                  | .sd                  |
     _________________||_______________________|______________________|______________________|______________________| 
                      || .lo.lo.hi.lo          | .lo.hi.hi.lo         | .hi.lo.hi.lo         | .hi.hi.hi.lo         |
      .even.odd       || .even.odd.even.even   | .even.odd.odd.even   | .even.odd.even.odd   | .even.odd.odd.odd    |
                      || .s2                   | .s6                  | .sa                  | .se                  |
     _________________||_______________________|______________________|______________________|______________________|
                      || .lo.lo.hi.hi          | .lo.hi.hi.hi         | .hi.lo.hi.hi         | .hi.hi.hi.hi         |
      .odd.odd        || .odd.odd.even.even    | .odd.odd.odd.even    | .odd.odd.even.odd    | .odd.odd.odd.odd     |
                      || .s3                   | .s7                  | .sb                  | .sf                  |
     _________________||_______________________|______________________|______________________|______________________|
    */    
    #define COL_0( mat4x4OrRowVec4 ) ( ( mat4x4OrRowVec4 ).lo.lo )
    #define COL_1( mat4x4OrRowVec4 ) ( ( mat4x4OrRowVec4 ).lo.hi )
    #define COL_2( mat4x4OrRowVec4 ) ( ( mat4x4OrRowVec4 ).hi.lo )
    #define COL_3( mat4x4OrRowVec4 ) ( ( mat4x4OrRowVec4 ).hi.hi )
    
    //not that this row selection even works on 4D-(column)vectors, not only on 4x4-Matrices!
    #define ROW_0( mat4x4OrColVec4 )  ( ( mat4x4OrColVec4 ).even.even )
    #define ROW_1( mat4x4OrColVec4 )  ( ( mat4x4OrColVec4 ).odd.even )
    #define ROW_2( mat4x4OrColVec4 )  ( ( mat4x4OrColVec4 ).even.odd )
    #define ROW_3( mat4x4OrColVec4 )  ( ( mat4x4OrColVec4 ).odd.odd )
    
    //note that you can nest the macros to fake kind of 2D-array selection!
    //first select the column (most nested), than the row;
    //mat->s0 == ROW_0(COL_0(*mat));
    //Actually, the order of nesting doesn't even play a role!
    //mat->s0 == COL_0(ROW_0(*mat));


    void constructRotationMatrix(__local float16* result, float4 rotationAxis, float rotationAngleRadians )
    {
      /*
        Code ported from glm math library by gtruc creation; file gtc/matrix_transform.inl, routine
      		
      		template <typename T> detail::tmat4x4<T> rotate(
			        detail::tmat4x4<T> const & m,
			        T const & angle, 
			        detail::tvec3<T> const & v);
      */
      
      float cosAlpha = cos(rotationAngleRadians); //TODO native_cos
      float sinAlpha = sin(rotationAngleRadians); //TODO native_sin

		  float4 tempVec = ( 1.0f - cosAlpha ) * rotationAxis;

		  result->s0 = cosAlpha + tempVec.x * rotationAxis.x;
	    result->s1 =            tempVec.x * rotationAxis.y + sinAlpha * rotationAxis.z;
	    result->s2 =            tempVec.x * rotationAxis.z - sinAlpha * rotationAxis.y;

	    result->s4 =            tempVec.y * rotationAxis.x - sinAlpha * rotationAxis.z;
	    result->s5 = cosAlpha + tempVec.y * rotationAxis.y;
	    result->s6 =            tempVec.y * rotationAxis.z + sinAlpha * rotationAxis.x;

	    result->s8 =            tempVec.z * rotationAxis.x + sinAlpha * rotationAxis.y;
	    result->s9 =            tempVec.z * rotationAxis.y - sinAlpha * rotationAxis.x;
	    result->sa = cosAlpha + tempVec.z * rotationAxis.z;
	    
	    //init also the rest to be sure... //maybe TODO remove when stable
	    result->s3=0.0f;
	    result->s7=0.0f;
	    result->sb=0.0f;
	    result->hi.hi = (float4)(0.0f,0.0f,0.0f,1.0f);
    }
    
    void matrixMult4x4( 
      __local float16* result, 
      __local float16* leftMat,
      __local float16* rightMat )
    {
      result->s0 = dot( leftMat->even.even, rightMat->lo.lo );
      result->s1 = dot( leftMat->odd.even, rightMat->lo.lo );
      result->s2 = dot( leftMat->even.odd, rightMat->lo.lo );
      result->s3 = dot( leftMat->odd.odd, rightMat->lo.lo );      
      
      result->s4 = dot( leftMat->even.even, rightMat->lo.hi );
      result->s5 = dot( leftMat->odd.even, rightMat->lo.hi );
      result->s6 = dot( leftMat->even.odd, rightMat->lo.hi );
      result->s7 = dot( leftMat->odd.odd, rightMat->lo.hi );  
      
      result->s8 = dot( leftMat->even.even, rightMat->hi.lo );
      result->s9 = dot( leftMat->odd.even, rightMat->hi.lo );
      result->sa = dot( leftMat->even.odd, rightMat->hi.lo );
      result->sb = dot( leftMat->odd.odd, rightMat->hi.lo );  
      
      result->sc = dot( leftMat->even.even, rightMat->hi.hi );
      result->sd = dot( leftMat->odd.even, rightMat->hi.hi );
      result->se = dot( leftMat->even.odd, rightMat->hi.hi );
      result->sf = dot( leftMat->odd.odd, rightMat->hi.hi );  
      
      /*
        or do you prefer the other flavour? :P
      COL_0( ROW_0( *result ) ) = dot( ROW_0( *leftMat ), COL_0( *rightMat ) );
      COL_0( ROW_1( *result ) ) = dot( ROW_1( *leftMat ), COL_0( *rightMat ) );
      COL_0( ROW_2( *result ) ) = dot( ROW_2( *leftMat ), COL_0( *rightMat ) );
      COL_0( ROW_3( *result ) ) = dot( ROW_3( *leftMat ), COL_0( *rightMat ) );
      
      COL_1( ROW_0( *result ) ) = dot( ROW_0( *leftMat ), COL_1( *rightMat ) );
      COL_1( ROW_1( *result ) ) = dot( ROW_1( *leftMat ), COL_1( *rightMat ) );
      COL_1( ROW_2( *result ) ) = dot( ROW_2( *leftMat ), COL_1( *rightMat ) );
      COL_1( ROW_3( *result ) ) = dot( ROW_3( *leftMat ), COL_1( *rightMat ) );
      
      COL_2( ROW_0( *result ) ) = dot( ROW_0( *leftMat ), COL_2( *rightMat ) );
      COL_2( ROW_1( *result ) ) = dot( ROW_1( *leftMat ), COL_2( *rightMat ) );
      COL_2( ROW_2( *result ) ) = dot( ROW_2( *leftMat ), COL_2( *rightMat ) );
      COL_2( ROW_3( *result ) ) = dot( ROW_3( *leftMat ), COL_2( *rightMat ) );
      
      COL_3( ROW_0( *result ) ) = dot( ROW_0( *leftMat ), COL_3( *rightMat ) );
      COL_3( ROW_1( *result ) ) = dot( ROW_1( *leftMat ), COL_3( *rightMat ) );
      COL_3( ROW_2( *result ) ) = dot( ROW_2( *leftMat ), COL_3( *rightMat ) );
      COL_3( ROW_3( *result ) ) = dot( ROW_3( *leftMat ), COL_3( *rightMat ) );
      */ 
      
    }
    
    //all elements but the rotational part are overwirrten so that the result matrix is in any case a valid
    //4x4 rotation matrix without any projective or translational part!
    void matrixMult3x3( 
      __local float16* result, 
      __local float16* leftMat,
      __local float16* rightMat )
    {
      result->s0 = dot( leftMat->even.even, rightMat->lo.lo );  
      result->s1 = dot( leftMat->odd.even, rightMat->lo.lo );  
      result->s2 = dot( leftMat->even.odd, rightMat->lo.lo );  
      result->s3 = 0.0f;
      
      result->s4 = dot( leftMat->even.even, rightMat->lo.hi );  
      result->s5 = dot( leftMat->odd.even, rightMat->lo.hi );  
      result->s6 = dot( leftMat->even.odd, rightMat->lo.hi );  
      result->s7 = 0.0f;
      
      result->s8 = dot( leftMat->even.even, rightMat->hi.lo );  
      result->s9 = dot( leftMat->odd.even, rightMat->hi.lo );  
      result->sa = dot( leftMat->even.odd, rightMat->hi.lo );  
      result->sb = 0.0f;
      
      result->hi.hi = (float4)(0.0f,0.0f,0.0f,1.0f);  
    }
    
     
    float4 matrixMult4x4Vec(__local float16* mat, float4 vec)
    {
      return (float4)(
        dot( mat->even.even, vec ), 
        dot( mat->odd.even , vec ), 
        dot( mat->even.odd , vec ), 
        dot( mat->odd.odd, vec )  
      );
    }
    
    float4 matrixMult3x3Vec(__local float16* mat, float4 vec)
    {
      return (float4)(
        dot( mat->even.even, vec ), 
        dot( mat->odd.even , vec ), 
        dot( mat->even.odd , vec ), 
        vec.w
      );
    }
    
#endif //FLEWNIT_CL_PROGRAMS_MATRIX_MATH_GUARD    
    
