#version 120

// Uniforms

uniform sampler2D heightMap ;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;

// Kernel
void main(void)
{
		float lowerLimit = 0.45 ;
		float buffer = 0.05 ;
		float height = texture2D( heightMap, gl_TexCoord[0].st).r ;

		float grassAlpha = 0.0 ;
		if(height < lowerLimit){
			grassAlpha = 1.0 ;
		} else {
			if( height < lowerLimit + buffer){
				grassAlpha = 1.0 -  (height - lowerLimit) / buffer;
			} else {
				grassAlpha = 0.0 ;
			}
		}



		vec3 grass = texture2D( grassTexture, gl_TexCoord[0].st).rgb ;
		vec3 rock = texture2D( rockTexture, gl_TexCoord[0].st).rgb ;
		vec3 res = grassAlpha * grass + (1.0 - grassAlpha) * rock;
		gl_FragColor = vec4(res.r, res.g, res.b, 1.0) ;

}