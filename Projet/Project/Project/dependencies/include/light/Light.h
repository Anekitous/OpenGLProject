#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Light {
public:
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	float range;

	Light(glm::vec3 _lightPos, glm::vec3 _lightColor, float _range) {
		lightPos = _lightPos;
		lightColor = _lightColor;
		range = _range;
	}
};

#endif