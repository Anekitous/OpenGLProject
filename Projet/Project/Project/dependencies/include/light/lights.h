#ifndef LIGHTS_H
#define LIGHTS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


class Lights {
public:
	// light attributes
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 lightDir;

	// light components
	glm::vec3 ambient; // pas sur
	glm::vec3 diffuse;
	glm::vec3 specular;

	float specularStrength;

	Lights(glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0), glm::vec3 lightdir = glm::vec3(0.0,0.0,0.0)) {
		Position = position;
		Color = color;
		lightDir = lightdir;
	}

};
#endif