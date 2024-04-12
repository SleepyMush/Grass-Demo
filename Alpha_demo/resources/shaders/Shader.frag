#version 460 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 texturePosition;

uniform sampler2D Texture;


void main()
{
	FragColor = texture(Texture,texturePosition);
}


//uniform vec4 Ucolor;
//in vec4 color;
//* vec4(vertexColor, 1.0);