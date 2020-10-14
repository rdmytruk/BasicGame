#include "Shader.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "DebugOut.h"

Shader::Shader()
{
	vertex_shader_id = 0;
	fragment_shader_id = 0;
	program_id = 0;
	linked = false;
}

Shader::Shader(std::string vertex_shader, std::string fragment_shader)
	: Shader()
{
	assert(CompileShader(vertex_shader, GL_VERTEX_SHADER, &vertex_shader_id));
	assert(CompileShader(fragment_shader, GL_FRAGMENT_SHADER, &fragment_shader_id));
	assert(Link());
}

Shader::Shader(const Shader & copy)
{
	// TODO rewrite to be a brand new copy of the shaders instead of copying these values
	// which are sharing resources on GPU
	vertex_shader_id = copy.vertex_shader_id;
	fragment_shader_id = copy.fragment_shader_id;
	program_id = copy.fragment_shader_id;
	linked = copy.linked;
}

Shader & Shader::operator=(Shader copy)
{
	swap(*this, copy);
	return *this;
}

Shader::~Shader()
{
	if (!linked)
	{
		glDetachShader(program_id, vertex_shader_id);
		glDetachShader(program_id, fragment_shader_id);
		linked = false;
	}

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
	glDeleteProgram(program_id);

	vertex_shader_id = 0;
	fragment_shader_id = 0;
	program_id = 0;
}

bool Shader::CompileShader(const std::string & filename, GLenum type, GLuint * shader_id)
{
	std::string shader_text = GetSource(filename);
	*shader_id = glCreateShader(type);
	const GLchar * source = static_cast<const GLchar*>(shader_text.c_str());
	glShaderSource(*shader_id, 1, &source, nullptr);
	glCompileShader(*shader_id);

	// request from opengl GL_COMPILE_STATUS of *shader_id into variable compiled
	GLint compiled = 0;
	glGetShaderiv(*shader_id, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(*shader_id, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
		{
			maxLength = 1024;
		}

		// the maxLength includes NULL
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(*shader_id, maxLength, &maxLength, &infoLog[0]);

		//infolog contains our error message - write it out
		std::string error_message(infoLog.begin(), infoLog.end());
		DBOUT("Shader Compile Error:" << filename.c_str() << std::endl << error_message.c_str() << std::endl);

		// We don't need the shader anymore
		glDeleteShader(*shader_id);
		*shader_id = 0;
		return false;
	}

	return true;
}

bool Shader::Link()
{
	GLuint program = glCreateProgram();
	// attach our shaders
	glAttachShader(program, vertex_shader_id);
	glAttachShader(program, fragment_shader_id);
	glLinkProgram(program);

	GLint linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
			maxLength = 1024;

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//infolog contains our error message - write it out
		std::string error_message(infoLog.begin(), infoLog.end());
		DBOUT("Shader Link Error:" << std::endl << error_message.c_str() << std::endl);

		// Delete the shaders
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		vertex_shader_id = 0;
		fragment_shader_id = 0;

		// We don't need the program anymore
		glDeleteProgram(program);
		program = 0;

		//infolog contains our error message
		// TOOD write out error message

		return false;
	}

	// Always detach shaders after a successful link
	glDetachShader(program, vertex_shader_id);
	glDetachShader(program, fragment_shader_id);

	program_id = program;
	linked = true;

	return linked;
}

std::string Shader::GetSource(const std::string & filename) const
{
	const std::string line_comment = "//";

	std::ifstream infile(filename);
	std::string line;

	std::ostringstream s;
	while (std::getline(infile, line))
	{
		if (!line.empty())
		{
			// strip out line comments
			size_t comment_from = line.find(line_comment);
			if (comment_from != std::string::npos)
			{
				if (comment_from > 0)
				{
					line = line.substr(0, comment_from);
					s << line << std::endl;
					continue;
				}
			}
			else
			{
				s << line << std::endl;
			}
		}

	}

	infile.close();
	return s.str();
}

GLuint Shader::GetProgram() const
{
	return program_id;
}

void swap(Shader & a, Shader & b)
{
	using std::swap;
	swap(a.vertex_shader_id, b.vertex_shader_id);
	swap(a.fragment_shader_id, b.fragment_shader_id);
	swap(a.program_id, b.program_id);
	swap(a.linked, b.linked);
}
