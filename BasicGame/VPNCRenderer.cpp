#include "VPNCRenderer.h"
#include <glm/gtc/type_ptr.hpp>

void VPNCRenderer::ReloadShaders()
{
	shader = Shader("VPNC.vert", "VPNC.frag");
	wvp_id = glGetUniformLocation(shader.GetProgram(), "mat_wvp");
	position_id = glGetUniformLocation(shader.GetProgram(), "position");
	light_direction_id = glGetUniformLocation(shader.GetProgram(), "light_direction");

	// if a variable comes back -1 after glGetUniformLocation it means:
	// a) it can't find that variable name in the code
	// b) it found that variable name in the code but it is unused so it was optimized out
}

void VPNCRenderer::UseProgram() const
{
	glUseProgram(shader.GetProgram());
}

void VPNCRenderer::SetCamera(const glm::mat4 & wvp) const
{
	glUniformMatrix4fv(wvp_id, 1, GL_FALSE, glm::value_ptr(wvp));
}

void VPNCRenderer::SetPosition(glm::vec3 position) const
{
	glUniform3fv(position_id, 1, glm::value_ptr(position));
}

void VPNCRenderer::SetLightDirection(glm::vec3 direction) const
{
	glUniform3f(light_direction_id, direction.x, direction.y, direction.z);
}
