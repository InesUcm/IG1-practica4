#pragma once
#include "Entity.h"

// ─── Cubo con colores por vértice (apt. 15/16) ───
class Cube : public SingleColorEntity
{
public:
	explicit Cube(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};

// ─── Contorno de caja con textura (apt. 22-25) ───
class BoxOutline : public EntityWithTexture
{
protected:
	Texture* mTexture2;
public:
	explicit BoxOutline(GLdouble length);
	virtual ~BoxOutline() override;
	void render(const glm::mat4& modelViewMat) const override;
};

// ─── Estrella 3D con textura y animación (apt. 26-29) ───
class Star3D : public EntityWithTexture
{
public:
	explicit Star3D(GLdouble re, GLuint np, GLdouble h);
	void render(const glm::mat4& modelViewMat) const override;
	void update() override;
};

// ─── Cristalera traslúcida (apt. 32) ───
class GlassParapet : public EntityWithTexture
{
public:
	explicit GlassParapet(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};

// ─── Apt. 62/64: Cubo indexado con iluminación ───
class IndexedBox : public ColorMaterialEntity
{
public:
	explicit IndexedBox(GLdouble length);
};