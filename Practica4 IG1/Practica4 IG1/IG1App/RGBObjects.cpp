#include "RGBObjects.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

RGBTriangle::RGBTriangle(GLdouble r, GLfloat Radio)
{
	mMesh = Mesh::generateRegularPolygon(3, r, GL_TRIANGLES);
}

// animacion de doble giro
void RGBTriangle::update()
{
	mModelMat = rotate(mModelMat, radians(-2.0f), vec3{ 0, 0, 1 }); // giro propio
	mModelMat = rotate(mat4(1), radians(1.0f), vec3{ 0, 0, 1 }) * mModelMat; // giro orbital
}


RGBRectangle::RGBRectangle(GLdouble w, GLdouble h)
{
	mMesh = Mesh::generateRGBRectangle(w, h);
}

void RGBRectangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		upload(aMat);

		glEnable(GL_CULL_FACE);

		// Se descartan frontales → se ve la cara trasera en relleno.
		glCullFace(GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mMesh->render();

		// Se descartan traseras → se ve la cara delantera en wireframe.
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mMesh->render();

		// Restaurar estado.
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
	}
}

RGBCube::RGBCube(GLdouble length)
{
	mMesh = Mesh::generateRGBCubeTriangles(length);
}