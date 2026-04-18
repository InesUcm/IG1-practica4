#ifndef _H_Texture_H_
#define _H_Texture_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Texture
{
public:
	Texture() = default;
	~Texture();

	Texture(const Texture& tex) = delete;
	Texture& operator=(const Texture& tex) = delete;

	void load(const std::string& filename,
		GLubyte alpha = 255);

	// Apartado 34: carga el color buffer como textura
	void loadColorBuffer(GLsizei width, GLsizei height,
		GLuint buffer = GL_FRONT);

	void bind();
	void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); };

	GLuint width() const { return mWidth; };
	GLuint height() const { return mHeight; };

	void setWrap(GLuint wp);

protected:
	void init();

	GLuint mWidth = 0;
	GLuint mHeight = 0;
	GLuint mId = 0;
};

#endif //_H_Texture_H_