#version 430 core

#define NO_DATA 0
#define TEXTURED 1
#define MATERIALS 2
#define COMBINED 3
#define SKYBOX 4

layout(location=0) in vec4 vec4ObjectCoords;
layout(location=1) in vec2 vec2TextureCoords;
layout(location=2) in vec3 vec3ObjectNormals;

//outputs
out vec2 vec2TextureCoordinateExport;
out vec3 vec3TextureCoordinateExport;
out vec3 vec3NormalExport;

out vec3 vec3FragmentPosition;

//data from backend
uniform uint uiVisualizationType;

uniform mat4 mat4View;
uniform mat4 mat4Model;
uniform mat4 mat4Projection;

void main(void)
{   
	vec3NormalExport = vec3ObjectNormals;

	vec4 vec4TempCoordinates = vec4ObjectCoords;
	//just to make sure that w is 1
	vec4TempCoordinates.w = 1;
	vec3FragmentPosition = vec3(mat4Model *  vec4TempCoordinates);

	switch(uiVisualizationType)
	{
		case TEXTURED:
			vec2TextureCoordinateExport = vec2TextureCoords;
			break;

		case SKYBOX:
			vec3TextureCoordinateExport = vec3(vec4TempCoordinates);
		break;
	}

	gl_Position = mat4Projection * mat4View * mat4Model * vec4TempCoordinates;	
}
