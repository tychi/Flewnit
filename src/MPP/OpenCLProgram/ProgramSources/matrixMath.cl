    
    
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
	    
	    //init also the rest to be sure... //maybe todo remove when stable
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
      
    }
    
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
