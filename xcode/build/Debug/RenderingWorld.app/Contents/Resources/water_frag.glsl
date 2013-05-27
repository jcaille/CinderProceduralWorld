#version 120

// Uniforms

uniform sampler2D waterHeightMap ;
uniform sampler2D waterTexture;
uniform vec3 sunPosition ;
uniform vec3 cameraPosition ;
uniform float size ;
uniform float heightMultiplicator ;
uniform float baseHeight ;
uniform float elapsedTime ;
uniform bool enableFog ;
uniform float yValue ;
uniform float exponent ;

// Kernel

void main(void)
{

	// float height = texture2D( heightMap, gl_TexCoord[0].st).r ;

	// COMPUTE NORMAL VECTOR

	vec3 normal = vec3(0, 1, 0) ;

	float x = gl_TexCoord[0].s ; 
	float y = gl_TexCoord[0].t ; 
	
	float loop = sin((1.5 + texture2D(waterHeightMap, vec2(1-x, 1-y)).r )* elapsedTime / 2);

	float d = 0.01 ;
	float sx = loop * (texture2D(waterHeightMap, vec2(x <= 1.0 - d ? x + d : x, y)).r - texture2D(waterHeightMap, vec2(x >= d ? x - d : x, y)).r)/(2 * d);
	float sy = loop * (texture2D(waterHeightMap, vec2(x, y <= 1.0 - d ? y + d : y)).r - texture2D(waterHeightMap, vec2(x, y >= d ? y - d : y)).r)/(2 * d);
	
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

	normal = normalize(vec3(-sx, yValue, -sy));
	vec3 basicNormal = vec3(0,1,0);

	// COMPUTE LIGHTNING VALUE

	x = gl_TexCoord[0].s ;
	y = gl_TexCoord[0].t ;

	vec3 pixelPosition = vec3(size * (x - 0.5), baseHeight , size * (y - 0.5));

	vec3 lightDirection = sunPosition - pixelPosition;
	lightDirection = normalize(lightDirection);

	vec3 cameraDirection = cameraPosition - pixelPosition;
	cameraDirection = normalize(cameraDirection);

	vec3 h = lightDirection + cameraDirection;
	h = normalize(h);

	float specular_light =  max(0,pow(dot(h, normal), exponent)) * 0.8;
	float diffuse_light = min(0.8, max(1.0, dot(normal, lightDirection)));
	
	float specular_light_2 = max(0,pow(dot(h, basicNormal), exponent)) * 0.9;

	vec3 specularity = (specular_light * vec3(1.0, 0.8, 0.8) + specular_light_2 * vec3(1.0, 1.0, 1.0)) / 2;

	vec2 waterDisplacement = elapsedTime / 100 * vec2(cos(cos(elapsedTime / 1000)), sin(cos(elapsedTime / 1000)));
	vec3 water = 	texture2D( waterTexture,	mod(waterDisplacement+ 8  * gl_TexCoord[0].st, 1.0)).rgb ;



	//COMPUTE FOG

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

	vec4 fogColor = vec4(0.0, 0.0, 1.0, 0.0);

	vec3 renderedWater = water * diffuse_light + specularity;
	vec4 renderedTransparentWater = vec4(renderedWater.r, renderedWater.g, renderedWater.b, 0.7);

	vec4 res ;
	res = renderedTransparentWater;

	gl_FragColor = res ;
	gl_FragColor *= 1.0 - fogAlpha ;

}