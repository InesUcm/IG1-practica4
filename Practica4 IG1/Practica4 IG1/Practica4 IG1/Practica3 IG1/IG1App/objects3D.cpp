#include "objects3D.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// CUBO

Cube::Cube(GLdouble length)
{
	mMesh = Mesh::generateCube(length);
}


void Cube::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("color", mColor);
	upload(aMat);

	glEnable(GL_CULL_FACE);

	glCullFace(GL_FRONT);                         
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    // wireframe
	mMesh->render();

	glCullFace(GL_BACK);                        
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);   // puntos
	mMesh->render();

	// Restaurar estado para no afectar objetos siguientes.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

//BOX OUTLINE

BoxOutline::BoxOutline(GLdouble length)
	: mTexture2(nullptr)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);

	mTexture = new Texture(); // textura exterior (cara visible desde fuera)
	mTexture->load("..\\assets\\images\\papelE.png");

	mTexture2 = new Texture(); // textura interior (cara visible desde dentro)
	mTexture2->load("..\\assets\\images\\container.jpg");
}

// mTexture es liberada por ~EntityWithTexture(); aquí solo liberamos mTexture2.
BoxOutline::~BoxOutline()
{
	delete mTexture2;
	mTexture2 = nullptr;
}


void BoxOutline::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	glDisable(GL_BLEND); // limpiar estado blend por si acaso
	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	upload(aMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	// Exterior: se descartan las caras frontales, se ven las traseras.
	glCullFace(GL_BACK);
	mTexture->bind();
	mMesh->render();
	mTexture->unbind();

	// Interior: se descartan las caras traseras, se ven las frontales.
	glCullFace(GL_FRONT);
	mTexture2->bind();
	mMesh->render();
	mTexture2->unbind();

	glDisable(GL_CULL_FACE);
}

// ESTRELLA

Star3D::Star3D(GLdouble re, GLuint np, GLdouble h)
{
	// Malla de estrella con coordenadas de textura. re=radio exterior, np=puntas, h=altura.
	mMesh = Mesh::generateStar3DTexCor(re, np, h);
	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\rueda.png");
}


void Star3D::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (mTexture != nullptr) mTexture->bind();

		// estrella 1
		glm::mat4 aMat1 = modelViewMat * mModelMat;
		upload(aMat1);
		mMesh->render();

		// estrella 2, rotada 180 grados sobre X para quedar enfrentada
		glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0));
		glm::mat4 aMat2 = modelViewMat * mModelMat * rotateMat;
		upload(aMat2);
		mMesh->render();

		if (mTexture != nullptr) mTexture->unbind();
	}
}

void Star3D::update()
{
	mModelMat = glm::rotate(mModelMat, glm::radians(1.0f), glm::vec3(0, 0, 1));
	mModelMat = glm::rotate(mModelMat, glm::radians(0.5f), glm::vec3(0, 1, 0));
}

// GLASS PARAPET

GlassParapet::GlassParapet(GLdouble length)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);
	mTexture = new Texture();
	// Alpha 200/255: semitransparente para simular vidrio.
	mTexture->load("..\\assets\\images\\windowC.png", 200);
}


void GlassParapet::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //mezcla el cristal con lo que hay detrás
		glDepthMask(GL_FALSE); // no escribir en z-buffer para que los objetos detrás del cristal sigan siendo visibles

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);

		if (mTexture != nullptr) mTexture->bind();
		mMesh->render();
		if (mTexture != nullptr) mTexture->unbind();

		// Restaurar estado GL para que no afecte a objetos renderizados después.
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
}