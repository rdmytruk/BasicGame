#include "PlayerRenderer.h"
#include <glm/gtc/type_ptr.hpp>

void PlayerRenderer::ReloadShaders()
{
	shader = Shader("VPNC.vert", "VPNC_Player.frag");
	wvp_id = glGetUniformLocation(shader.GetProgram(), "mat_wvp");
	position_id = glGetUniformLocation(shader.GetProgram(), "position");
	light_direction_id = glGetUniformLocation(shader.GetProgram(), "light_direction");
	color_id = glGetUniformLocation(shader.GetProgram(), "player_color");

	// if a variable comes back -1 after glGetUniformLocation it means:
	// a) it can't find that variable name in the code
	// b) it found that variable name in the code but it is unused so it was optimized out
}

void PlayerRenderer::UseProgram() const
{
	glUseProgram(shader.GetProgram());
}

void PlayerRenderer::SetCamera(const glm::mat4 & wvp) const
{
	glUniformMatrix4fv(wvp_id, 1, GL_FALSE, glm::value_ptr(wvp));
}

void PlayerRenderer::SetPosition(glm::vec3 position) const
{
	glUniform3fv(position_id, 1, glm::value_ptr(position));
}

void PlayerRenderer::SetLightDirection(glm::vec3 direction) const
{
	glUniform3f(light_direction_id, direction.x, direction.y, direction.z);
}

void PlayerRenderer::SetColor(glm::vec3 color) const
{
	glUniform3fv(color_id, 1, glm::value_ptr(color));
}
