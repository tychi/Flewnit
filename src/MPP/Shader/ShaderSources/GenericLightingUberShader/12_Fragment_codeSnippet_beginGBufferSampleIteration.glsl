 {%comment%}
  GLSL Shader Template: Fragment shader code snippet: begin the iteration over every sample in a deferred lighting context;
                                                      the read-out sample varibles must have the same name as the "in" variables in the non deferred context,
                                                      in order to assure code compatibility
  applicable to following stages: fragment     {%endcomment%} 
 
        float totalValidMultiSamples=0.0;
        for(int sampleIndex = 0; sampleIndex < NUM_MULTISAMPLES ;sampleIndex++); //iterate over the values in the MS G-Buffer
        {
              //acquire G-buffer values in order to omit multiple reads to same texture and to use the same variable names in the following code;		          
		          vec3 position = 	GBUFFER_ACQUIRE(positionTexture, sampleIndex).xyz;
		          if(position.z >= cameraFarClipPlane - 0.001) //subtract epsilon
		          {
			          //I wanna keep the option to compress normals to 16 bit normalized unsigned int textures; hence, a coding which allows detection
			          //of invalid samples is lost; so, i detext those samples via its depth value:
			          continue;
		          }
		          else { totalValidMultiSamples += 1.0; }
		          
 		          vec3 normalVN		=	getNormal(sampleIndex);
		          vec4 fragmentColor =		GBUFFER_ACQUIRE(colorTexture, sampleIndex).xyzw;
              float shininess = fragmentColor.w;
              //fragmentColor.w = 1.0; //reset alpha to omit some fuckup <-- forgot why i wanted to do this
              
