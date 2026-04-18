#include "objects2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// POLIGONOS

RegularPolygon::RegularPolygon(GLuint num, GLdouble r)
{
	mMesh = Mesh::generateRegularPolygon(num, r, GL_LINE_LOOP);
}

// GROUND

Ground::Ground(GLdouble w, GLdouble h)
{
	const GLuint reps = 4; // la textura se repite 4×4 veces sobre el suelo
	mMesh = Mesh::generateRectangleTexCor(w, h, reps, reps);

	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\baldosaC.png");

	// Rotamos 90° para que se ponga en el suelo
	mModelMat = rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
}

// FOTO

Photo::Photo(GLdouble w, GLdouble h)
{
	mMesh = Mesh::generateRectangleTexCor(w, h, 1, 1);
	mTexture = new Texture();
	// La textura se actualiza en update()
}

// Convierte la captura en la textura de la foto
void Photo::update()
{
	// Leer las dimensiones reales del viewport para que la captura cubra toda la ventana.
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLsizei w = viewport[2];
	GLsizei h = viewport[3];
	mTexture->loadColorBuffer(w, h, GL_FRONT);
	mReady = true; // a partir de aquí render() ya puede mostrar la textura
}

void Photo::render(const glm::mat4& modelViewMat) const
{
	if (!mReady) return;

	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	upload(aMat);
	mTexture->bind();
	mMesh->render();
	mTexture->unbind();
}