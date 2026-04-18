#include "Texture.h"
#include <vector>
#include <iostream>
#include "Image.h"

// Libera el objeto de textura en la GPU si fue creado
Texture::~Texture()
{
	if (mId != 0)
		glDeleteTextures(1, &mId);
}

// Genera el objeto de textura en la GPU y configura los parámetros por defecto
void Texture::init()
{
	glGenTextures(1, &mId);
	glBindTexture(GL_TEXTURE_2D, mId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

// Activa esta textura para los siguientes draw calls
void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mId);
}

// Carga una imagen desde fichero y la sube a la GPU como textura RGBA
void Texture::load(const std::string& name, GLubyte alpha)
{
	if (mId == 0) init();

	Image image;
	image.load(name);

	if (alpha != 255)
		image.setAlpha(alpha); // ajustar transparencia global

	mWidth = image.width();
	mHeight = image.height();

	glBindTexture(GL_TEXTURE_2D, mId);
	// glTexImage2D sube los datos de la imagen a la GPU
	// Nivel 0 = imagen base (sin mipmaps), sin borde, formato RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		mWidth, mHeight, 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		image.data());

	glBindTexture(GL_TEXTURE_2D, 0); // desvincular para no afectar otras operaciones
}

// Configura el modo de repetición/clamping de la textura en ambos ejes UV
void Texture::setWrap(GLuint wp)
{
	glBindTexture(GL_TEXTURE_2D, mId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wp);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Carga el color buffer del framebuffer como textura 2D
void Texture::loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer)
{
	if (mId == 0) init();

	mWidth = width;
	mHeight = height;

	glReadBuffer(buffer);
	glBindTexture(GL_TEXTURE_2D, mId);
	// Copia el rectángulo (0,0,width,height) del framebuffer a la textura.
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}