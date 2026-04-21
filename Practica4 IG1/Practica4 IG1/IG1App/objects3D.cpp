#include "objects3D.h"
#include "IndexMesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// ─── CUBO ─────────────────────────────────────────────────────────────────────

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // caras frontales: wireframe
	mMesh->render();

	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // caras traseras: puntos
	mMesh->render();

	// Restaurar estado GL
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

// ─── BOX OUTLINE ──────────────────────────────────────────────────────────────

BoxOutline::BoxOutline(GLdouble length)
	: mTexture2(nullptr)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);

	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\papelE.png");

	mTexture2 = new Texture();
	mTexture2->load("..\\assets\\images\\container.jpg");
}

BoxOutline::~BoxOutline()
{
	delete mTexture2;
	mTexture2 = nullptr;
}

void BoxOutline::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	glDisable(GL_BLEND);
	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	upload(aMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	// Exterior: ver cara trasera (la que mira hacia fuera de la caja)
	glCullFace(GL_BACK);
	mTexture->bind();
	mMesh->render();
	mTexture->unbind();

	// Interior: ver cara frontal (la que mira hacia dentro)
	glCullFace(GL_FRONT);
	mTexture2->bind();
	mMesh->render();
	mTexture2->unbind();

	glDisable(GL_CULL_FACE);
}

// ─── ESTRELLA 3D ──────────────────────────────────────────────────────────────

Star3D::Star3D(GLdouble re, GLuint np, GLdouble h)
{
	mMesh = Mesh::generateStar3DTexCor(re, np, h);
	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\rueda.png");
}

void Star3D::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (mTexture != nullptr) mTexture->bind();

	// Estrella 1
	glm::mat4 aMat1 = modelViewMat * mModelMat;
	upload(aMat1);
	mMesh->render();

	// Estrella 2: girada 180° sobre X para quedar enfrentada
	glm::mat4 aMat2 = modelViewMat * mModelMat *
		glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0));
	upload(aMat2);
	mMesh->render();

	if (mTexture != nullptr) mTexture->unbind();
}

void Star3D::update()
{
	mModelMat = glm::rotate(mModelMat, glm::radians(1.0f), glm::vec3(0, 0, 1));
	mModelMat = glm::rotate(mModelMat, glm::radians(0.5f), glm::vec3(0, 1, 0));
}

// ─── GLASS PARAPET ────────────────────────────────────────────────────────────

GlassParapet::GlassParapet(GLdouble length)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);
	mTexture = new Texture();
	mTexture->load("..\\assets\\images\\windowC.png", 200); // alpha semitransparente
}

void GlassParapet::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;
	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("modulate", mModulate);
	upload(aMat);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // no escribe en z-buffer para que lo de detrás siga visible

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	// Primer paso: caras frontales (interior del cristal)
	glCullFace(GL_FRONT);
	if (mTexture) mTexture->bind();
	mMesh->render();

	// Segundo paso: caras traseras (exterior del cristal)
	glCullFace(GL_BACK);
	mMesh->render();
	if (mTexture) mTexture->unbind();

	// Restaurar estado
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

// ─── INDEXED BOX (Apt. 62/64) ─────────────────────────────────────────────────

IndexedBox::IndexedBox(GLdouble length)
	: ColorMaterialEntity(glm::vec4(0, 1, 0, 1)) // verde
{
	// Apt. 64: 24 vértices (4 por cara), normales correctas por cara
	mMesh = IndexMesh::generateIndexedBox(length);
}