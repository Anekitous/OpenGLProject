#version 420 core

out vec4 FragColor;

in vec3 TextCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TextCoords);
}