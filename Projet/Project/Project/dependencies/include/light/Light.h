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
	glm::vec3 spotDir;
	float strength;
	float range;
	float cutOff;
	float outerCutOff;
	bool isSpot = false;

	Light(glm::vec3 _lightPos, glm::vec3 _lightColor, float _strength, float _range) {
		lightPos = _lightPos;
		lightColor = _lightColor;
		strength = _strength;
		range = _range;
		spotDir = glm::vec3(1.0f);
		outerCutOff = 0.0f;
		cutOff = 0.0f;
	}

	//Spotlight constructor
	Light(glm::vec3 _lightPos, glm::vec3 _lightColor, float _strength, float _range, glm::vec3 _spotDir, float _cutOff, bool _isSpot, float _outerCutOff) {
		lightPos = _lightPos;
		lightColor = _lightColor;
		strength = _strength;
		range = _range;
		spotDir = _spotDir;
		cutOff = _cutOff;
		outerCutOff = _outerCutOff;
		isSpot = _isSpot;
	}
};

#endif