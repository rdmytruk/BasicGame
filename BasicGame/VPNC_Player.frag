#version 330
// input variables expected from the vertex shader
in vec3 pos3D;
in vec3 normal;
in vec3 color;

out vec4 frag_color;

// variables that can be changed by CPU code
uniform vec3 light_direction;
uniform vec3 player_color;

void main()
{
	float ambient = 0.35;
	float diffuse = max(0.0, dot(light_direction, normal));
	float lighting = clamp(ambient + diffuse, 0.0, 1.0);
	frag_color.rgba = vec4(player_color * lighting, 1.0);
}