#pragma once
#include <string>
#include <GL/glew.h>
class Shader
{
private:
	GLuint vertex_shader_id;
	GLuint fragment_shader_id;
	GLuint program_id;
	bool linked;
public:
	Shader();
	Shader(std::string vertex_shader, std::string fragment_shader);
	Shader(const Shader & copy);
	Shader & operator=(Shader copy);
	~Shader();
	friend void swap(Shader & a, Shader & b);
	GLuint GetProgram() const;
private:
	bool CompileShader(const std::string & filename, GLenum type, GLuint * shader_id);
	bool Link();
	std::string GetSource(const std::string & filename) const;
};

