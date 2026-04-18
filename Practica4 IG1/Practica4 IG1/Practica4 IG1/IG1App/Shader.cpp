#include <fstream>
#include <map>
#include <memory>
#include <filesystem>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

using namespace std;

static map<string, unique_ptr<Shader>>* shaders = nullptr;

constexpr const char* SHADERS_ROOT = "../assets/shaders/";

// Compila vertex + fragment (y geometry si existe) y los enlaza en un programa.
Shader::Shader(const string& name)
{
	GLuint vertex, fragment;

	// Separar nombres de vertex y fragment si se usa el formato "vtx:frag".
	string vertexName = name, fragmentName = name;
	if (auto pos = name.find(':'); pos != string::npos) {
		vertexName = name.substr(0, pos);
		fragmentName = name.substr(pos + 1);
	}

	buildShader(vertex, GL_VERTEX_SHADER, SHADERS_ROOT + vertexName + "_vertex.glsl");
	buildShader(fragment, GL_FRAGMENT_SHADER, SHADERS_ROOT + fragmentName + "_fragment.glsl");

	mProgram = glCreateProgram();
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	// El geometry shader es opcional: solo se carga si el fichero existe.
	if (std::filesystem::exists(SHADERS_ROOT + name + "_geometry.glsl")) 
	{
		GLuint geometry;
		buildShader(geometry, GL_GEOMETRY_SHADER, SHADERS_ROOT + name + "_geometry.glsl");
		glAttachShader(mProgram, geometry);
	}

	glLinkProgram(mProgram);
	// Los objetos de shader individuales ya no son necesarios tras el enlazado.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(mProgram);
}

// Lee el fichero GLSL, lo compila y guarda el ID en 'shader'
bool
Shader::buildShader(GLuint& shader, GLuint type, const string& filename)
{
	ifstream source(filename);
	if (!source.is_open())
		return false;

	string buffer(istreambuf_iterator<char>(source), {});
	const char* bufferp = buffer.data();

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &bufferp, nullptr);
	glCompileShader(shader);

	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
		throw logic_error("Error al compilar el shader: " + filename);

	return true;
}

// Activa este programa para el siguiente draw call.
void Shader::use() { glUseProgram(mProgram); }

void Shader::setUniform(const string& name, bool value)
{
	glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
}

void Shader::setUniform(const string& name, float value)
{
	glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value);
}

void Shader::setUniform(const string& name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(mProgram, name.c_str()), value.r, value.g, value.b);
}

void Shader::setUniform(const string& name, const glm::vec4& value)
{
	glUniform4f(glGetUniformLocation(mProgram, name.c_str()), value.r, value.g, value.b, value.a);
}

void Shader::setUniform(const string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


Shader*
Shader::get(const string& name)
{
	// Crear el mapa la primera vez que se solicita un shader
	if (shaders == nullptr)
		shaders = new map<string, unique_ptr<Shader>>();

	auto it = shaders->find(name);
	if (it != shaders->end())
		return it->second.get();

	// Primera vez: compilar y almacenar en la caché
	Shader* shader = new Shader(name);
	(*shaders)[name] = unique_ptr<Shader>(shader);
	return shader;
}

void
Shader::setUniform4All(const string& name, const glm::mat4& value)
{
	if (shaders == nullptr) return;
	for (const auto& [_, shader] : *shaders) {
		shader->use();
		shader->setUniform(name, value);
	}
}

// Destruye completamente la caché de shaders y libera su memoria del heap
void
Shader::freeAll()
{
	delete shaders; // destruye map + unique_ptrs + ~Shader() + glDeleteProgram
	shaders = nullptr;
}