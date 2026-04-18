#ifndef _H_Shader_H_
#define _H_Shader_H_

#include <string>
#include <glm/fwd.hpp>
#include <GL/glew.h>

// Compila y enlaza programas GLSL (vertex + fragment, y geometry si existe).
// Los shaders se almacenan en una caché interna (patrón Factory):
//   - Shader::get("nombre") devuelve el shader ya compilado o lo crea.
//   - Shader::freeAll() destruye toda la caché; llamar en IG1App::destroy()
//     ANTES de glfwTerminate() para que el contexto OpenGL siga activo
//     y para evitar falsos leaks detectados por el CRT de MSVC.
class Shader
{
public:
	// Compila vertex + fragment (y geometry si existe).
	// Formato del nombre: "simple" (mismo vtx y frag) o "vtx:frag" (distintos).
	explicit Shader(const std::string& name);
	~Shader(); // llama a glDeleteProgram

	Shader(const Shader& m) = delete;
	Shader& operator=(const Shader& m) = delete;

	// Activa este programa para el siguiente draw call.
	void use();

	// Envío de valores a uniforms del shader activo.
	void setUniform(const std::string& name, bool value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, const glm::mat4& value);

	// Devuelve el shader con ese nombre desde la caché; lo crea si no existe.
	static Shader* get(const std::string& name);
	// Envía el mismo uniform mat4 a todos los shaders (útil para la proyección).
	static void setUniform4All(const std::string& name, const glm::mat4& value);
	// Destruye COMPLETAMENTE la caché. Llamar antes de glfwTerminate().
	static void freeAll();

protected:
	GLuint mProgram; // identificador del programa enlazado en la GPU

	// Compila un shader individual desde fichero. Devuelve false si no existe.
	bool buildShader(GLuint& shader, GLuint type, const std::string& filename);
};

#endif // _H_Shader_H_