// FRAGMENT SHADER
#version 330

uniform mat4 matrixView;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;

uniform float shininess;
uniform float reflectionPower;

uniform sampler2D texture0;

uniform samplerCube textureCubeMap;

in vec2 texCoord0;
in vec3 normal;
in vec3 texCoordCubeMap;
in vec4 color;
in vec4 position;

out vec4 outColor;



struct AMBIENT
{	
	int on;
	vec3 color;
};
uniform AMBIENT lightAmbient,bulbAmbient1,bulbAmbient2;

//Calculate Ambient Light function

vec4 AmbientLight(AMBIENT light)
{
	return vec4(materialAmbient * light.color, 1);
}



struct DIRECTIONAL
{	
	int on;
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

//Calculate Directional Light function

vec4 DirectionalLight(DIRECTIONAL light)
{
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);
	if (NdotL > 0)
	color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
	return color;
}


struct POINT
{
	int on;
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};
uniform POINT lightPoint1,lightPoint2;

//Calculate Point Light function

vec4 PointLight(POINT light)
{
	vec4 color = vec4(0, 0, 0, 0);

	vec3 L = normalize((matrixView * vec4(light.position,1)-position).xyz);
	float NdotL = dot(L,normal);

	if(NdotL>0)
		color+=vec4(materialDiffuse*light.diffuse,1)*NdotL;

	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);

	if (NdotL > 0 && RdotV > 0)
	    color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	return color;
}



void main(void) 
{
  outColor = color;
  
	// calculate light
	if (lightAmbient.on == 1) 
		outColor += AmbientLight(lightAmbient);
	
	if (bulbAmbient1.on == 1) 
		outColor += AmbientLight(bulbAmbient1);
	
	if (bulbAmbient2.on == 1) 
		outColor += AmbientLight(bulbAmbient2);

	if (lightDir.on == 1) 
		outColor += DirectionalLight(lightDir);

	if (lightPoint1.on == 1) 
		outColor += PointLight(lightPoint1);

	if (lightPoint2.on == 1) 
		outColor += PointLight(lightPoint2);

	outColor = mix(outColor * texture(texture0, texCoord0.st),texture(textureCubeMap, texCoordCubeMap)
                      , reflectionPower);
}
