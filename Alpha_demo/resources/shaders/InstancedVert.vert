#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture;
layout (location = 3) in mat4 instanceMatrices;

//Vertex Data
out vec3 vertexColor;
out vec2 texturePosition;

//Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
//uniform mat4 transform;

void main() 
{
    float t = sin(time)/(100);

    vec4 pos = instanceMatrices * model * vec4(position, 1.0);
    if (pos.y > 0.001)
    {
        pos.x += (t + 0.02) * pos.y * 20;
    }
    gl_Position  = projection * view * pos;
    vertexColor = color;
    texturePosition = texture;
    texturePosition = vec2(texture.x, texture.y);
}