#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// ─── Abs_Entity ───────────────────────────────────────────────────────────────

Abs_Entity::~Abs_Entity()
{
	delete mMesh;
	mMesh = nullptr;
}

void Abs_Entity::load()
{
	if (mMesh) mMesh->load();
}

void Abs_Entity::unload()
{
	if (mMesh) mMesh->unload();
}

void Abs_Entity::upload(const mat4& modelViewMat) const
{
	mShader->setUniform("modelView", modelViewMat);
}

// ─── EntityWithColors ─────────────────────────────────────────────────────────

EntityWithColors::EntityWithColors()
{
	mShader = Shader::get("vcolors");
}

void EntityWithColors::render(const mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

// ─── SingleColorEntity ────────────────────────────────────────────────────────

SingleColorEntity::SingleColorEntity(glm::vec4 color)
	: mColor(color)
{
	mShader = Shader::get("simple");
}

void SingleColorEntity::render(const mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("color", mColor);
		upload(aMat);
		mMesh->render();
	}
}

// ─── RGBAxes ──────────────────────────────────────────────────────────────────

RGBAxes::RGBAxes(GLdouble l)
{
	mMesh = Mesh::createRGBAxes(l);
}

// ─── EntityWithTexture ────────────────────────────────────────────────────────

EntityWithTexture::EntityWithTexture()
	: mTexture(nullptr)
{
	mShader = Shader::get("texture");
}

EntityWithTexture::~EntityWithTexture()
{
	delete mTexture;
	mTexture = nullptr;
}

void EntityWithTexture::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);
		mTexture->bind();
		mMesh->render();
		mTexture->unbind();
	}
}

// ─── ColorMaterialEntity ──────────────────────────────────────────────────────

bool ColorMaterialEntity::mShowNormals = false;

ColorMaterialEntity::ColorMaterialEntity(glm::vec4 color)
	: SingleColorEntity(color)
{
	mShader = Shader::get("simple_light");
}

void ColorMaterialEntity::render(const glm::mat4& modelViewMat) const
{
	if (mMesh == nullptr) return;

	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	mShader->setUniform("color", mColor);
	upload(aMat);
	mMesh->render();

	// Apt. 63: renderizar normales como segmentos amarillos si está activado
	if (mShowNormals) {
		Shader* normShader = Shader::get("normals");
		normShader->use();
		upload(aMat); // sube la misma matriz al shader de normales
		mMesh->render();
	}
}

// ─── CompoundEntity (Apt. 66) ────────────────────────────────────────────────

CompoundEntity::~CompoundEntity()
{
	// Libera todas las entidades hijas
	for (Abs_Entity* e : gObjects) delete e;
	gObjects.clear();
}

void CompoundEntity::addEntity(Abs_Entity* ae)
{
	gObjects.push_back(ae);
}

void CompoundEntity::render(const glm::mat4& modelViewMat) const
{
	// Acumula la matriz de modelado de esta entidad compuesta antes de pasar a los hijos
	mat4 aMat = modelViewMat * mModelMat;
	for (Abs_Entity* e : gObjects)
		e->render(aMat);
}

void CompoundEntity::update()
{
	for (Abs_Entity* e : gObjects)
		e->update();
}

void CompoundEntity::load()
{
	// No hay mMesh propio; carga los hijos
	for (Abs_Entity* e : gObjects)
		e->load();
}

void CompoundEntity::unload()
{
	for (Abs_Entity* e : gObjects)
		e->unload();
}