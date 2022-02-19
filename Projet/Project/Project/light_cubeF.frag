#version 420 core
out vec4 FragColor;
in vec3 Normal;

uniform vec3 lightCubeColor;


void main()
{
    FragColor = 0.6 * vec4(lightCubeColor,1.0); // set all 4 vector values to 1.0
}