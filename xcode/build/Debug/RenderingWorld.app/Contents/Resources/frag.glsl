#version 120

// Uniforms

uniform sampler2D heightMap ;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2D dirtTexture ;
uniform sampler2D sandTexture ;
uniform vec3 sunPosition ;
uniform vec3 cameraPosition ;
uniform float  size ;
uniform float heightMultiplicator ;
uniform bool enableFog ;


//PARAMETERS
uniform float dirtLimit ;
uniform float sandGrassLimit ;
uniform float sandGrassBuffer ;
uniform float sandGrassSlopeModifier ;

// Kernel

void main(void)
{

	float size_copy = size ;
	float height = texture2D(heightMap, gl_TexCoord[0].st).r ;

	// COMPUTE NORMAL VECTOR

	vec3 normal ;

	float x = gl_TexCoord[0].s;
	float y = gl_TexCoord[0].t;

	float d = 0.01 ;
	float sx = (texture2D(heightMap, vec2(x <= 1.0 - d ? x + d : x, y)).r - texture2D(heightMap, vec2(x >= d ? x - d : x, y)).r)/(2 * d);
	float sy = (texture2D(heightMap, vec2(x, y <= 1.0 - d ? y + d : y)).r - texture2D(heightMap, vec2(x, y >= d ? y - d : y)).r)/(2 * d);
	
	if(x >= 1.0 - d){
		sx *= 2 ;
	} else {}
	if(x <= d){
		sx *= 2 ;	
	} else {}

	if(y >= 1.0 - d){
		sy *= 2 ;
	} else {}
	if(y <= d){
		sy *= 2 ;	
	} else {}

	normal = vec3(-sx , 2  , -sy );
	normal = normalize(normal);

	float slope = sqrt(normal.x * normal.x + normal.z * normal.z) ;

	// COMPUTE LIGHTNING VALUE

	vec3 currentPosition = vec3(size * (x - 0.5), heightMultiplicator * (height - 0.5) , size * (y - 0.5));

	vec3 lightDirection = sunPosition - currentPosition;

	lightDirection = normalize(lightDirection);
	float diffuseLight =  min(1.0, 0.5 + max(0.0 , dot(lightDirection, normal )));

	//COMPUTE GRASS ALPHA VALUE
	//Set buffers and limits :
	//Water limit is at  0.3

	// float sandGrassLimit = 0.35 ;
	// float sandGrassBuffer = 0.05 ;
	// float sandGrassSlopeModifier = 0.0 ;

	float grassRockLimit = 0.70 ;
	float grassRockBuffer = 0.07 ;
	float grassRockSlopeModifier = - 1 / 4 ;

	float rockSnowLimit = 0.80 ;
	float rockSnowBuffer = 0.1 ;
	float rockSnowSlopeMofifier = 1/8 ;

	float sandAlpha = 0.0 ;
	float grassAlpha = 0.0 ;
	float rockAlpha = 0.0 ;
	float snowAlpha = 0.0 ;


	float grassUpperLimit = 0.65 - slope / 3 ;
	float buffer = 0.07 ;

	//Compute Sand Alpha
	float realSandGrassLimit = sandGrassLimit + sandGrassSlopeModifier * slope ;
	if(height < realSandGrassLimit){
		sandAlpha = 1.0 ;
	} else {
		if(height < realSandGrassLimit + sandGrassBuffer){
				sandAlpha = 1 - (height - realSandGrassLimit) / sandGrassBuffer ;
			} else {
				sandAlpha = 0.0 ;
			}
	}

	//Compute Grass Alpha
	float realGrassRockLimit = grassRockLimit + grassRockSlopeModifier * slope ;
	if(height > realSandGrassLimit){
		if( height < realSandGrassLimit + sandGrassBuffer ){
			grassAlpha = 1 - sandAlpha ;
		} else {
			if(height < realGrassRockLimit){
				grassAlpha = 1.0 ;
			} else {
				if(height < realGrassRockLimit + grassRockBuffer){
					grassAlpha = 1 - (height - realGrassRockLimit) / grassRockBuffer ;
				}
			}
		}
	}

	//Compute Rock Alpha
	float realRockSnowLimit = rockSnowLimit + rockSnowSlopeMofifier * slope ;
	if(height > realGrassRockLimit){
		if( height < realGrassRockLimit + grassRockBuffer ){
			rockAlpha = 1 - grassAlpha ;
		} else {
			if(height < realRockSnowLimit){
				rockAlpha = 1.0 ;
			} else {
				if(height < realRockSnowLimit + rockSnowBuffer){
					rockAlpha = 1 - (height - realRockSnowLimit) / rockSnowBuffer ;
				} else {
					rockAlpha = 0.0 ;
				}
			}
		}
	}

	if(height > realRockSnowLimit){
		snowAlpha = 1.0 - rockAlpha ;
	}


	// ADD DIRT WHERE THERE IS GRASS AND SLOPE
	float dirtAlpha = 0.0;
	// float dirtLimit = 0.5  ;
	if(slope > dirtLimit){
		float lambda = (slope - dirtLimit) / (1.0 - dirtLimit);
		dirtAlpha = lambda * grassAlpha ;
		grassAlpha -= dirtAlpha ;			
	}
	
	snowAlpha = 1.0 - rockAlpha - grassAlpha - dirtAlpha - sandAlpha ;

	// //COMPUTE ROCK ALPHA
	// float rockAlpha = 1.0 - snowAlpha - grassAlpha - dirtAlpha;

	//COMPUTE FOG

	vec3 pixelPosition = vec3( size * (x - 0.5),  (height - 0.5) * heightMultiplicator, size * (y - 0.5));
	vec3 de = pixelPosition - cameraPosition ;

	float distanceToCamera = length(de);

	float fogLimit = 1400 ;
	float fogBuffer = 400 ;

	float fogAlpha = 0.0 ;
	if(distanceToCamera > fogLimit){
		if(distanceToCamera > fogLimit + fogBuffer){
			fogAlpha = 1.0 ;
		} else {
			fogAlpha = (distanceToCamera - fogLimit) / fogBuffer ;
		}
	}

	vec3 fogColor = vec3 (0.0, 1.0, 0.0);

	vec3 grass = 	grassAlpha 	* texture2D( grassTexture, 	mod(10 * gl_TexCoord[0].st, 1.0)).rgb ;
	vec3 rock = 	rockAlpha 	* texture2D( rockTexture, 	mod(10 * gl_TexCoord[0].st, 1.0)).rgb ;
	vec3 snow = 	snowAlpha 	* texture2D( snowTexture, 	mod(10 * gl_TexCoord[0].st, 1.0)).rgb ;
	vec3 dirt = 	dirtAlpha	* texture2D( dirtTexture, 	mod(10 * gl_TexCoord[0].st, 1.0)).rgb ;
	vec3 sand =		sandAlpha 	* texture2D( sandTexture, 	mod(10 * gl_TexCoord[0].st, 1.0)).rgb ;

	vec3 terrain = (grass + rock + snow + dirt + sand) * diffuseLight ;
	vec3 res ;
	if(enableFog){
		// res = fogAlpha * fogColor + (1-fogAlpha) * terrain ;
		res = terrain ;
	} else {
		res = terrain ;
	}

	gl_FragColor = vec4(terrain.r, terrain.g, terrain.b, 1.0) ;
	gl_FragColor *= 1.0 - fogAlpha ;
}