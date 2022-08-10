// VERTEX SHADER
#version 330

uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;
 
layout (location = 0) in vec3 aVertex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec2 texCoord0;
out vec3 normal;
out vec3 texCoordCubeMap;
out vec4 color;
out vec4 position;


void main(void) 
{
	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;

	// calculate normal
	normal=normalize(mat3(matrixModelView) * aNormal);

	// calculate textures coordinate
	texCoord0 = aTexCoord;

    texCoordCubeMap = inverse(mat3(matrixView)) * mix(reflect(position.xyz, normal.xyz), normal.xyz, 0.2);	

	//setting initial colour
	color = vec4(0, 0, 0, 1);
}
