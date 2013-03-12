#version 150

// Uniforms
uniform float limitWaterGrass ;
uniform float limitGrassRock ;
uniform float textureHeight ;
uniform sampler2D grassTexture;

// Input attributes
in vec4 normal;
in vec4 position;
in vec4 uv;

// Output attributes
out vec4 color;

// Kernel
void main(void)
{
		float grassAlpha = 0.0 ;

		if(position.z > limitGrassRock + textureHeight || position.z < limitWaterGrass - textureHeight){
			float grassAlpha = 0.0 ;
		} else if(position.z > limitWaterGrass - textureHeight && position.z < limitWaterGrass) {
			float grassAlpha = (position.z - limitWaterGrass) / textureHeight ;
		} else if(position.z < limitGrassRock + textureHeight && position.z > limitGrassRock) {
			float grassAlpha = (limitGrassRock - position.z) / textureHeight ;
		} else {
			float grassAlpha = 1.0 ;
		}
		// Color point white
		vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
		vec4 grass = grassTexture(uv.t, uv.s);
		color = white * (1.0 - grassAlpha) + grass ;

	}

}