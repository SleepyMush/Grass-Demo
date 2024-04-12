#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture;

//Vertex Data
out vec3 vertexColor;
out vec2 texturePosition;

//Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 transform;

void main() 
{
	gl_Position  = projection * view * model * vec4(position, 1.0);
	vertexColor = color;
	texturePosition = texture;
	texturePosition = vec2(texture.x, texture.y);
}