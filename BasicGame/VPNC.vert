#version 330
// What the data coming in to the vertex shader looks like
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vertex_color;

// Data that leaves the vertex shader and goes on to the fragment shader
out vec3 pos3D;
out vec3 normal;
out vec3 color;

// variables that can be changed by CPU code
uniform mat4 mat_wvp;
uniform vec3 position;

void main(void)
{
	// gl_Position is a built in variable (shader intrinsic variable) that needs to be written to
	// its data is used by the rasterizer to generate data for the fragment shader
	gl_Position = mat_wvp * vec4(vertex_position + position, 1.0);
	pos3D = vertex_position;
	normal = vertex_normal;
	color = vertex_color;
}