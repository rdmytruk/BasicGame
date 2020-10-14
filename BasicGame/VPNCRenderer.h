#pragma once
#include "Shader.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
struct VPNCRenderer
{
	Shader shader;
	int wvp_id;
	int position_id;
	int light_direction_id;

	void ReloadShaders();
	void UseProgram() const;
	void SetCamera(const glm::mat4 & wvp) const;
	void SetPosition(glm::vec3 position) const;
	void SetLightDirection(glm::vec3 direction) const;
};

